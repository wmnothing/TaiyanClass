#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#include <iostream>
#include <sys/mman.h>
#include <stddef.h>
#endif

const int MAX_MAP_BLOCK_SIZE = 500*1024*1024;		// 最大的映射块大小

class CTaiyanMappingFile
{
public:
	CTaiyanMappingFile(void);
	~CTaiyanMappingFile(void);

	bool MapNextBlock();

	void CloseMappingFile();
	void CloseFile();

    void *GetMappingHead();
    void *GetMappingTail();
    size_t GetMappingLength();
    size_t GetFileLength();

    enum{MAPMODE_READONLY=1, MAPMODE_READWRITE=2};

    bool LoadFileToMem(const char *pszFileName, unsigned long dwOpenMode=MAPMODE_READONLY, size_t nFileOffset=0, size_t nMapBlockSize=MAX_MAP_BLOCK_SIZE);

#ifdef WIN32

	bool CreateMappingFile(HANDLE hFile, LPCTSTR lpName=NULL, DWORD dwMaximumSize=0, DWORD dwOpenMode = MAPMODE_READONLY, LPSECURITY_ATTRIBUTES lpAttributes = NULL, BOOL bIsMapView = TRUE);
	bool OpenMappingFile(LPCTSTR lpName, DWORD dwOpenMode = MAPMODE_READONLY, BOOL bInheritHandle = FALSE, BOOL bIsMapView = TRUE);

	bool MapView(DWORD dwFileOffset = 0, SIZE_T dwNumberOfBytesToMap = 0);
	void UnMapView();
#else
	bool CreateMappingFile(int fd, size_t length=0, void *start=NULL, off_t offset=0, int prot=PROT_READ/*|PROT_WRITE*/, int flags=MAP_SHARED);
#endif

private:
    size_t m_nFileLength;			// 文件总长度
    size_t m_nMapBlockSize;         // 映射块的大小
	size_t m_nFileOffset;			// 映射开始的文件偏移长度
	size_t m_nMappingLength;		// 映射的文件块长度
	void * m_lpMappingAddress;		// 映射后的基地址

    unsigned long m_dwOpenMode;             // 打开模式

private:
#ifdef WIN32
	HANDLE m_hMappingFile;
    HANDLE m_hFile;
#else
    FILE *m_file;
#endif

};
