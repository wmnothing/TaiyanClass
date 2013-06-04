///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2008.
// All right reserved!
// 文 件 名：TyFile.cpp
// 版    本：1.0
// 功能描述：简单封装 C语言 的文件函数，在性能要求不高时使用方面
// 创建日期：2009-3-3 11:08:31
// 修改日期：
// 作    者： 吴旻(I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////
#include "TyFile.h"
#include <stdarg.h>
#include <assert.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>
#endif

CTyFile::CTyFile(void)
{
	m_file = NULL;
	m_nLastErr = 0;
}

CTyFile::~CTyFile(void)
{
	Close();
}

// 附加到一个文件指针
void CTyFile::Attach(FILE *file)
{
	m_file = file;
	m_nLastErr = 0;
}

// 取消附加
FILE * CTyFile::Detach()
{
	FILE * file = m_file;
	m_file = NULL;
	return file;
}

// 打开文件
bool CTyFile::Open(const char *filename, const char *mode)
{
	Close();

	m_file = fopen(filename, mode);
	return (m_file != NULL);
}

// 关闭文件
void CTyFile::Close()
{
	if (m_file)
    {
		fclose(m_file);
        m_file = NULL;
    }
}

// 读文件
size_t CTyFile::Read(void *buffer, size_t count)
{
	if (m_file == NULL)
		return 0;

	return fread(buffer, sizeof(char), count, m_file);

}

// 写文件
size_t CTyFile::Write(const void *buffer, size_t count)
{
	if (m_file == NULL)
		return 0;

	return fwrite(buffer, sizeof(char), count, m_file);
}

// 以不定参数方式写文件
size_t CTyFile::WriteFmt(char *pszFmt, ...) 
{
	if (m_file == NULL)
		return 0;

	va_list argptr;
	va_start(argptr, pszFmt);
	vfprintf(m_file, pszFmt, argptr);
	va_end(argptr);

	return fflush(m_file);
}


// 以不定参数方式写文件
size_t CTyFile::WriteVarList(char *pszFmt, va_list argptr)
{
	if (m_file == NULL)
		return 0;

	vfprintf(m_file, pszFmt, argptr);
	return fflush(m_file);
}

// 定位文件
bool CTyFile::Seek(long offset, int origin)
{
	if (m_file == NULL)
		return 0;

	return (0 == fseek(m_file, offset, origin));
}

// 检测读文件是否已经越界
bool CTyFile::IsEOF()
{
    return feof(m_file);
}

// 检测文件是否已经结束
bool CTyFile::End()
{
    return ftell(m_file) == GetFileLength();
}


// 读取一行文件
char * CTyFile::GetLine(char *str, int n)
{
	if (m_file == NULL)
		return 0;

	return fgets(str, n, m_file);
}

// 得到文件长度
long CTyFile::GetFileLength()
{
    if (m_file == NULL)
        return 0;

    int fd = fileno(m_file);
    struct stat fileStat; 
    fstat(fd, &fileStat);

    return fileStat.st_size;
    //return filelength(fd);
}

CTyFile& CTyFile::operator<< (char *pszData)
{
    Write(pszData, strlen(pszData));
    return *this;
}

CTyFile& CTyFile::operator<< (unsigned int nData)
{
    WriteFmt("%d", nData);
    return *this;
}

CTyFile& CTyFile::operator<< (unsigned long nData)
{
    WriteFmt("%ld", nData);
    return *this;
}

CTyFile& CTyFile::operator<< (char cData)
{
    WriteFmt("%c", cData);
    return *this;
}

CTyFile& CTyFile::operator<< (int nData)
{
    WriteFmt("%d", nData);
    return *this;
}

CTyFile& CTyFile::operator<< (long nData)
{
    WriteFmt("%ld", nData);
    return *this;
}

CTyFile& CTyFile::operator<< (long long nData)
{
    WriteFmt("%ld", nData);
    return *this;
}

CTyFile& CTyFile::operator<< (float fData)
{
    WriteFmt("%f", fData);
    return *this;
}
CTyFile& CTyFile::operator<< (double fData)
{
    WriteFmt("%f", fData);
    return *this;
}

void CTyFile::operator>> (unsigned int &nData)
{
    Read(&nData, sizeof(unsigned int));
}

void CTyFile::operator>> (unsigned long &nData)
{
    Read(&nData, sizeof(unsigned long));
}

void CTyFile::operator>> (char &cData)
{
    Read(&cData, sizeof(char));
}

void CTyFile::operator>> (int &nData)
{
    Read(&nData, sizeof(int));
}

void CTyFile::operator>> (long &nData)
{
    Read(&nData, sizeof(long));
}

void CTyFile::operator>> (long long &nData)
{
    Read(&nData, sizeof(long long));
}

void CTyFile::operator>> (float &fData)
{
    Read(&fData, sizeof(float));
}

void CTyFile::operator>> (double &fData)
{
    Read(&fData, sizeof(double));
}


// 创建一个完整的路径
bool CTyFile::CreateFullDirectory(const char *pszPath)
{
	assert(pszPath);
    if (NULL == pszPath)
        return false;

	int nLen = strlen(pszPath);
	char *pszPathTemp = new char [nLen + 1];
	strcpy(pszPathTemp, pszPath);

	if (pszPathTemp[nLen-1] == DIR_SLASH)
		pszPathTemp[nLen-1] = '\0';

	if (access(pszPathTemp, 0) == 0) 
    {
        delete [] pszPathTemp;
        return true;
    }

	char *pszDirSlash = strrchr(pszPathTemp, DIR_SLASH);
	if (pszDirSlash)
    {
		*pszDirSlash = '\0';
    }
    else 
    {
        delete [] pszPathTemp;
        return true;
    }

	bool bRetVal = CreateFullDirectory(pszPathTemp);
	if (bRetVal)
	{
		if (pszDirSlash)
			*pszDirSlash = DIR_SLASH;
#ifdef WIN32
        bRetVal = (mkdir(pszPathTemp) == 0);
#else
        bRetVal = (mkdir(pszPathTemp, S_ISUID|S_ISGID|S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) == 0);
#endif
	}

	delete [] pszPathTemp;
	return bRetVal;
}

// 删除一个完整的路径
bool CTyFile::RemoveFullDirectory(const char *pszPath)
{
    CTyFileFinder findFile;
    string strFilter = pszPath;
    if (pszPath[strlen(pszPath)-1] != DIR_SLASH)
        strFilter += DIR_SLASH;
    strFilter += "*.*";

    if (findFile.Find(strFilter.c_str()))
    {
        while (findFile.FindNext())
        {
            if(findFile.IsDots())
                continue;

            if(findFile.IsDirectory())
            {
                RemoveFullDirectory(findFile.GetFilePathName());
                continue;
            }

            RemoveFile(findFile.GetFilePathName());
        }
   }

    findFile.Close();
    return 0==rmdir(pszPath);
}


// 获得文件所在的路径
string CTyFile::GetFilePath(const char *pszFile)
{
	int nLen = strlen(pszFile);
    if (nLen == 0)
        return "";

    char *pszPathTemp = new char [nLen + 1];
	strcpy(pszPathTemp, pszFile);

	char *pszDirSlash = strrchr(pszPathTemp, DIR_SLASH);

	if (pszDirSlash)
    {
		*pszDirSlash = '\0';
    }
    else 
    {
        delete [] pszPathTemp;
        return "";
    }

	string strPath = pszPathTemp;

	if (pszDirSlash)
		*pszDirSlash = DIR_SLASH;

	delete [] pszPathTemp;
	return strPath;
}

// 获得文件名
string CTyFile::GetFileName(const char *pszFile)
{
    int nLen = strlen(pszFile);
    if (nLen == 0)
        return "";

    char *pszDirSlash = strrchr((char *)pszFile, DIR_SLASH);

    if (pszDirSlash)
        return (pszDirSlash+1);

    return "";
}

bool CTyFile::Rename(const char *pszOldFileFile, const char *pszNewFileName)
{
    return 0==rename(pszOldFileFile, pszNewFileName);
}

bool CTyFile::RemoveFile(const char *pszFileName)
{
    return 0==unlink(pszFileName);
}

long CTyFile::GetFileLength( const char *pszFileName )
{
    struct stat file_status;
    if(stat(pszFileName, &file_status) == 0)
        return file_status.st_size;

    return 0;
}

//////////////////////////////////////////////////////////////////////////

CTyFileFinder::CTyFileFinder()
{
    m_bIsOpened = false;
    memset(m_szPath,   0, sizeof(m_szPath));
    memset(m_szFilter, 0, sizeof(m_szFilter));
    memset(m_szData,   0, sizeof(m_szData));

#ifdef _WIN32
    m_hFile = NULL;
    memset(&m_structFindData, 0, sizeof(m_structFindData));
#else
    m_dir = NULL;
    m_structFindData = NULL;
    memset(&m_stat, 0, sizeof(m_stat));
#endif

}

CTyFileFinder::~CTyFileFinder()
{
    Close();
}

bool CTyFileFinder::Find(const char* filter)
{
    char* p = NULL;
    if((p = strrchr((char*)filter, CTyFile::DIR_SLASH)) == NULL)
        return false;

    strncpy(m_szPath, filter, p - filter);
    if(*(p + 1) == 0)
        sprintf(m_szFilter, "%s", "*.*");

    strcpy(m_szFilter, p+1);

#ifdef _WIN32
    if((m_hFile=_findfirst(filter, &m_structFindData)) == -1)
        return false;
#else
    if((m_dir=opendir(m_szPath)) == NULL)
        return false;
#endif

    m_bIsOpened = true;
    return true;
}

bool CTyFileFinder::FindNext()
{
    if(!m_bIsOpened)
        return false;

#ifdef _WIN32
    return (_findnext(m_hFile, &m_structFindData) == 0);
#else
    if((m_structFindData=readdir(m_dir)) != NULL)
    {
        if(lstat (GetFilePathName(), &m_stat) < 0)
            return false;
    }
    return (NULL != m_structFindData);
#endif
}

void CTyFileFinder::Close()
{
    if(m_bIsOpened)
    {
#ifdef _WIN32
        _findclose(m_hFile);
#else
        closedir(m_dir);
#endif
        m_bIsOpened = false;
    }
}

unsigned int CTyFileFinder::GetLength()
{
#ifdef _WIN32
    return m_structFindData.size;
#else
    return m_stat.st_size;
#endif
}

char* CTyFileFinder::GetFilePath()
{
    return m_szPath;
}

char* CTyFileFinder::GetFileName()
{
#ifdef _WIN32
    return m_structFindData.name;
#else
    return m_structFindData->d_name;
#endif
}

char* CTyFileFinder::GetFilePathName()
{
#ifdef _WIN32
    sprintf(m_szData, "%s%c%s", m_szPath, CTyFile::DIR_SLASH, m_structFindData.name);
#else
    sprintf(m_szData, "%s%c%s", m_szPath, CTyFile::DIR_SLASH, m_structFindData->d_name);
#endif

    return m_szData;
}

bool CTyFileFinder::IsDots()
{
#ifdef _WIN32
    return (strcmp(m_structFindData.name, ".") == 0) || (strcmp(m_structFindData.name, "..") == 0);
#else
    return (strcmp(m_structFindData->d_name, ".") == 0) || (strcmp(m_structFindData->d_name, "..") == 0);
#endif
}

bool CTyFileFinder::IsDirectory()
{
#ifdef _WIN32
    return ((m_structFindData.attrib & _A_SUBDIR) != 0);
#else
    return S_ISDIR(m_stat.st_mode);
#endif
}
