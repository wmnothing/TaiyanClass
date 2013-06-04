///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2009.
// All right reserved!
// 文 件 名：TyCircleBuffer.cpp
// 版    本：1.0
// 功能描述：内存循环使用类
// 创建日期：2009-8-1 11:33:29
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////

#include "TyCircleBuffer.h"
#include <stddef.h>
#include <memory.h>

CTyCircleBuffer::CTyCircleBuffer(void)
{
    m_pBufBaseAddr = NULL;
    m_nBufferSize = 0;
    Free();
}

CTyCircleBuffer::CTyCircleBuffer(int size)
{
    m_pBufBaseAddr = NULL;
    m_nBufferSize = 0;
    Alloc(size);
}

CTyCircleBuffer::~CTyCircleBuffer(void)
{
    Free();
}

// 分配内存
bool CTyCircleBuffer::Alloc(int size)
{
    if (m_nBufferSize != size)
    {
        Free();

        // 多一个字节，以便保证m_pHead和m_pTail相等时表示数据为空
        m_nBufferSize  = size;
        m_pBufBaseAddr = new char[m_nBufferSize+32];
        m_pBufTailAddr = m_pBufBaseAddr + m_nBufferSize + 1;
    }

    Reset();

    return (m_pBufBaseAddr != NULL);
}

// 释放内存
void CTyCircleBuffer::Free()
{
    delete[] m_pBufBaseAddr;

    m_pBufBaseAddr = NULL;
    m_pBufTailAddr = NULL;
    m_nBufferSize  = 0;

    Reset();
}

// 重置，即抛弃原有数据，重新开始
void CTyCircleBuffer::Reset()
{
    if(m_pBufBaseAddr != NULL)
    {
        m_pHead = m_pBufBaseAddr;
        m_pTail = m_pBufBaseAddr;
    }
    else
    {
        m_pHead = NULL;
        m_pTail = NULL;
    }

    m_bWritingData = false;
    m_bReadingData = false;
}

// 内存大小
int CTyCircleBuffer::GetMaxDataLen()
{
    return m_nBufferSize;
}

// 获得数据长度
int CTyCircleBuffer::GetDataBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    char *pVirtualTail = (pTail>=pHead) ? pTail : (pTail+m_nBufferSize+1);
    return (pVirtualTail - pHead);
}

// 获得剩余空间长度
int CTyCircleBuffer::GetAvailableBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pHead == pTail)
        return m_nBufferSize;

    char *pVirtualTail = (pTail>=pHead) ? pTail : (pTail+m_nBufferSize+1);
    return m_nBufferSize - (pVirtualTail-pHead);
}

// 获得可连续写的内存长度
int CTyCircleBuffer::GetContinuousAvailableBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pHead > pTail)
        return (pHead - pTail) - 1;

    if ((pTail==pHead) && (pHead==m_pBufBaseAddr))
            return m_nBufferSize;

    return m_pBufTailAddr-pTail;
}

// 获得可连续读的内存长度
int CTyCircleBuffer::GetContinuousDataBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pTail>=pHead)
        return pTail-pHead;

    return m_pBufTailAddr-pHead;
}

// 是否为连续的数据长度
bool CTyCircleBuffer::IsContinuousDataLen( int len )
{
    return m_pHead+len < m_pBufTailAddr;
}


// 读取内存
int CTyCircleBuffer::Read(char *pDest, int len)
{
    if (GetDataBufferLen() < len)
        return 0;

    // 正在读数据或没有数据，返回
    if(m_bReadingData || len<=0)
        return 0;

    // 防止函数执行时有数据写入导致地址发生变化
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if(pHead == pTail)
        return 0;

    m_bReadingData = true;

    int nCopyLength = 0;
    if (pHead < pTail)     // 内存完整连在一起
    {
        while (pHead<pTail && len--)
        {
            *pDest++ = *pHead++;
            nCopyLength++;
        }
    }
    else                            // 内存需要拼接
    {
        // 复制到内存尾
        while (pHead<m_pBufTailAddr && len--)
        {
            *pDest++ = *pHead++;
            nCopyLength++;
        }

        if (len > 0)
        {
            // 从内存头开始复制
            pHead = m_pBufBaseAddr;
            while (pHead<pTail && len--)
            {
                *pDest++ = *pHead++;
                nCopyLength++;
            }
        }
    }

    m_pHead = pHead;
    m_bReadingData = false;
    return nCopyLength;
}

// 向内存中写入数据
bool CTyCircleBuffer::Write(char *pSrc, int len)
{
    if(m_bWritingData || len <= 0)
        return false;

    //如果数据长度大于内存空间剩余长度，返回
    if(len > GetAvailableBufferLen())
        return false;

    //正在写数据
    m_bWritingData = true;

    //char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pTail+len < m_pBufTailAddr)
    {
        while (len--)
            *pTail++ = *pSrc++;
    }
    else
    {
        // 复制到内存尾
        while (pTail<m_pBufTailAddr && len--)
            *pTail++ = *pSrc++;

        if (len > 0)
        {
            // 从内存头开始复制
            pTail = m_pBufBaseAddr;
            while (len--)
                *pTail++ = *pSrc++;
        }
    }

    m_pTail = pTail;
    //数据已写完
    m_bWritingData = false;	
    return true;
}


// 不写数据却增加数据长度（相当于在尾部增加数据）
bool CTyCircleBuffer::IncDataLenWithoutWrite(int len)
{
    if(len <= 0)
        return false;

    m_pTail += len;
    if(m_pTail >= m_pBufTailAddr)
        m_pTail -= m_nBufferSize+1;

    return true;
}

// 不读数据却减少数据长度（相当于从头部扔掉数据）
bool CTyCircleBuffer::DecDataLenWithoutRead(int len)
{
    if(len <= 0)
        return false;

    int nDataLen = GetDataBufferLen();
    if (len > nDataLen)
        len = nDataLen;

    m_pHead += len;
    if(m_pHead >= m_pBufTailAddr)
        m_pHead -= m_nBufferSize+1;

    return true;
}

// 得到读取数据的开始指针
char * CTyCircleBuffer::GetReadDataPtr()
{
    return (char *)m_pHead;
}

// 得到写入数据的开始指针
char * CTyCircleBuffer::GetWriteDataPtr()
{
    return (char *)m_pTail;
}

// 直接获取以m_pHead为首地址的某个字节
char CTyCircleBuffer::operator[] (int index)
{
    // 已经回到内存首地址了
    if ((m_pHead + index) >= m_pBufTailAddr)
        return m_pBufBaseAddr[index - (m_pBufTailAddr-m_pHead)];

    return m_pHead[index];
}

// 比较内存
int CTyCircleBuffer::memcmp(const void * buf, int count)
{
    if (!count)
        return(0);

    char *buf1 = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *buf2 = (char *)buf;

    bool bMoreCmp = false;
    while ((buf1<m_pBufTailAddr) && --count && (bMoreCmp=(*buf1 == *buf2)))
    {
        buf1++;
        buf2++;
    }

    if (bMoreCmp && (count>0))
    {
        buf1 = m_pBufBaseAddr;

        while ( --count && (*buf1 == *buf2))
        {
            buf1++;
            buf2++;
        }
    }

    return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );
}

// 内存复制，将里面的复制到外面
void * CTyCircleBuffer::memcpy (void * dst, int count)
{
    char * src  = (char *) (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;;
    char * dest = (char *) dst;

    while (src<m_pBufTailAddr && count--) 
        *dest++ = *src++;

    if (count > 0)
    {
        src = m_pBufBaseAddr;
        while (src<m_pBufTailAddr && count--) 
            *dest++ = *src++;
    }

    return(dst);
}
