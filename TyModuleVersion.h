#pragma once
#include <wtypes.h>

class CTyModuleVersion
{
public:
    CTyModuleVersion(void);
    CTyModuleVersion(TCHAR * pszModulePathName);
    ~CTyModuleVersion(void);

    BOOL LoadModuleVersion(TCHAR * pszModulePathName);
    TCHAR * GetVersionSectionValue(TCHAR * pszSectionName);

private:
    char *m_pVersionData;
};
