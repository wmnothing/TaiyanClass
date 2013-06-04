#pragma once
#include "TySocket.h"
#include <map>
#include <string>
#include <list>

using namespace std;

class CTyHttpSvr : public CTySocket
{
public:
    CTyHttpSvr(void);
    ~CTyHttpSvr(void);

    void SetMainPath(const char *pszMainPath);
    void AddVirtailPath(const char * pszVirtailPathName, const char * pszVirtualPath);
    void AddDefaultDocument(const char * pszDefaultDocument);

    bool RecvRequest();
    bool SendResponseFile();

protected:
    bool ParseRequest(const char * pszRequest);
    bool SendResponseHead(const char * pFileName);

private:
    string m_strMainPath;

    list<string> m_listDefaultDocument;
    map<string, string> m_mapVirtualPathName;

    map<string, string> m_mapRequestKeyValue;
};


class CTyHttpClient : public CTySocket
{
public:
    CTyHttpClient();
    ~CTyHttpClient();

    bool DownloadUrl(const char * url, const char *pszPath=NULL, const char *pszFileName=NULL);

protected:
    void NormalizationUrl();
    bool ParseUrl(const char * url);
    void SendRequest();
    bool RecvResponseHead();
    bool SaveHttpFile(const char *pszFileName);
    bool ParseResponse(const char *pszRequest);

private:
    string m_strHost;
    int m_nPort;
    string m_strRequestUrl;
    string m_strHttpFileName;

    map<string, string> m_mapResponseKeyValue;

};