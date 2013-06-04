#pragma once
#include <sys/timeb.h>

#ifdef WIN32
    #include <winsock.h>
    #pragma comment(lib, "Ws2_32.lib")

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <net/if.h>
    #include <sys/ioctl.h>

    #define closesocket close
    #define INVALID_SOCKET          (-1)
    #define SOCKET_ERROR			(-1)
#endif

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

class CTySocket
{
public:
	CTySocket(int s);
	CTySocket();
	~CTySocket(void);

	bool Create();

	bool Bind(const char* lpszSocketAddress = INADDR_ANY, unsigned short nSocketPort = 0, int nReuseAddr = 1);
	bool Listen(int nMaxListen = 5);
    bool Create_Bind_Listen(const char* lpszSocketAddress = INADDR_ANY, unsigned short nSocketPort = 0, int nMaxListen = 5);
	bool Accept(CTySocket &socketAccept);


	bool Connect(const char *  lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut = 5*1000);
    bool Create_Connect(const char *  lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut = 5*1000);

	int Select(unsigned int uTimeOut = 5*1000, int typeSelect=typeRead);
	int Receive(void* lpBuf, int nBufLen, int nFlags = 0);

    int Select_Receive(void* lpBuf, int nBufLen, unsigned int uTimeOut = 5*1000, int nFlags = 0);
	bool ReceiveFixedLength(void* lpBuf, int nBufLen, int nRetryCount = 5,int nFlags = 0);

    int Select_Accept(CTySocket &socketAccept, unsigned int uTimeOut = 5*1000);

#ifdef WIN32
    int Send(const void* lpBuf, int nBufLen, unsigned int uTimeOut=5*1000, int nFlags = 0);
#else
    int Send(const void* lpBuf, int nBufLen, unsigned int uTimeOut=5*1000, int nFlags = MSG_NOSIGNAL);
#endif
    int SendFixedLength(const void* lpBuf, int nBufLen, unsigned int uTimeOut=1000, int nMaxRetryCount=5);

	bool IsClosed();
	void Close();

    char * GetIpAddr(bool bIsUsingLast=true);
    int GetPortNumber(bool bIsUsingLast=true);

	int GetSendLength();
	int GetRecvLength();
	int GetRecvInterval();
	int GetRecvSpeed();

    int Detach();
    void Attach(int s);

    int  GetReservedBufSize(int type);
    void SetReservedBufSize(int type, int size);

    void SetBlockingMode(bool bIsBlocking);
    bool IsBlocking();

    void SetLastError();
    const char * GetLastErrMsg();

    operator int ();

    static char * GetLocalHostIp();
    bool TransmitFile(const char *pszFilePathName);

public:
	int m_hSocket;
    enum {typeRead, typeWrite, typeError};

private:
	struct timeb m_tmLastStart;
	struct timeb m_tmLastEnd;

	int m_nLastRecvLength;
	int m_nLastSendLength;

    bool m_bIsSvr;
    int m_nLastError;
    bool m_bIsBlockingMode;

    char m_szLastIp[64];
    int  m_nLastPort;
};

bool InitSocket();
void CleanupSocket();


class CTySocketPool
{
public:
    CTySocketPool();
    ~CTySocketPool();

    void SetSize(int size);
    int GetSize(){return m_nPoolSize;}
    int GetFreeCount();

    CTySocket * GetFreeSocket();
    CTySocket * operator [] (int index);
    bool IsUsing(int index);

    void FreeSocket(int index);

private:
    void FreePool();

private:
    CTySocket * m_pTySocketPool;
    bool * m_pbIsUsing;
    int m_nPoolSize;
};
