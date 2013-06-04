///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2008.
// All right reserved!
// �� �� ����TyMappingFile.cpp
// ��    ����2.0
// �����������ڴ��ļ�ӳ����
// �������ڣ�2012-3-30 15:18:35
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include "TyMappingFile.h"
#include <sys/stat.h>

CTaiyanMappingFile::CTaiyanMappingFile(void)
{
    m_nFileLength     = 0;		// �ļ��ܳ���
    m_nMapBlockSize   = 0;      // ӳ���Ĵ�С
    m_nFileOffset     = 0;		// ӳ�俪ʼ���ļ�ƫ�Ƴ���
    m_nMappingLength  = 0;		// ӳ����ļ��鳤��
    m_lpMappingAddress = NULL;	// ӳ���Ļ���ַ
    m_dwOpenMode       = MAPMODE_READONLY;

#ifdef WIN32
	m_hMappingFile     = NULL;
	m_hFile            = NULL;
#else
	m_file             = NULL;
#endif
}

CTaiyanMappingFile::~CTaiyanMappingFile(void)
{
	CloseMappingFile();
	CloseFile();
}


#ifdef WIN32

bool CTaiyanMappingFile::LoadFileToMem(const char *pszFileName, unsigned long dwOpenMode/* = MAPMODE_READONLY*/, size_t nFileOffset/* =0*/, size_t nMapBlockSize/* = MAX_MAP_BLOCK_SIZE*/)
{
	CloseFile();
    m_dwOpenMode     = dwOpenMode;
    m_nMapBlockSize = nMapBlockSize;
    m_nFileOffset     = nFileOffset;

    DWORD dwDesiredAccess = (m_dwOpenMode==MAPMODE_READWRITE) ? GENERIC_READ|GENERIC_WRITE : GENERIC_READ;
	m_hFile = CreateFileA(pszFileName, dwDesiredAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

    m_nFileLength = GetFileSize(m_hFile, NULL);
	return CreateMappingFile(m_hFile, NULL, m_nFileLength, m_dwOpenMode);
}

void CTaiyanMappingFile::CloseFile()
{
	if (m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

    m_nFileLength     = 0;		// �ļ��ܳ���
    m_nMapBlockSize   = 0;      // ӳ���Ĵ�С
    m_nFileOffset     = 0;		// ӳ�俪ʼ���ļ�ƫ�Ƴ���
    m_nMappingLength  = 0;		// ӳ����ļ��鳤��
    m_lpMappingAddress = NULL;	// ӳ���Ļ���ַ
}

// ����
bool CTaiyanMappingFile::CreateMappingFile(HANDLE hFile, LPCTSTR lpName, DWORD dwMaximumSize, DWORD dwOpenMode/* = MAPMODE_READONLY*/, LPSECURITY_ATTRIBUTES lpAttributes/* = NULL*/, BOOL bIsMapView/* = TRUE*/)
{
	CloseMappingFile();
    m_dwOpenMode = dwOpenMode;

    DWORD flProtect = (m_dwOpenMode==MAPMODE_READWRITE) ? PAGE_READWRITE : PAGE_READONLY;
	m_hMappingFile = CreateFileMapping(hFile, lpAttributes, flProtect, 0, dwMaximumSize, lpName);
	if(m_hMappingFile == NULL)
		return false;

	if(bIsMapView)
        return MapNextBlock();

	return true;
}

// ��
bool CTaiyanMappingFile::OpenMappingFile(LPCTSTR lpName, DWORD dwOpenMode/* = MAPMODE_READONLY*/, BOOL bInheritHandle /*= FALSE*/, BOOL bIsMapView/* = TRUE*/)
{
	CloseMappingFile();
    m_dwOpenMode = dwOpenMode;

    DWORD dwDesiredAccess = (dwOpenMode==MAPMODE_READWRITE) ? FILE_MAP_READ|FILE_MAP_WRITE : FILE_MAP_READ;
	m_hMappingFile = OpenFileMapping(dwDesiredAccess, bInheritHandle, lpName);

	if(m_hMappingFile == NULL)
		return false;

	if(bIsMapView)
		return MapView();

	return true;
}

// ӳ��
bool CTaiyanMappingFile::MapView(DWORD dwFileOffset /*= 0*/,	SIZE_T dwNumberOfBytesToMap /*= 0*/)
{
    if(m_lpMappingAddress)
        UnmapViewOfFile(m_lpMappingAddress);

    m_nMappingLength = dwNumberOfBytesToMap;
    DWORD dwDesiredAccess = (m_dwOpenMode==MAPMODE_READWRITE) ? FILE_MAP_READ|FILE_MAP_WRITE:FILE_MAP_READ;
	m_lpMappingAddress = MapViewOfFile(m_hMappingFile, dwDesiredAccess, 0, dwFileOffset, m_nMappingLength);

	return m_lpMappingAddress != NULL;
}

bool CTaiyanMappingFile::MapNextBlock()
{
    m_nFileOffset += m_nMappingLength;
    if (m_nFileOffset >= m_nFileLength)
        return false;

    if (m_nFileOffset+m_nMapBlockSize > m_nFileLength)
        m_nMappingLength = m_nFileLength-m_nFileOffset;
    else
        m_nMappingLength = m_nMapBlockSize;

    return MapView(m_nFileOffset, m_nMappingLength);
}

// ����ӳ��
void CTaiyanMappingFile::UnMapView()
{
	if(m_lpMappingAddress)
	{
		UnmapViewOfFile(m_lpMappingAddress);
        m_nMappingLength  = 0;		// ӳ����ļ��鳤��
        m_lpMappingAddress = NULL;	// ӳ���Ļ���ַ
	}
}

// �ر�
void CTaiyanMappingFile::CloseMappingFile()
{
	UnMapView();

	if(m_hMappingFile)
	{
		CloseHandle(m_hMappingFile);
		m_hMappingFile = NULL;
	}
}

#else
#include <errno.h>
bool CTaiyanMappingFile::LoadFileToMem(const char *pszFileName, unsigned long dwOpenMode/* = MAPMODE_READONLY*/, size_t nFileOffset/* =0*/, size_t nMapBlockSize/* = MAX_MAP_BLOCK_SIZE*/)
{
	CloseFile();
    m_dwOpenMode     = dwOpenMode;
    m_nMapBlockSize = nMapBlockSize;
    m_nFileOffset     = nFileOffset;

    const char *pOpenMode = (m_dwOpenMode==MAPMODE_READONLY) ? "rb" : "wb";

	m_file = fopen(pszFileName, pOpenMode);
	if(m_file == NULL)
		return false;

    int prot = (m_dwOpenMode==MAPMODE_READONLY) ? PROT_READ|PROT_WRITE:PROT_READ;
	return CreateMappingFile(fileno(m_file), 0, NULL, 0, prot);
}

bool CTaiyanMappingFile::CreateMappingFile(int fd, size_t length/*=0*/, void *start/*=NULL*/, off_t offset/*=0*/, int prot/*=PROT_READ|PROT_WRITE*/, int flags/*=MAP_SHARED*/)
{
	if (0 == length)
	{
		struct stat fileStat; 
		fstat(fd, &fileStat);

		length = fileStat.st_size;
	}

	m_nFileLength = length;
	return MapNextBlock();
}

bool CTaiyanMappingFile::MapNextBlock()
{
    munmap(m_lpMappingAddress, m_nMappingLength);

    m_nFileOffset += m_nMappingLength;
    if (m_nFileOffset >= m_nFileLength)
        return false;

    if (m_nFileOffset+m_nMapBlockSize > m_nFileLength)
        m_nMappingLength = m_nFileLength-m_nFileOffset;
    else
        m_nMappingLength = m_nMapBlockSize;

    int prot = (m_dwOpenMode==MAPMODE_READONLY) ? PROT_READ:PROT_READ|PROT_WRITE;
    m_lpMappingAddress = mmap(NULL, m_nMappingLength, prot, MAP_SHARED, fileno(m_file), m_nFileOffset); 

    return m_lpMappingAddress != MAP_FAILED;
}

// �ر�
void CTaiyanMappingFile::CloseMappingFile()
{
	munmap(m_lpMappingAddress, m_nMappingLength);

    m_nMappingLength  = 0;		// ӳ����ļ��鳤��
    m_lpMappingAddress = NULL;	// ӳ���Ļ���ַ
}


void CTaiyanMappingFile::CloseFile()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}

    m_nFileLength     = 0;		// �ļ��ܳ���
    m_nMapBlockSize   = 0;      // ӳ���Ĵ�С
    m_nFileOffset     = 0;		// ӳ�俪ʼ���ļ�ƫ�Ƴ���
    m_nMappingLength  = 0;		// ӳ����ļ��鳤��
    m_lpMappingAddress = NULL;	// ӳ���Ļ���ַ
}

#endif

void * CTaiyanMappingFile::GetMappingHead()
{
    return m_lpMappingAddress;
}

void * CTaiyanMappingFile::GetMappingTail()
{
    return (char*)m_lpMappingAddress + m_nMappingLength;
}

size_t CTaiyanMappingFile::GetMappingLength()
{
    return m_nMappingLength;
}

size_t CTaiyanMappingFile::GetFileLength()
{
    return m_nFileLength;
}

