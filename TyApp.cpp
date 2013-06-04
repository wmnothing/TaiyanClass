///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2008.
// All right reserved!
// 文 件 名：TyApp.cpp
// 版    本：1.0
// 功能描述：一个模仿 CWinApp 部分功能的类
// 创建日期：2009-3-5 9:58:28
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////
#include "TyApp.h"
#include <time.h>

CTyApp g_theApp;


CTyApp::CTyApp(void)
{
    m_bufStream = NULL;
}

CTyApp::~CTyApp(void)
{
}

// 设置进程的路径文件名
void CTyApp::SetModuleFileName(const char *pszModuleFileName)
{
#ifdef WIN32
    m_strModuleFileName = pszModuleFileName;
#else
    m_strModuleFileName = GetLinuxModuleFileName(pszModuleFileName);
#endif

	int nPos  = m_strModuleFileName.find_last_of(DIR_SLASH);
	if (nPos > 0)
		m_strAppPath = m_strModuleFileName.substr(0, nPos);
	else
		m_strAppPath = ".";

    m_strFileName =  m_strModuleFileName.substr(nPos+1);
}

// 得到进程的路径
string CTyApp::GetAppPath()
{
	return m_strAppPath;
}

//  linux 下得到进程路径全名
#ifndef WIN32
string CTyApp::GetLinuxModuleFileName(const char* szModuleName)
{
    string strModuleFileName;
    char sLine[1024] = { 0 };
    void* pSymbol = (void*)"";

    FILE *fp = fopen ("/proc/self/maps", "r");
    if ( fp == NULL )
        return "";

    while (!feof (fp))
    {
        unsigned long start, end;

        if ( !fgets (sLine, sizeof (sLine), fp))
            continue;

        if ( !strstr (sLine, " r-xp ") || !strchr (sLine, '/'))
            continue;

        sscanf (sLine, "%lx-%lx ", &start, &end);
        if (pSymbol >= (void *)start && pSymbol < (void *)end)
        {
            // Extract the filename; it is always an absolute path 
            char *pPath = strchr (sLine, '/');

            // Get rid of the newline 
            char *tmp = strrchr (pPath, '\n');
            if (tmp)
                *tmp = 0;

            /* Get rid of "(deleted)" */
            //size_t len = strlen (pPath);
            //if (len > 10 && strcmp (pPath + len - 10, " (deleted)") == 0)
            //{
            //    tmp = pPath + len - 10;
            //    *tmp = 0;
            //}

            strModuleFileName = pPath;
        }
    }

    fclose (fp);
    return strModuleFileName;
}
#endif

void CTyApp::RedirectCout(const char *pszFileName)
{
    if(pszFileName == NULL)
    {
        m_fileCout.close();
        cout.rdbuf(m_bufStream);   // recover  
    }
    else
    {
        m_fileCout.open(pszFileName);  
        m_bufStream = cout.rdbuf();     
        cout.rdbuf(m_fileCout.rdbuf());
    }
}


char * CTyApp::GetCurrentTimeStr()
{
    static char bufTime[32];
    memset(bufTime, 0, 32);

    time_t timeNow = time(NULL);
    strftime(bufTime, 32, "%Y%m%d-%H:%M:%S", localtime( &timeNow ));
    return bufTime;
}
