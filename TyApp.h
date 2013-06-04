#pragma once
#include <iostream>
#include <fstream> 
#include "win_linux.h"
using namespace std;

class CTyApp
{
public:
	CTyApp(void);
	~CTyApp(void);

	void SetModuleFileName(const char *pszModuleFileName);
	string GetAppPath();

    void RedirectCout(const char *pszFileName=NULL);
    static char * GetCurrentTimeStr();

private:
#ifndef WIN32
    string GetLinuxModuleFileName(const char* szModuleName);
#endif

public:
	string m_strModuleFileName;  // 路径+文件名
	string m_strAppPath;         // 路径
    string m_strFileName;        // 文件名

private:
    streambuf * m_bufStream;
    ofstream m_fileCout;
};

extern CTyApp g_theApp;

