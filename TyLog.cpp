///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2008.
// All right reserved!
// �� �� ����TyLog.cpp
// ��    ����1.0
// ������������־��¼��
// �������ڣ�2009-3-10 15:00:36
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////

#include "TyLog.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "TyApp.h"

char g_szFlag[][16] = 
{
	//"�ɹ�",	"ʧ��",	"��ʼ",	"����",	"����",	"ȡ��",	"����",	"����"
	"Succeeded", "Failed", "Begin", "End", "Normal", "Cancel", "Error", "Warning"
};

CTyLog * CTyLog::m_log = NULL;
CTyLog& CTyLog::GetInstance()
{
    if (m_log == NULL)
        m_log = new CTyLog;

    return *m_log;
}


CTyLog& CTyLog::GetInstanceWithTimeStr()
{
    CTyLog& tyLog = GetInstance();
    return tyLog << CTyApp::GetCurrentTimeStr() << " : ";
}


CTyLog::CTyLog(void)
{
	m_fileStdErr.Attach(stderr);
    m_bIsWriteToScreen = false;
}

CTyLog::~CTyLog(void)
{
	m_fileStdErr.Detach();
}

// ����־�ļ�
bool CTyLog::OpenLogFile(const char *pszLogFileName)
{
	return m_fileLog.Open(pszLogFileName, "a");
}

// ���һ����־
void CTyLog::AddLog(LogFlag nLogFlag, char *pszFmt, ...)
{
    char szBuf[1024] = {0};

    // ��¼ʱ��
    time_t timeNow;
    time(&timeNow);
    struct tm *ptmNow = localtime(&timeNow);
    strftime(szBuf, 1023, "%Y-%m-%d %#X  ", ptmNow);
    if(m_bIsWriteToScreen)
        m_fileStdErr.Write(szBuf, strlen(szBuf));
    m_fileLog.Write(szBuf, strlen(szBuf));

    // ��¼��־
    sprintf(szBuf, "%-10s", g_szFlag[nLogFlag]);
    if(m_bIsWriteToScreen)
        m_fileStdErr.Write(szBuf, strlen(szBuf));
    m_fileLog.Write(szBuf, strlen(szBuf));

    // ��¼��������
    va_list argptr;
    if(m_bIsWriteToScreen)
    {
        va_start(argptr, pszFmt);
        m_fileStdErr.WriteVarList(pszFmt, argptr);
        va_end(argptr);
    }

    va_start(argptr, pszFmt);
    m_fileLog.WriteVarList(pszFmt, argptr);
    va_end(argptr);

    // �����س�
    sprintf(szBuf, "\r\n");
    if(m_bIsWriteToScreen)
        m_fileStdErr.Write(szBuf, strlen(szBuf));
    m_fileLog.Write(szBuf, strlen(szBuf));
}

CTyLog& CTyLog::operator<< (char *pszData)
{
    m_fileLog << pszData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << pszData;

    return *this;
}

CTyLog& CTyLog::operator<< (unsigned int nData)
{
    m_fileLog << nData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << nData;

    return *this;
}

CTyLog& CTyLog::operator<< (unsigned long nData)
{
    m_fileLog << nData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << nData;

    return *this;
}

CTyLog& CTyLog::operator<< (char cData)
{
    m_fileLog << cData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << cData;

    return *this;
}

CTyLog& CTyLog::operator<< (int nData)
{
    m_fileLog << nData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << nData;

    return *this;
}

CTyLog& CTyLog::operator<< (long nData)
{
    m_fileLog << nData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << nData;

    return *this;
}

CTyLog& CTyLog::operator<< (long long nData)
{
    m_fileLog << nData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << nData;

    return *this;
}

CTyLog& CTyLog::operator<< (float fData)
{
    m_fileLog << fData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << fData;

    return *this;
}
CTyLog& CTyLog::operator<< (double fData)
{
    m_fileLog << fData;

    if(m_bIsWriteToScreen)
        m_fileStdErr << fData;

    return *this;
}
