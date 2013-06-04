#pragma once

#include <stdio.h>
#include <iostream>
using namespace std;

class CTyFile
{
public:
	CTyFile(void);
	~CTyFile(void);

	bool Open(const char *filename, const char *mode);
	void Close();

	size_t Read(void *buffer, size_t count);
	size_t Write(const void *buffer, size_t count);
	size_t WriteFmt(char *pszFmt, ...);
	size_t WriteVarList(char *pszFmt, va_list argptr);

	bool Seek(long offset, int origin);
    bool IsEOF();
    bool End();
	long GetFileLength();

	char * GetLine(char *str, int n);

	void Attach(FILE *file);
	FILE *Detach();

	static bool CreateFullDirectory(const char *pszPath);
	static string GetFilePath(const char *pszFile);
    static string GetFileName(const char *pszFile);
    static bool Rename(const char *pszOldFileFile, const char *pszNewFileName);
    static bool RemoveFullDirectory(const char *pszPath);
    static bool RemoveFile(const char *pszFileName);
    static long GetFileLength(const char *pszFileName);

    CTyFile& operator<< (char *pszData); 
    CTyFile& operator<< (unsigned int nData); 
    CTyFile& operator<< (unsigned long nData); 
    CTyFile& operator<< (char cData); 
    CTyFile& operator<< (int nData); 
    CTyFile& operator<< (long nData); 
    CTyFile& operator<< (long long nData); 
    CTyFile& operator<< (float fData); 
    CTyFile& operator<< (double fData); 

    void operator>> (unsigned int &nData); 
    void operator>> (unsigned long &nData); 
    void operator>> (char &cData); 
    void operator>> (int &nData); 
    void operator>> (long &nData); 
    void operator>> (long long &nData); 
    void operator>> (float &fData); 
    void operator>> (double &fData); 

public:
#ifdef WIN32
    static const char DIR_SLASH = '\\';
#else
	static const char DIR_SLASH = '/';
#endif

private:
	FILE *m_file;
	int m_nLastErr;
};


#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

class CTyFileFinder // 此类开发未完成，不能使用
{
public:
    CTyFileFinder();
    virtual ~CTyFileFinder();

    bool Find(const char* filter = "*.*");
    bool FindNext();
    void Close();

    char* GetFilePath();
    char* GetFileName();
    char* GetFilePathName();

    bool IsDots();
    bool IsDirectory();

    unsigned int GetLength();

private:
    bool m_bIsOpened;
    char m_szPath[1024];
    char m_szFilter[1024];

    char m_szData[1024];

#ifdef _WIN32
	struct _finddata_t m_structFindData;
	intptr_t m_hFile;
#else
	DIR *m_dir;
	struct dirent *m_structFindData;
	struct stat m_stat;
#endif
};

