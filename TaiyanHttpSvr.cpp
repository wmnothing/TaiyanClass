///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007.
// All right reserved!
// �� �� ����TaiyanHttpSvr.cpp
// ��    ����1.0
// ����������ʵ��һ��http��������
// �������ڣ�2008-1-3 22:23:45
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////
#include "TaiyanHttpSvr.h"
#include <time.h>

CTaiyanHttpSvr::CTaiyanHttpSvr(void)
{
}

CTaiyanHttpSvr::~CTaiyanHttpSvr(void)
{
}

// ������վ����
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

// ֹͣ��վ����
void CTaiyanHttpSvr::StopServer()
{
	if(m_socketServer.IsClosed())
		return;

	m_socketServer.Close();
	m_listVirtualPathName.clear();
	m_listVirtualPath.clear();
	m_listDefaultDocument.clear();
}

// ������ʾ
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

// �����û�����
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

// ����HTTP����ͷ
CTaiyanHttpSvr::Request_Type CTaiyanHttpSvr::ParseRequest(const char *pszRequest, char *pszRequestFilePathName)
{
	if(memcmp(pszRequest, "GET", 3) == 0)										// �����GET��ʽ
	{
		char *pszRequestFileName = (char *)pszRequest + 4;
		char *pszEnd = (char *)memchr(pszRequestFileName, ' ', strlen(pszRequestFileName));

		strncpy(pszRequestFilePathName, pszRequestFileName, (pszEnd - pszRequestFileName));
		return modeGET;
	}

	if(memcmp(pszRequest, "POST", 4) == 0)										// �����POST��ʽ
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

// �� HTTP Ӧ��ͷ
void CTaiyanHttpSvr::SendHeader(CTySocket &socketClient, int nContentLen, struct tm *pLastModifiedTime)
{
	//������ȷʱ���ص���Ϣ
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

// �����ļ�δ�ҵ� HTTPӦ��
void CTaiyanHttpSvr::SendNotFoundObject(CTySocket &socketClient)
{
	//���Ӵ��󷵻ص��������Ϣ
	char hdrErr[]=
		"HTTP/1.0 404 ����û���ҵ�\r\n"
		"Server: MySocket Server\r\n"
		"Content-Type: text/html\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 62\r\n\r\n"
		"<html><h1><body>HTTP/1.0 404 ����û���ҵ�</h1></body></html>\r\n";

	socketClient.Send(hdrErr, strlen(hdrErr));
}

// ����Ŀ¼�л�ȡ�ļ�
string CTaiyanHttpSvr::GetHttpFileFromLocalPath(string strRequestFileName, string strLocalPath)
{
	return "";
}

// ������·���л�ȡ�ļ�
string CTaiyanHttpSvr::GetHttpFileFromVirtualPath(string strVirtualPathHttpFile)
{
	string strVirtualPathName;
	string strHttpFile;

	int nVirtualPahtPos = strVirtualPathHttpFile.find('/', 1);
	if(nVirtualPahtPos > 0)														// ����Ŀ¼�µ��ض��ļ�
	{
		strVirtualPathName = strVirtualPathHttpFile.substr(1, nVirtualPahtPos-1);// ȥ�� / 
		strHttpFile = strVirtualPathHttpFile.substr(nVirtualPahtPos);
	}
	else																		// ����Ŀ¼�µ�Ĭ���ļ�
	{
		strVirtualPathName = strVirtualPathHttpFile.substr(1);					// ȥ�� / 
		strHttpFile = "/";														// ����������Ŀ¼��Ĭ���ļ�
	}


	for (list<string>::iterator it=m_listVirtualPath.begin(); it!=m_listVirtualPath.end(); ++it)
	{
		if (strVirtualPathName.compare(*it) == 0)
			return GetHttpFileFromLocalPath(strHttpFile, *it);
	}

	// ���û���������·��
	return "";
}

// ��HTTP·��ת���ɱ���·��
string CTaiyanHttpSvr::GetLocalPath(const char *pszHttpFile)
{
	return "";
}

// ����һ�������ļ�
void CTaiyanHttpSvr::SendRequestFile(CTySocket &socketClient, const char *pszRequestFilePathName)
{
}

// ������վ��Ŀ¼
void CTaiyanHttpSvr::SetMainPath(const char *pszMainPath)
{
	m_strMainPath = pszMainPath;
}

// �������·��
void CTaiyanHttpSvr::AddVirtailPath(const char * pszVirtailPathName, const char * pszVirtualPath)
{
	m_listVirtualPathName.push_back(pszVirtailPathName);
	m_listVirtualPath.push_back(pszVirtualPath);
}

// ���Ĭ����ҳ�ļ���
void CTaiyanHttpSvr::AddDefaultDocument(const char * pszDefaultDocument)
{
	m_listDefaultDocument.push_back(pszDefaultDocument);
}

