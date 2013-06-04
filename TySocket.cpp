///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007.
// All right reserved!
// �� �� ����TaiyanSocket.cpp 
// ��    ����1.0
// ������������socket�������з�װ����
// �������ڣ�2008-1-2 12:10:36
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////
#include "TySocket.h"

#include <iostream>
#include <fstream>
#include <errno.h>

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

CTySocket::CTySocket()
{
    m_hSocket    = INVALID_SOCKET;
    m_bIsSvr     = false;
    m_bIsBlockingMode = true;

    m_szLastIp[0] = 0;
    m_nLastPort   = 0;
    m_nLastError  = 0;
    m_nLastRecvLength = 0;
    m_nLastSendLength = 0;
}

CTySocket::CTySocket(int s)
{
    m_hSocket    = INVALID_SOCKET;
    m_bIsSvr     = false;
    m_bIsBlockingMode = true;

    m_szLastIp[0] = 0;
    m_nLastPort   = 0;
    m_nLastError  = 0;
    m_nLastRecvLength = 0;
    m_nLastSendLength = 0;

    Attach(s);
}

CTySocket::~CTySocket(void)
{
    Close();
}

// ���� socket 
bool CTySocket::Create()
{
    Close();

    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (INVALID_SOCKET == m_hSocket) 
    {
        SetLastError();
        return false;
    }

    return true;
}

// socket �Ƿ��ѹر�
bool CTySocket::IsClosed()
{
    if(INVALID_SOCKET == m_hSocket)
        return true;

    // ������socket����δ�������ӻ�δʹ��
    if (0 == m_nLastPort)
        return false;

	if (m_bIsSvr)
	{
		// ����Ǽ���������Ͳ�����recv�����ԣ������пͻ�������ʱ���䷵��ֵ����-1
		if (SOCKET_ERROR == GetPortNumber(false))
			return true;
	}
	else
	{
		char buf[16];
		if (SOCKET_ERROR == Select_Receive(buf, 1, 0, MSG_PEEK))
			return true;
	}

    return (INVALID_SOCKET == m_hSocket);
}

// �ر� socket
void CTySocket::Close()
{
    if (INVALID_SOCKET != m_hSocket)
    {
        closesocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    //m_szLastIp[0] = 0;
    //m_nLastPort   = 0;
    m_bIsBlockingMode = true;
}

CTySocket::operator int()
{
    return m_hSocket;
}

// ʹ����һ�� socket
void CTySocket::Attach(int s)
{
    Close();
    m_hSocket = s;

    m_nLastPort = GetPortNumber(false);
    if (m_nLastPort > 0)
        strcpy(m_szLastIp, GetIpAddr(false));
    else
        Close();
}

// ����ǰ�� socket �������
int CTySocket::Detach()
{
    int s = m_hSocket;
    m_hSocket = INVALID_SOCKET;
    return s;
}

// ��
bool CTySocket::Bind(const char* lpszSocketAddress/* = INADDR_ANY*/, unsigned short nSocketPort/* = 0*/, int nReuseAddr/* = 1*/)
{
    m_bIsSvr = true;

    sockaddr_in service;
    service.sin_family = AF_INET;
    if (lpszSocketAddress == INADDR_ANY)
        service.sin_addr.s_addr = INADDR_ANY;
    else
        service.sin_addr.s_addr = inet_addr(lpszSocketAddress);

    service.sin_port = htons(nSocketPort);										// 0�����Զ�����һ���˿ں�

    if (nReuseAddr)
        setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&nReuseAddr, sizeof(int));

    if(SOCKET_ERROR == bind(m_hSocket, (sockaddr*)&service, sizeof(service)))
    {
        SetLastError();
        Close();
        return false;
    }


    if (lpszSocketAddress == INADDR_ANY)
        strcpy(m_szLastIp, "INADDR_ANY");
    else
        strcpy(m_szLastIp, lpszSocketAddress);

    m_nLastPort = nSocketPort;

    return true;
}

// ����
bool CTySocket::Listen(int nMaxListen /*= 5*/)
{
    if(SOCKET_ERROR == listen(m_hSocket, nMaxListen))
    {
        SetLastError();
        Close();
        return false;
    }

    return true;
}


// ֱ������һ�������˿�
bool CTySocket::Create_Bind_Listen(const char* lpszSocketAddress/* = INADDR_ANY*/, unsigned short nSocketPort/* = 0*/, int nMaxListen /*= 5*/)
{
    if(!Create())
        return false;

    if (!Bind(lpszSocketAddress, nSocketPort))
        return false;

    return Listen(nMaxListen);
}


// ��������
bool CTySocket::Accept(CTySocket &socketAccept)
{
    sockaddr sockAddrClient;
    socklen_t nLen = sizeof(sockaddr);
    socketAccept.m_hSocket = accept(m_hSocket, &sockAddrClient, &nLen);

    if (INVALID_SOCKET != socketAccept.m_hSocket)
    {
        strcpy(socketAccept.m_szLastIp, socketAccept.GetIpAddr(false));
        socketAccept.m_nLastPort = socketAccept.GetPortNumber(false);
        return true;
    }

    return false;
}

// ��������������
int CTySocket::Select_Accept(CTySocket &socketAccept, unsigned int uTimeOut /*= 5*1000*/)
{
    int nRet = Select(uTimeOut, CTySocket::typeRead);
    if (nRet <=0)
        return nRet;

    sockaddr sockAddrClient;
    socklen_t nLen = sizeof(sockaddr);
    socketAccept.m_hSocket = accept(m_hSocket, &sockAddrClient, &nLen);

    if (INVALID_SOCKET != socketAccept.m_hSocket)
    {
        strcpy(socketAccept.m_szLastIp, socketAccept.GetIpAddr(false));
        socketAccept.m_nLastPort = socketAccept.GetPortNumber(false);
    }

    return socketAccept.m_hSocket;
}


// ���ӵ�
bool CTySocket::Connect(const char * lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut)
{
    int s = m_hSocket;          // ������Ϊ�˶��̰߳�ȫ����ֹ��;m_hSocket����Ϊ-1
    if(INVALID_SOCKET == s)
        return false;

    m_bIsSvr = false;

    sockaddr_in skaddr = {0};

    hostent *he;														// ������Ϣ����
    he = gethostbyname(lpszHostAddress);								// he = gethostbyname("www.sina.com.cn");
    if(he == NULL)
    {
        SetLastError();
        Close();
        return false;
    }
    memcpy(&skaddr.sin_addr, he->h_addr, sizeof(he->h_addr));

    skaddr.sin_family = AF_INET;
    skaddr.sin_port	  = htons(nHostPort);								// ��֤�ֽ�˳��

    SetBlockingMode(false);
    connect(s, (sockaddr*)&skaddr, sizeof(sockaddr));
    //if(SOCKET_ERROR == connect(m_hSocket, (sockaddr*)&skaddr, sizeof(sockaddr)))
    //{
    //    SetLastError();
    //    shutdown(m_hSocket, SD_BOTH);
    //    Close();
    //    return false;
    //}
    SetBlockingMode(true);

    timeval timeout = {0};
    timeout.tv_sec = uTimeOut / 1000;
    timeout.tv_usec = uTimeOut % 1000;

    fd_set fdWrite = {0};
    FD_ZERO(&fdWrite);
    FD_SET(s, &fdWrite);

    int nRetVal = select(s+1, NULL, &fdWrite, NULL, &timeout);
    if(nRetVal>0 && FD_ISSET(s, &fdWrite))
    {
        int error;   
        int len = sizeof(error);   

        nRetVal = getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&error, (socklen_t *)&len);   
        if(nRetVal==0 && error==0)
        {
            strcpy(m_szLastIp, lpszHostAddress);
            m_nLastPort = nHostPort;
            return true;
        }
    }

    shutdown(s, SD_BOTH);
    Close();
    return false;
}

// ֱ�����ӵ�һ����ַ
bool CTySocket::Create_Connect(const char *  lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut)
{
    if (!Create())
        return false;

    return Connect(lpszHostAddress, nHostPort, uTimeOut);
}

int CTySocket::Select(unsigned int uTimeOut/* = 5*1000*/, int typeSelect/*=typetRead*/)
{
    int s = m_hSocket;          // ������Ϊ�˶��̰߳�ȫ����ֹ��;m_hSocket����Ϊ-1
    if(INVALID_SOCKET == s)
        return SOCKET_ERROR;

    timeval timeout = {0};
    timeout.tv_sec = uTimeOut / 1000;
    timeout.tv_usec = uTimeOut % 1000;

    fd_set fdsetSelect;
    FD_ZERO(&fdsetSelect);
    FD_SET(s, &fdsetSelect);

    fd_set * pFdSet[3] = {0};
    switch (typeSelect)
    {
    case typeRead:
        pFdSet[0] = &fdsetSelect;
        break;
    case typeWrite:
        pFdSet[1] = &fdsetSelect;
        break;
    case typeError:
        pFdSet[2] = &fdsetSelect;
        break;
    default:
        return SOCKET_ERROR;
    }

    // zero if the time limit expired, or SOCKET_ERROR if an error occurred
    int nRetVal = select(s+1, pFdSet[0], pFdSet[1], pFdSet[2], &timeout);
    if (nRetVal < 0)
        Close();

    return nRetVal;

    //return (FD_ISSET(m_hSocket, &fdsetSelect));
}

// ����
int CTySocket::Receive(void* lpBuf, int nBufLen, int nFlags/* = 0*/)
{
    // If the connection has been gracefully closed, the return value is zero. 
    // Otherwise, a value of SOCKET_ERROR is returned

    struct timeb tmStart;
    ftime(&tmStart);

    int nRecvLength = recv(m_hSocket, (char*)lpBuf, nBufLen, nFlags);
    if (nRecvLength > 0)
    {
        m_nLastRecvLength = nRecvLength;
        m_tmLastStart = tmStart;
        ftime(&m_tmLastEnd);
    }
    else
	{
		SetLastError();
        Close();
	}

    return nRecvLength;
}

int CTySocket::Select_Receive(void* lpBuf, int nBufLen, unsigned int uTimeOut/* = 5*1000*/, int nFlags/* = 0*/)
{
    int nSelect = Select(uTimeOut);
    if(nSelect <= 0)
        return nSelect;

    int nRecvLen = Receive(lpBuf, nBufLen, nFlags);
    if (nRecvLen <= 0)
        return SOCKET_ERROR;

    return nRecvLen;
}

// ���չ̶�����
bool CTySocket::ReceiveFixedLength(void* lpBuf, int nBufLen, int nRetryCount/* = 5*/,int nFlags/* = 0*/)
{
    int nRecvLength = 0;
    int nRetVal  = 0;

    struct timeb tmStart;
    ftime(&tmStart);

    // ���ȴ� nRetryCount ��
    for (int i=0; i<nRetryCount && (SOCKET_ERROR != nRetVal) && (nRecvLength < nBufLen); )
    {
        nRetVal = Select(1 * 1000);
        if (nRetVal == 0)
        {
            i++;
            continue;
        }

        if (nRetVal < 0)
        {
            return false;
        }

        nRetVal = Receive((char*)lpBuf+nRecvLength, nBufLen-nRecvLength, nFlags);
        if (nRetVal > 0)
            nRecvLength += nRetVal;
        else
            return false;
    }

    if (nRecvLength > 0)
    {
        m_nLastRecvLength = nRecvLength;
        m_tmLastStart = tmStart;
        ftime(&m_tmLastEnd);
    }
    return (nRecvLength == nBufLen);
}

// ����
int CTySocket::Send(const void* lpBuf, int nBufLen, unsigned int uTimeOut/* = 5*1000*/, int nFlags/* = 0*/)
{
    int nRetVal = Select(uTimeOut, typeWrite);
    if (nRetVal > 0)
        nRetVal = send(m_hSocket, (char*)lpBuf, nBufLen, nFlags);

    return nRetVal;
}

int CTySocket::SendFixedLength(const void* lpBuf, int nBufLen, unsigned int uTimeOut, int nMaxRetryCount)
{
    int nSendedLength = 0;
    int nRetVal     = 0;
	int nRetryCount = 0;
    do 
	{
		nRetVal = Send((char*)lpBuf + nSendedLength, nBufLen - nSendedLength, uTimeOut);
		if (nRetVal < 256)
			nRetryCount++;
    } while ((SOCKET_ERROR != nRetVal) && ((nSendedLength += nRetVal) < nBufLen) && (nRetryCount<nMaxRetryCount));

    if (nSendedLength > 0)
        m_nLastSendLength = nSendedLength;

    return nSendedLength;
}

// ��ñ�socket����Ӧ��IP��ַ
// bool bIsSvr Ϊ true ��ʾΪ��������falseΪ�ͻ���
char * CTySocket::GetIpAddr( bool bIsUsingLast/*=true*/ )
{
    if(bIsUsingLast)
        return m_szLastIp;

    sockaddr_in skaddr = {0};
    socklen_t nLen = sizeof(sockaddr_in);

    if (m_bIsSvr)
        getsockname(m_hSocket, (sockaddr*)&skaddr, &nLen);
    else
        getpeername(m_hSocket, (sockaddr*)&skaddr, &nLen);

	// inet_ntoa�ǰ�ȫ�ģ����ֵ��0�����ص�ַ��0.0.0.0
	return inet_ntoa(skaddr.sin_addr);
}

// ��ñ�socket��ʹ�õĶ˿ں�
// bool bIsSvr Ϊ true ��ʾΪ��������falseΪ�ͻ���
int CTySocket::GetPortNumber( bool bIsUsingLast/*=true*/ )
{
    if(bIsUsingLast)
        return m_nLastPort;

    sockaddr_in skaddr = {0};
    socklen_t nLen = sizeof(sockaddr_in);

    if (m_bIsSvr)
    {
        if(SOCKET_ERROR  == getsockname(m_hSocket, (sockaddr*)&skaddr, &nLen))
        {
            SetLastError();
            return SOCKET_ERROR;
        }
    }
    else
    {
        if(SOCKET_ERROR  == getpeername(m_hSocket, (sockaddr*)&skaddr, &nLen))
        {
            SetLastError();
            return SOCKET_ERROR;
        }
    }

    return ntohs(skaddr.sin_port);
}

// ��û����С
int CTySocket::GetReservedBufSize(int type)
{
    int nBufLen = 0;
    int optname = (type==typeRead) ? SO_RCVBUF : SO_SNDBUF;

#ifdef WIN32
    int nOptRetVal = sizeof(int);
    /*int nErr =*/ getsockopt(m_hSocket, SOL_SOCKET, optname, (char*)&nBufLen, &nOptRetVal);
#else
    socklen_t nOptRetVal = sizeof(int);
    /*int nErr =*/ getsockopt(m_hSocket, SOL_SOCKET, optname, (void*)&nBufLen, &nOptRetVal);
#endif

    return nBufLen;
}

// ���û����С
void CTySocket::SetReservedBufSize(int type, int size)
{
    int optname = (type==typeRead) ? SO_RCVBUF : SO_SNDBUF;
    /*int nRet =*/ setsockopt(m_hSocket, SOL_SOCKET, optname, (const char*)&size, sizeof(int));
}

void CTySocket::SetBlockingMode(bool bIsBlocking)
{
#ifdef WIN32
    unsigned long iMode  = bIsBlocking ? 0 : 1;   
    /*int nRetVal =*/ ioctlsocket(m_hSocket, FIONBIO, &iMode );   
#else
    //int flags = fcntl(m_hSocket, F_GETFL, 0);   
    //if (bIsBlocking)
    //    flags ^= ~O_NONBLOCK;
    //else
    //    flags |= O_NONBLOCK;

    fcntl(m_hSocket, F_SETFL, bIsBlocking? 0 : O_NONBLOCK);
#endif

    m_bIsBlockingMode = bIsBlocking;
}

bool CTySocket::IsBlocking()
{
    return m_bIsBlockingMode;
}


// ��ȡ���η��͵ĳ���
int CTySocket::GetSendLength()
{
    return m_nLastSendLength;
}

// ��ȡ���ν��յĳ���
int CTySocket::GetRecvLength()
{
    return m_nLastRecvLength;
}

// ��ȡ���ν�����ʱ
int CTySocket::GetRecvInterval()
{
    return abs((int)((m_tmLastEnd.time & 0xFFFFFF)*1000 + m_tmLastEnd.millitm)  - (int)((m_tmLastStart.time & 0xFFFFFF)*1000 - m_tmLastStart.millitm));
}

// ��ȡ���ν��յ��ٶ� �ֽ�/��
int CTySocket::GetRecvSpeed()
{
    int milli_second = GetRecvInterval();
    if (milli_second == 0)
        milli_second = 1;

    return int((1000.0 *m_nLastRecvLength) / milli_second);
}

// ���õ�ǰ������Ϣ
void CTySocket::SetLastError()
{
#ifdef WIN32
    m_nLastError = WSAGetLastError();
#else
    m_nLastError = errno;
#endif
}

// ���ش�����Ϣ
const char * CTySocket::GetLastErrMsg()
{
#ifdef WIN32
    static string strErrMsg;
	LPVOID lpMsgBuf;
	if (!FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, m_nLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *) &lpMsgBuf, 0, NULL))
	{
		return " δ֪�Ĵ������� ";
	}

	strErrMsg = (char *)lpMsgBuf;
	LocalFree( lpMsgBuf );
    return strErrMsg.c_str();
#else
	return strerror(m_nLastError);
#endif
}

// ��ñ���IP��ַ
char * CTySocket::GetLocalHostIp() 
{
#ifdef WIN32
    char szLocalHostName[256];
    if(gethostname(szLocalHostName,sizeof(szLocalHostName)) == SOCKET_ERROR)		// ��ȡ������������
        return NULL;

    struct hostent *host_entry = gethostbyname(szLocalHostName);					    // �����������ݿ��еõ���Ӧ�ġ�������
    for( int i = 0; host_entry!=NULL && host_entry->h_addr_list!=NULL; i++ )    // ѭ���ó����ػ�������IP��ַ
    {
        const char *pszLocalAddr = inet_ntoa(*(struct in_addr *)host_entry->h_addr_list[i]);

        if (host_entry->h_addr_list[i]+host_entry->h_length >= host_entry->h_name)
            return (char *)pszLocalAddr;
    }

    return NULL;

#else

    int sock; 
    struct sockaddr_in sin; 
    struct ifreq ifr;
    //static char szLocalHostIp[100]  = {0};

    sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if(sock >= 0)   //!<0
    {
        for(int i=0; i<100; i++)
        {
            sprintf(ifr.ifr_name, "eth%d", i);
            if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
                break;
            
            memcpy(&sin, &ifr.ifr_addr, sizeof(sin));

            const char *pszLocalAddr = inet_ntoa(sin.sin_addr);
            if(pszLocalAddr)
            {
                close(sock);
                return (char *)pszLocalAddr;
            }
        }
        close(sock);
    }
#endif
}

// ����һ���ļ�
bool CTySocket::TransmitFile(const char *pszFilePathName)
{
    ifstream fileStatic(pszFilePathName, ios::binary|ios::out);
    if (!fileStatic.is_open())
        return false;

    const int BUFFER_LENGTH = 1024;
    char buf[BUFFER_LENGTH*2];
    int len = 0;
    while(!IsClosed() 
        && (len=fileStatic.read(buf, BUFFER_LENGTH).gcount()) > 0)
    {
        if(!SendFixedLength(buf, len, 3*1000, 5))      // 15�볬ʱ����Ϊ�������
        {
            fileStatic.close();
            return false;
        }
    }

    fileStatic.close();
    return true;
}

// ��ʼ��
bool InitSocket()
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);

    return (0 == err);
#else
    return true;
#endif
}

void CleanupSocket()
{
#ifdef WIN32
    WSACleanup();
#endif
}


//////////////////////////////////////////////////////////////////////////
CTySocketPool::CTySocketPool() : m_nPoolSize(0), m_pTySocketPool(NULL), m_pbIsUsing(NULL)
{

}

CTySocketPool::~CTySocketPool()
{
    FreePool();
}

void CTySocketPool::SetSize( int size )
{
    FreePool();
    if (size > 0)
    {
        m_nPoolSize = size;
        m_pTySocketPool = new CTySocket[m_nPoolSize];
        m_pbIsUsing = new bool [m_nPoolSize] ;
        memset(m_pbIsUsing, 0, sizeof(bool)*m_nPoolSize);
    }
}

void CTySocketPool::FreePool()
{
    if (m_pTySocketPool)
    {
        delete [] m_pTySocketPool;
        m_pTySocketPool = NULL;
        delete [] m_pbIsUsing;
        m_pbIsUsing = NULL;
    }

    m_nPoolSize = 0;
}


CTySocket * CTySocketPool::GetFreeSocket()
{
    static int nLastIndex = 0;
    for (int i=nLastIndex+1; i!=nLastIndex; ++i)
    {
        if (i >= m_nPoolSize)
            i = 0;

        if (!m_pbIsUsing[i] && m_pTySocketPool[i].IsClosed())
        {
            nLastIndex = i;
            m_pbIsUsing[i] = true;
            return &m_pTySocketPool[i];
        }
    }

    return NULL;
}

CTySocket * CTySocketPool::operator [] (int index)
{
    if (index>=0 && index<m_nPoolSize)
        return &m_pTySocketPool[index];

    return NULL;
}

int CTySocketPool::GetFreeCount()
{
    int count = 0;
    for (int i=0; i<m_nPoolSize; ++i)
    {
        if (m_pTySocketPool[i].IsClosed())
            ++count;
    }

    return count;
}

void CTySocketPool::FreeSocket(int index)
{
    if (index>=0 && index<m_nPoolSize)
    {
        m_pTySocketPool[index].Close();
        m_pbIsUsing[index] = false;
    }
}

bool CTySocketPool::IsUsing( int index )
{
    if (index>=0 && index<m_nPoolSize)
        return m_pbIsUsing[index];

    return false;
}
