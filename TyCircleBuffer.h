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
    char *m_pBufBaseAddr;           // �ڴ��׵�ַ
    char *m_pBufTailAddr;           // �ڴ�β��ַ
    int m_nBufferSize;              // �ڴ��С

    volatile char * m_pHead;	    // ͷָ��
    volatile char * m_pTail;	    // βָ��

    volatile bool m_bWritingData;	// �Ƿ�����д����;
    volatile bool m_bReadingData;	// �Ƿ����ڶ�����;
};
