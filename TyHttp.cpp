///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2012.
// All right reserved!
// 文 件 名：TyHttp.cpp
// 版    本：1.0
// 功能描述：
// 创建日期：2012-5-7 13:31:38
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////

#include "TyHttp.h"
#include <sys/stat.h>
#include <time.h>
#include <fstream>
#include "win_linux.h"


CTyHttpSvr::CTyHttpSvr(void)
{
}

CTyHttpSvr::~CTyHttpSvr(void)
{
}

// 设置主目录
void CTyHttpSvr::SetMainPath( const char *pszMainPath )
{
    m_strMainPath = pszMainPath;
}

// 添加虚拟目录
void CTyHttpSvr::AddVirtailPath( const char * pszVirtailPathName, const char * pszVirtualPath )
{
    m_mapVirtualPathName[pszVirtualPath] = pszVirtailPathName;
}

// 添加默认页面文件
void CTyHttpSvr::AddDefaultDocument( const char * pszDefaultDocument )
{
    m_listDefaultDocument.push_back(pszDefaultDocument);
}

// 接收请求
bool CTyHttpSvr::RecvRequest()
{
    if (GetReservedBufSize(CTySocket::typeWrite) < 1024*100)
        SetReservedBufSize(CTySocket::typeWrite, 1024*100);

    char *pszRequest = new char[1024*10];
    char *pCurrent = pszRequest;
    while(!IsClosed())
    {
        // 每次只接收1字节
        if (1 == Select_Receive(pCurrent, 1))
        {
            ++pCurrent;

            // 接完4个字节后,可判断是否已全接完
            if((pCurrent>pszRequest+4) && (memcmp(pCurrent-4, "\r\n\r\n", 4)==0))
                break;
        }
    }

    ParseRequest(pszRequest);

    if (m_mapRequestKeyValue["RequestType"] == "POST")
    {
        m_mapRequestKeyValue["RequestUrl"] += "?";
        int nRecvLen = Select_Receive(pszRequest, 1024);
        if (nRecvLen > 0)
        {
            pszRequest[nRecvLen] = '\0';
            m_mapRequestKeyValue["RequestUrl"] += pszRequest;
        }
    }

    delete [] pszRequest;
    return true;
}

// 解析请求
bool CTyHttpSvr::ParseRequest(const char * pszRequest)
{
    string strRequestType;

    char sep_CR[] = "\r\n";
    char *token = strtok((char *)pszRequest, sep_CR);

    // 第一行，请求方式/请求的文件
    if (token)
        strRequestType = token;

    token = strtok(NULL, sep_CR);
    while(token)
    {
        char *pKey = token;
        char *pColon = (char *)memchr(token, ':', strlen(token));
        if (pColon)
        {
            *pColon = '\0';
            char *pValue = pColon+1;

            m_mapRequestKeyValue[pKey] = pValue;
        }
    }

    char sep_BLANK[] = " ";
    token = strtok((char *)strRequestType.c_str(), sep_BLANK);
    if (token)
        m_mapRequestKeyValue["RequestType"] = token;

    token = strtok(NULL, sep_BLANK);
    if (token)
        m_mapRequestKeyValue["RequestUrl"] = token;

    token = strtok(NULL, sep_BLANK);
    if (token)
        m_mapRequestKeyValue["HttpVersion"] = token;

    return true;
}

// 返回响应头信息
bool CTyHttpSvr::SendResponseHead(const char * pFileName)
{
    char szHeader[1024*2];
    struct stat timeFileCreated;

    if( stat(pFileName, &timeFileCreated ) != -1 ) 
    {
        //连接正确时返回的信息
        char szHdrFmt[]=
            "HTTP/1.1 200 OK\r\n"
            "Server: TaiyanSocket Server\r\n"
            "Date: %s\r\n"
            "Content-Type: application/octet-stream\r\n"
            "Content-Length: %d\r\n"
            "Last-Modified: %s\r\n\r\n";

        time_t timeNow;
        time(&timeNow);

        char szTimeNow[1024];
        strftime(szTimeNow, 1023, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&timeNow));

        struct tm* pTm = localtime(&timeFileCreated.st_mtime);
        char szTimeLastModifyed[1024];
        strftime(szTimeLastModifyed, 1023, "%a, %d %b %Y %H:%M:%S GMT", pTm);

        sprintf(szHeader, szHdrFmt, szTimeNow, timeFileCreated.st_size, szTimeLastModifyed);
        Send(szHeader, strlen(szHeader));
        return true;
    }

    sprintf(szHeader, "HTTP/1.0 404 Object Not Found\r\n"
        "Server: TaiyanSocket Server\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 65\r\n\r\n"
        "<html><h1><body>HTTP/1.0 404 Object Not Found</h1></body></html>");

    Send(szHeader, strlen(szHeader));
    return false;
}

// 发送请求/响应的文件
bool CTyHttpSvr::SendResponseFile()
{
    string strFilePathName = m_strMainPath;
    strFilePathName += "/";
    strFilePathName += m_mapRequestKeyValue["RequestUrl"];

    if (SendResponseHead(strFilePathName.c_str()))
        return TransmitFile(strFilePathName.c_str());

    return false;
}

CTyHttpClient::CTyHttpClient() : m_nPort(80)
{

}

CTyHttpClient::~CTyHttpClient()
{

}

// 下载URL文件到本地
bool CTyHttpClient::DownloadUrl( const char * url, const char *pszPath, const char *pszFileName/*=NULL*/ )
{
    NormalizationUrl();
    if(!ParseUrl(url))
        return false;

    if (!Create_Connect(m_strHost.c_str(), m_nPort))
        return false;

    if (GetReservedBufSize(CTySocket::typeRead) < 1024*100)
        SetReservedBufSize(CTySocket::typeRead, 1024*100);

    SendRequest();
    if(RecvResponseHead())
    {
        if(!pszFileName)
            pszFileName = m_strHttpFileName.c_str();

        string strPathName;
        if (pszPath)
		{
            strPathName = pszPath;
			strPathName += DIR_SLASH;
		}
		
        strPathName += pszFileName;
        return SaveHttpFile(strPathName.c_str());
     }

    return false;
}

// 归一化URL
void CTyHttpClient::NormalizationUrl()      // 将UTF8等格式转换成正常格式
{

}

// 解析URL
bool CTyHttpClient::ParseUrl(const char * url)
{
    // http://www.taiyan.net:80/....../list.asp
    string strUrl = url;
    url = strUrl.c_str();


    // 查找HTTP头
    char *pProtocolSeperator = strstr((char *)url, "://");
    if (pProtocolSeperator)
    {
        *pProtocolSeperator = '\0';
        *pProtocolSeperator = ':';

        char *pHostIpPort = pProtocolSeperator+3;
        char *pHostSeperator = strstr(pHostIpPort, "/");
        if (pHostSeperator)
        {
            *pHostSeperator = '\0';
            // www.taiyan.net:80

            char *pColon = (char *)memchr(pHostIpPort, ':', strlen(pHostIpPort));
            if (pColon)
            {
                *pColon = '\0';
                m_nPort = atoi(pColon+1);
            }
            else 
            {
                m_nPort = 80;
            }

            m_strHost = pHostIpPort;

            *pHostSeperator = '/';
            m_strRequestUrl = pHostSeperator;     // /....../list.asp

            m_strHttpFileName = m_strRequestUrl.substr(m_strRequestUrl.rfind('/')+1);

            return true;
        }
    }

    return false;
}

// 发送请求
void CTyHttpClient::SendRequest()
{
    char szRequestHead[1024*10];
    sprintf(szRequestHead, "GET %s HTTP/1.0\r\n"
        "User-Agent: TyHttpClient/1.0.1\r\n"
        "Accept: */*\r\n"
        "Host: %s\r\n"
        "Connection: Keep-Alive\r\n\r\n", m_strRequestUrl.c_str(), m_strHost.c_str());

    Send(szRequestHead, strlen(szRequestHead));
}

// 接收响应头
bool CTyHttpClient::RecvResponseHead()
{
    char *pszRequest = new char[1024*10];
    char *pCurrent = pszRequest;
    while(!IsClosed())
    {
        // 每次只接收1字节
        if (1 == Select_Receive(pCurrent, 1))
        {
            ++pCurrent;

            // 接完4个字节后,可判断是否已全接完
            if((pCurrent>pszRequest+4) && (memcmp(pCurrent-4, "\r\n\r\n", 4)==0))
            {
                *pCurrent = '\0';
                break;
            }
        }
    }

    bool bRet = ParseResponse(pszRequest);
    delete [] pszRequest;

    return bRet;
}


// 解析返回的响应信息
bool CTyHttpClient::ParseResponse(const char * pszRequest)
{
    string strResponseSummary;

    char sep_CR[] = "\r\n";
    char *token = strtok((char *)pszRequest, sep_CR);

    // 第一行，请求方式/请求的文件
    if (token)
        strResponseSummary = token;

    token = strtok(NULL, sep_CR);
    while(token)
    {
        char *pKey = token;
        char *pColon = (char *)memchr(token, ':', strlen(token));
        if (pColon)
        {
            *pColon = '\0';
            char *pValue = pColon+1;

            m_mapResponseKeyValue[pKey] = pValue;
        }

        token = strtok(NULL, sep_CR);
    }

    char sep_BLANK[] = " ";
    token = strtok((char *)strResponseSummary.c_str(), sep_BLANK);
    if (token)
        m_mapResponseKeyValue["HttpVersion"] = token;

    token = strtok(NULL, sep_BLANK);
    if (token)
        m_mapResponseKeyValue["ResponseValue"] = token;

    token = strtok(NULL, sep_BLANK);
    if (token)
        m_mapResponseKeyValue["ResponseInfo"] = token;

    return m_mapResponseKeyValue["ResponseValue"] == "200";
}

// 保存HTTP文件到本地
bool CTyHttpClient::SaveHttpFile(const char *pszFileName)
{
    ofstream fileHttp;
    fileHttp.open(pszFileName, ios::binary|ios::trunc);
    if (!fileHttp.is_open())
        return false;

    char buf[1024];
    int nFileLen = atoi(m_mapResponseKeyValue["Content-Length"].c_str());
    int nRecvLen = 0;
    while(!IsClosed() && nRecvLen<nFileLen)
    {
        int len = Select_Receive(buf, 1024);
        if (len>0)
        {
            fileHttp.write(buf, len);
            nRecvLen += len;
        }
    }

    Close();

    return (nFileLen==nRecvLen);
}
