#pragma once
#include "TyFile.h"

#define USING_LOG

class CTyLog
{
public:
	enum LogFlag {SucceededFlag, FailedFlag, BeginFlag, EndFlag, NormalFlag, CancelFlag, ErrorFlag, WarningFlag, FlagNumber};

public:
	CTyLog(void);
	~CTyLog(void);

	bool OpenLogFile(const char *pszLogFileName);
	void AddLog(LogFlag nLogFlag, char *pszFmt, ...);

    CTyLog& operator<< (char *pszData); 
    CTyLog& operator<< (unsigned int nData); 
    CTyLog& operator<< (unsigned long nData); 
    CTyLog& operator<< (char cData); 
    CTyLog& operator<< (int nData); 
    CTyLog& operator<< (long nData); 
    CTyLog& operator<< (long long nData); 
    CTyLog& operator<< (float fData); 
    CTyLog& operator<< (double fData); 

    static CTyLog& GetInstance();
    static CTyLog& GetInstanceWithTimeStr();
public:
    bool m_bIsWriteToScreen;

private:
	CTyFile m_fileStdErr;
	CTyFile m_fileLog;

    static CTyLog * m_log;
};
