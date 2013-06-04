///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2008.
// All right reserved!
// 文 件 名：TyLog.cpp
// 版    本：1.0
// 功能描述：日志记录类
// 创建日期：2009-3-10 15:00:36
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////

#include "TyLog.h"
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "TyApp.h"

char g_szFlag[][16] = 
{
	//"成功",	"失败",	"开始",	"结束",	"正常",	"取消",	"错误",	"警告"
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

// 打开日志文件
bool CTyLog::OpenLogFile(const char *pszLogFileName)
{
	return m_fileLog.Open(pszLogFileName, "a");
}

// 添加一条日志
void CTyLog::AddLog(LogFlag nLogFlag, char *pszFmt, ...)
{
    char szBuf[1024] = {0};

    // 记录时间
    time_t timeNow;
    time(&timeNow);
    struct tm *ptmNow = localtime(&timeNow);
    strftime(szBuf, 1023, "%Y-%m-%d %#X  ", ptmNow);
    if(m_bIsWriteToScreen)
        m_fileStdErr.Write(szBuf, strlen(szBuf));
    m_fileLog.Write(szBuf, strlen(szBuf));

    // 记录标志
    sprintf(szBuf, "%-10s", g_szFlag[nLogFlag]);
    if(m_bIsWriteToScreen)
        m_fileStdErr.Write(szBuf, strlen(szBuf));
    m_fileLog.Write(szBuf, strlen(szBuf));

    // 记录其它内容
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

    // 结束回车
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
