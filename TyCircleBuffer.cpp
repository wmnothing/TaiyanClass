///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2009.
// All right reserved!
// �� �� ����TyCircleBuffer.cpp
// ��    ����1.0
// �����������ڴ�ѭ��ʹ����
// �������ڣ�2009-8-1 11:33:29
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
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

// �����ڴ�
bool CTyCircleBuffer::Alloc(int size)
{
    if (m_nBufferSize != size)
    {
        Free();

        // ��һ���ֽڣ��Ա㱣֤m_pHead��m_pTail���ʱ��ʾ����Ϊ��
        m_nBufferSize  = size;
        m_pBufBaseAddr = new char[m_nBufferSize+32];
        m_pBufTailAddr = m_pBufBaseAddr + m_nBufferSize + 1;
    }

    Reset();

    return (m_pBufBaseAddr != NULL);
}

// �ͷ��ڴ�
void CTyCircleBuffer::Free()
{
    delete[] m_pBufBaseAddr;

    m_pBufBaseAddr = NULL;
    m_pBufTailAddr = NULL;
    m_nBufferSize  = 0;

    Reset();
}

// ���ã�������ԭ�����ݣ����¿�ʼ
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

// �ڴ��С
int CTyCircleBuffer::GetMaxDataLen()
{
    return m_nBufferSize;
}

// ������ݳ���
int CTyCircleBuffer::GetDataBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    char *pVirtualTail = (pTail>=pHead) ? pTail : (pTail+m_nBufferSize+1);
    return (pVirtualTail - pHead);
}

// ���ʣ��ռ䳤��
int CTyCircleBuffer::GetAvailableBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pHead == pTail)
        return m_nBufferSize;

    char *pVirtualTail = (pTail>=pHead) ? pTail : (pTail+m_nBufferSize+1);
    return m_nBufferSize - (pVirtualTail-pHead);
}

// ��ÿ�����д���ڴ泤��
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

// ��ÿ����������ڴ泤��
int CTyCircleBuffer::GetContinuousDataBufferLen()
{
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if (pTail>=pHead)
        return pTail-pHead;

    return m_pBufTailAddr-pHead;
}

// �Ƿ�Ϊ���������ݳ���
bool CTyCircleBuffer::IsContinuousDataLen( int len )
{
    return m_pHead+len < m_pBufTailAddr;
}


// ��ȡ�ڴ�
int CTyCircleBuffer::Read(char *pDest, int len)
{
    if (GetDataBufferLen() < len)
        return 0;

    // ���ڶ����ݻ�û�����ݣ�����
    if(m_bReadingData || len<=0)
        return 0;

    // ��ֹ����ִ��ʱ������д�뵼�µ�ַ�����仯
    char *pHead = (m_pHead < m_pBufTailAddr) ? (char *)m_pHead : m_pBufBaseAddr;
    char *pTail = (m_pTail < m_pBufTailAddr) ? (char *)m_pTail : m_pBufBaseAddr;

    if(pHead == pTail)
        return 0;

    m_bReadingData = true;

    int nCopyLength = 0;
    if (pHead < pTail)     // �ڴ���������һ��
    {
        while (pHead<pTail && len--)
        {
            *pDest++ = *pHead++;
            nCopyLength++;
        }
    }
    else                            // �ڴ���Ҫƴ��
    {
        // ���Ƶ��ڴ�β
        while (pHead<m_pBufTailAddr && len--)
        {
            *pDest++ = *pHead++;
            nCopyLength++;
        }

        if (len > 0)
        {
            // ���ڴ�ͷ��ʼ����
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

// ���ڴ���д������
bool CTyCircleBuffer::Write(char *pSrc, int len)
{
    if(m_bWritingData || len <= 0)
        return false;

    //������ݳ��ȴ����ڴ�ռ�ʣ�೤�ȣ�����
    if(len > GetAvailableBufferLen())
        return false;

    //����д����
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
        // ���Ƶ��ڴ�β
        while (pTail<m_pBufTailAddr && len--)
            *pTail++ = *pSrc++;

        if (len > 0)
        {
            // ���ڴ�ͷ��ʼ����
            pTail = m_pBufBaseAddr;
            while (len--)
                *pTail++ = *pSrc++;
        }
    }

    m_pTail = pTail;
    //������д��
    m_bWritingData = false;	
    return true;
}


// ��д����ȴ�������ݳ��ȣ��൱����β���������ݣ�
bool CTyCircleBuffer::IncDataLenWithoutWrite(int len)
{
    if(len <= 0)
        return false;

    m_pTail += len;
    if(m_pTail >= m_pBufTailAddr)
        m_pTail -= m_nBufferSize+1;

    return true;
}

// ��������ȴ�������ݳ��ȣ��൱�ڴ�ͷ���ӵ����ݣ�
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

// �õ���ȡ���ݵĿ�ʼָ��
char * CTyCircleBuffer::GetReadDataPtr()
{
    return (char *)m_pHead;
}

// �õ�д�����ݵĿ�ʼָ��
char * CTyCircleBuffer::GetWriteDataPtr()
{
    return (char *)m_pTail;
}

// ֱ�ӻ�ȡ��m_pHeadΪ�׵�ַ��ĳ���ֽ�
char CTyCircleBuffer::operator[] (int index)
{
    // �Ѿ��ص��ڴ��׵�ַ��
    if ((m_pHead + index) >= m_pBufTailAddr)
        return m_pBufBaseAddr[index - (m_pBufTailAddr-m_pHead)];

    return m_pHead[index];
}

// �Ƚ��ڴ�
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

// �ڴ渴�ƣ�������ĸ��Ƶ�����
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
