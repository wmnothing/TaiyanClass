///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007.
// All right reserved!
// 文 件 名：TaiyanHttpSvr.cpp
// 版    本：1.0
// 功能描述：实现一个http服务器端
// 创建日期：2008-1-3 22:23:45
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////
#include "TaiyanHttpSvr.h"
#include <time.h>

CTaiyanHttpSvr::CTaiyanHttpSvr(void)
{
}

CTaiyanHttpSvr::~CTaiyanHttpSvr(void)
{
}

// 启动网站服务
bool CTaiyanHttpSvr::StartServer(const char * szSvrIpAddr, unsigned int nSocketPort)
{
	if(!m_socketServer.Create())
		return false;


	if(!m_socketServer.Bind(szSvrIpAddr, nSocketPort))
		return false;


	if(!m_socketServer.Listen())
		return false;

	return true;
}

// 停止网站服务
void CTaiyanHttpSvr::StopServer()
{
	if(m_socketServer.IsClosed())
		return;

	m_socketServer.Close();
	m_listVirtualPathName.clear();
	m_listVirtualPath.clear();
	m_listDefaultDocument.clear();
}

// 接收请示
bool CTaiyanHttpSvr::RecvRequest(CTySocket &socketAccept, char *pszRequest, int nRequestLen)
{
	if (m_socketServer.Accept(socketAccept))
	{
		int nLen = socketAccept.Receive(pszRequest, nRequestLen);
		pszRequest[nLen] = 0;
		return true;
	}
	 
	return false;
}


void SendData(CTySocket &socketClient, const char *pszData, int nLen)
{
	socketClient.Send(pszData, nLen);
}

bool CTaiyanHttpSvr::IsClosed()
{
	return m_socketServer.IsClosed();
}

// 处理用户请求
void CTaiyanHttpSvr::Response(CTySocket &socketClient, const char *pszHttpHeader)
{
	char szRequestFilePathName[PATH_MAX] = {0};
	switch(ParseRequest(pszHttpHeader, szRequestFilePathName))
	{
	case CTaiyanHttpSvr::modeGET:
		//AddLog(g_szMsg[msgClientGet], szRequestFilePathName);

		SendRequestFile(socketClient, szRequestFilePathName);
		break;
	case CTaiyanHttpSvr::modePOST:
		//AddLog(g_szMsg[msgClientPost], szRequestFilePathName);

		SendRequestFile(socketClient, szRequestFilePathName);
		break;
	default:
		//AddLog(g_szMsg[msgUnknownRequest]);
		break;
	}
}

// 解析HTTP请求头
CTaiyanHttpSvr::Request_Type CTaiyanHttpSvr::ParseRequest(const char *pszRequest, char *pszRequestFilePathName)
{
	if(memcmp(pszRequest, "GET", 3) == 0)										// 浏览器GET方式
	{
		char *pszRequestFileName = (char *)pszRequest + 4;
		char *pszEnd = (char *)memchr(pszRequestFileName, ' ', strlen(pszRequestFileName));

		strncpy(pszRequestFilePathName, pszRequestFileName, (pszEnd - pszRequestFileName));
		return modeGET;
	}

	if(memcmp(pszRequest, "POST", 4) == 0)										// 浏览器POST方式
	{
		char *pszRequestFileName = (char *)pszRequest + 5;
		char *pszEnd = (char *)memchr(pszRequestFileName, ' ', strlen(pszRequestFileName));

		strncpy(pszRequestFilePathName, pszRequestFileName, (pszEnd - pszRequestFileName));

		char *pRequest = strstr((char*)pszRequest, "\r\n\r\n");
		if (pRequest)
		{
			pRequest += 4;

			strcat(pszRequestFilePathName, "?");
			strcat(pszRequestFilePathName, pRequest);
		}
		return modePOST;
	}

	return modeNONE;
}

// 发 HTTP 应答头
void CTaiyanHttpSvr::SendHeader(CTySocket &socketClient, int nContentLen, struct tm *pLastModifiedTime)
{
	//连接正确时返回的信息
	char szHdrFmt[]=
		"HTTP/1.0 200 OK\r\n"
		"Server: TaiyanSocket Server\r\n"
		"Date: %s\r\n"
		"Content-Type: text/html\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: %d\r\n"
		"Last-Modified: %s\r\n\r\n";

	char szTimeNow[1024];
	time_t timeNow;
	time(&timeNow);
	strftime(szTimeNow, 1023, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&timeNow));

	char szTimeLastModifyed[1024];
	strftime(szTimeLastModifyed, 1023, "%a, %d %b %Y %H:%M:%S GMT", pLastModifiedTime);

	char szHeader[1024*2];
	sprintf(szHeader, szHdrFmt, szTimeNow, nContentLen, szTimeLastModifyed);

	socketClient.Send(szHeader, strlen(szHeader));
}

// 发送文件未找到 HTTP应答
void CTaiyanHttpSvr::SendNotFoundObject(CTySocket &socketClient)
{
	//连接错误返回到浏览的信息
	char hdrErr[]=
		"HTTP/1.0 404 对象没有找到\r\n"
		"Server: MySocket Server\r\n"
		"Content-Type: text/html\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 62\r\n\r\n"
		"<html><h1><body>HTTP/1.0 404 对象没有找到</h1></body></html>\r\n";

	socketClient.Send(hdrErr, strlen(hdrErr));
}

// 从主目录中获取文件
string CTaiyanHttpSvr::GetHttpFileFromLocalPath(string strRequestFileName, string strLocalPath)
{
	return "";
}

// 从虚拟路径中获取文件
string CTaiyanHttpSvr::GetHttpFileFromVirtualPath(string strVirtualPathHttpFile)
{
	string strVirtualPathName;
	string strHttpFile;

	int nVirtualPahtPos = strVirtualPathHttpFile.find('/', 1);
	if(nVirtualPahtPos > 0)														// 虚拟目录下的特定文件
	{
		strVirtualPathName = strVirtualPathHttpFile.substr(1, nVirtualPahtPos-1);// 去掉 / 
		strHttpFile = strVirtualPathHttpFile.substr(nVirtualPahtPos);
	}
	else																		// 虚拟目录下的默认文件
	{
		strVirtualPathName = strVirtualPathHttpFile.substr(1);					// 去掉 / 
		strHttpFile = "/";														// 当作是虚拟目录的默认文件
	}


	for (list<string>::iterator it=m_listVirtualPath.begin(); it!=m_listVirtualPath.end(); ++it)
	{
		if (strVirtualPathName.compare(*it) == 0)
			return GetHttpFileFromLocalPath(strHttpFile, *it);
	}

	// 如果没有这个虚拟路径
	return "";
}

// 从HTTP路径转换成本地路径
string CTaiyanHttpSvr::GetLocalPath(const char *pszHttpFile)
{
	return "";
}

// 发送一个本地文件
void CTaiyanHttpSvr::SendRequestFile(CTySocket &socketClient, const char *pszRequestFilePathName)
{
}

// 设置网站主目录
void CTaiyanHttpSvr::SetMainPath(const char *pszMainPath)
{
	m_strMainPath = pszMainPath;
}

// 添加虚拟路径
void CTaiyanHttpSvr::AddVirtailPath(const char * pszVirtailPathName, const char * pszVirtualPath)
{
	m_listVirtualPathName.push_back(pszVirtailPathName);
	m_listVirtualPath.push_back(pszVirtualPath);
}

// 添加默认网页文件名
void CTaiyanHttpSvr::AddDefaultDocument(const char * pszDefaultDocument)
{
	m_listDefaultDocument.push_back(pszDefaultDocument);
}

