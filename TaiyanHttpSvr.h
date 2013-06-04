#pragma once
#include "TySocket.h"
#include <iostream>
#include <list>

using namespace std;

class CTaiyanHttpSvr
{
public:
	enum Request_Type{modeNONE, modeGET, modePOST};
public:
	CTaiyanHttpSvr(void);
	~CTaiyanHttpSvr(void);

	void SetMainPath(const char *pszMainPath);
	void AddVirtailPath(const char * pszVirtailPathName, const char * pszVirtualPath);
	void AddDefaultDocument(const char * pszDefaultDocument);

	bool StartServer(const char * szSvrIpAddr = NULL, unsigned int nSocketPort = 80);
	void StopServer();

	void Response(CTySocket &socketClient, const char *pszHttpHeader);

	void SendData(CTySocket &socketClient, const char *pszData, int nLen);
	void SendRequestFile(CTySocket &socketClient, const char *pszHttpFile);
	void SendHeader(CTySocket &socketClient, int nContentLen, struct tm *pLastModifiedTime);
	void SendNotFoundObject(CTySocket &socketClient);

	Request_Type ParseRequest(const char *pszHeader, char *pszRequestFilePathName);
	string GetLocalPath(const char *pszHttpFile);

	string GetHttpFileFromLocalPath(string strRequestFileName, string strLocalPath);
	string GetHttpFileFromVirtualPath(string strVirtualPathHttpFile);

	bool IsClosed();
	bool RecvRequest(CTySocket &socketAccept, char *pszRequest, int nRequestLen);
private:
	CTySocket m_socketServer;
	string m_strMainPath;

	list<string> m_listDefaultDocument;
	list<string> m_listVirtualPathName;
	list<string> m_listVirtualPath;
};


