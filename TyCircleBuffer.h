#pragma once

class CTyCircleBuffer
{
public:
    CTyCircleBuffer(void);
    CTyCircleBuffer(int size);
    ~CTyCircleBuffer(void);

    bool Alloc(int size);
    void Free();
    void Reset();

    int Read(char *pDest, int len);
    bool Write(char *pSrc, int len);

    char * GetReadDataPtr();
    char * GetWriteDataPtr();

    bool IncDataLenWithoutWrite(int len);
    bool DecDataLenWithoutRead(int len);

    int GetMaxDataLen();
    int GetDataBufferLen();
    int GetAvailableBufferLen();

    int GetContinuousAvailableBufferLen();
    int GetContinuousDataBufferLen();

    char operator[] (int index);

    int memcmp (const void * buf, int count);
    void * memcpy (void * dst, int count);
    bool IsContinuousDataLen(int len);
protected:
    char *m_pBufBaseAddr;           // 内存首地址
    char *m_pBufTailAddr;           // 内存尾地址
    int m_nBufferSize;              // 内存大小

    volatile char * m_pHead;	    // 头指针
    volatile char * m_pTail;	    // 尾指针

    volatile bool m_bWritingData;	// 是否正在写数据;
    volatile bool m_bReadingData;	// 是否正在读数据;
};
