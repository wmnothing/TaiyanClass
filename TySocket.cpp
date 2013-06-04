///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007.
// All right reserved!
// 文 件 名：TaiyanSocket.cpp 
// 版    本：1.0
// 功能描述：对socket操作进行封装的类
// 创建日期：2008-1-2 12:10:36
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 修 改 者：
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

// 创建 socket 
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

// socket 是否已关闭
bool CTySocket::IsClosed()
{
    if(INVALID_SOCKET == m_hSocket)
        return true;

    // 已生成socket，但未建立连接或还未使用
    if (0 == m_nLastPort)
        return false;

	if (m_bIsSvr)
	{
		// 如果是监听情况，就不能用recv来测试，否则当有客户连上来时，其返回值会是-1
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

// 关闭 socket
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

// 使用另一个 socket
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

// 将当前的 socket 分离出来
int CTySocket::Detach()
{
    int s = m_hSocket;
    m_hSocket = INVALID_SOCKET;
    return s;
}

// 绑定
bool CTySocket::Bind(const char* lpszSocketAddress/* = INADDR_ANY*/, unsigned short nSocketPort/* = 0*/, int nReuseAddr/* = 1*/)
{
    m_bIsSvr = true;

    sockaddr_in service;
    service.sin_family = AF_INET;
    if (lpszSocketAddress == INADDR_ANY)
        service.sin_addr.s_addr = INADDR_ANY;
    else
        service.sin_addr.s_addr = inet_addr(lpszSocketAddress);

    service.sin_port = htons(nSocketPort);										// 0代表自动分配一个端口号

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

// 监听
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


// 直接启动一个监听端口
bool CTySocket::Create_Bind_Listen(const char* lpszSocketAddress/* = INADDR_ANY*/, unsigned short nSocketPort/* = 0*/, int nMaxListen /*= 5*/)
{
    if(!Create())
        return false;

    if (!Bind(lpszSocketAddress, nSocketPort))
        return false;

    return Listen(nMaxListen);
}


// 接受连接
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

// 非阻塞接受连接
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


// 连接到
bool CTySocket::Connect(const char * lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut)
{
    int s = m_hSocket;          // 这样是为了多线程安全，防止中途m_hSocket被改为-1
    if(INVALID_SOCKET == s)
        return false;

    m_bIsSvr = false;

    sockaddr_in skaddr = {0};

    hostent *he;														// 主机信息类型
    he = gethostbyname(lpszHostAddress);								// he = gethostbyname("www.sina.com.cn");
    if(he == NULL)
    {
        SetLastError();
        Close();
        return false;
    }
    memcpy(&skaddr.sin_addr, he->h_addr, sizeof(he->h_addr));

    skaddr.sin_family = AF_INET;
    skaddr.sin_port	  = htons(nHostPort);								// 保证字节顺序

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

// 直接连接到一个地址
bool CTySocket::Create_Connect(const char *  lpszHostAddress, unsigned short nHostPort, unsigned int uTimeOut)
{
    if (!Create())
        return false;

    return Connect(lpszHostAddress, nHostPort, uTimeOut);
}

int CTySocket::Select(unsigned int uTimeOut/* = 5*1000*/, int typeSelect/*=typetRead*/)
{
    int s = m_hSocket;          // 这样是为了多线程安全，防止中途m_hSocket被改为-1
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

// 接收
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

// 接收固定长度
bool CTySocket::ReceiveFixedLength(void* lpBuf, int nBufLen, int nRetryCount/* = 5*/,int nFlags/* = 0*/)
{
    int nRecvLength = 0;
    int nRetVal  = 0;

    struct timeb tmStart;
    ftime(&tmStart);

    // 最多等待 nRetryCount 秒
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

// 发送
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

// 获得本socket所对应的IP地址
// bool bIsSvr 为 true 表示为服务器，false为客户端
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

	// inet_ntoa是安全的，如果值是0，返回地址是0.0.0.0
	return inet_ntoa(skaddr.sin_addr);
}

// 获得本socket所使用的端口号
// bool bIsSvr 为 true 表示为服务器，false为客户端
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

// 获得缓存大小
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

// 设置缓存大小
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


// 获取本次发送的长度
int CTySocket::GetSendLength()
{
    return m_nLastSendLength;
}

// 获取本次接收的长度
int CTySocket::GetRecvLength()
{
    return m_nLastRecvLength;
}

// 获取本次接收用时
int CTySocket::GetRecvInterval()
{
    return abs((int)((m_tmLastEnd.time & 0xFFFFFF)*1000 + m_tmLastEnd.millitm)  - (int)((m_tmLastStart.time & 0xFFFFFF)*1000 - m_tmLastStart.millitm));
}

// 获取本次接收的速度 字节/秒
int CTySocket::GetRecvSpeed()
{
    int milli_second = GetRecvInterval();
    if (milli_second == 0)
        milli_second = 1;

    return int((1000.0 *m_nLastRecvLength) / milli_second);
}

// 设置当前错误信息
void CTySocket::SetLastError()
{
#ifdef WIN32
    m_nLastError = WSAGetLastError();
#else
    m_nLastError = errno;
#endif
}

// 返回错误信息
const char * CTySocket::GetLastErrMsg()
{
#ifdef WIN32
    static string strErrMsg;
	LPVOID lpMsgBuf;
	if (!FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, m_nLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *) &lpMsgBuf, 0, NULL))
	{
		return " 未知的错误类型 ";
	}

	strErrMsg = (char *)lpMsgBuf;
	LocalFree( lpMsgBuf );
    return strErrMsg.c_str();
#else
	return strerror(m_nLastError);
#endif
}

// 获得本地IP地址
char * CTySocket::GetLocalHostIp() 
{
#ifdef WIN32
    char szLocalHostName[256];
    if(gethostname(szLocalHostName,sizeof(szLocalHostName)) == SOCKET_ERROR)		// 获取本地主机名称
        return NULL;

    struct hostent *host_entry = gethostbyname(szLocalHostName);					    // 从主机名数据库中得到对应的“主机”
    for( int i = 0; host_entry!=NULL && host_entry->h_addr_list!=NULL; i++ )    // 循环得出本地机器所有IP地址
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

// 传送一个文件
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
        if(!SendFixedLength(buf, len, 3*1000, 5))      // 15秒超时，认为网络断了
        {
            fileStatic.close();
            return false;
        }
    }

    fileStatic.close();
    return true;
}

// 初始化
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
