///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2008.
// All right reserved!
// �� �� ����TyApp.cpp
// ��    ����1.0
// ����������һ��ģ�� CWinApp ���ֹ��ܵ���
// �������ڣ�2009-3-5 9:58:28
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
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

// ���ý��̵�·���ļ���
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

// �õ����̵�·��
string CTyApp::GetAppPath()
{
	return m_strAppPath;
}

//  linux �µõ�����·��ȫ��
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
