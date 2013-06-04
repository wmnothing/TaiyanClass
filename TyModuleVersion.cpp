//#include "StdAfx.h"
#include "TyModuleVersion.h"
#include <atlstr.h>

#pragma comment (lib, "Version.lib")

struct LANGANDCODEPAGE 
{
    WORD wLanguage;
    WORD wCodePage;
};


CTyModuleVersion::CTyModuleVersion(void)
{
    m_pVersionData = NULL;
}

CTyModuleVersion::CTyModuleVersion(TCHAR * pszModulePathName)
{
    m_pVersionData = NULL;
    LoadModuleVersion(pszModulePathName);
}

CTyModuleVersion::~CTyModuleVersion(void)
{
    if(m_pVersionData)
    {
        delete []m_pVersionData;
        m_pVersionData = NULL;
    }
}


BOOL CTyModuleVersion::LoadModuleVersion(TCHAR *  pszModulePathName)
{
    if(m_pVersionData)
    {
        delete []m_pVersionData;
        m_pVersionData = NULL;
    }

    DWORD dwSize = GetFileVersionInfoSize(pszModulePathName, NULL);
    m_pVersionData = new char [dwSize];

    return GetFileVersionInfo(pszModulePathName, NULL, dwSize, m_pVersionData);
}

TCHAR * CTyModuleVersion::GetVersionSectionValue(TCHAR * pszSectionName)
{
    if(m_pVersionData == NULL)
        return _T("");

    LANGANDCODEPAGE *lpTranslate;
    UINT cbTranslate=0;

    VerQueryValue(m_pVersionData, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate);

    CString strSubBlock;
    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\%s"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, pszSectionName);

    TCHAR *pSectionValue = NULL;
    UINT dwBytes        = 0;;
    VerQueryValue(m_pVersionData, (LPTSTR)(LPCTSTR)strSubBlock, (void **)&pSectionValue, &dwBytes);
    return (pSectionValue);

    //int nStructSize = sizeof(struct LANGANDCODEPAGE);

    //for( unsigned int i=0; i < (cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
    //{
    //    TCHAR *lpProductName, *lpBufferVersion, *lpLegalCopyright, *lpComments;

    //    CString strSubBlock;

    //    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\ProductName"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
    //    VerQueryValue(m_pVersionData, (LPTSTR)(LPCTSTR)strSubBlock, (void **)&lpProductName, &dwBytes);

    //    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
    //    VerQueryValue(m_pVersionData, (LPTSTR)(LPCTSTR)strSubBlock, (void **)&lpBufferVersion, &dwBytes);

    //    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\LegalCopyright"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
    //    VerQueryValue(m_pVersionData, (LPTSTR)(LPCTSTR)strSubBlock, (void **)&lpLegalCopyright, &dwBytes);

    //    strSubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\Comments"), lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
    //    VerQueryValue(m_pVersionData, (LPTSTR)(LPCTSTR)strSubBlock, (void **)&lpComments, &dwBytes);
    //}

}
