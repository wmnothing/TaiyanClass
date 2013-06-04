#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#include <iostream>
#include <sys/mman.h>
#include <stddef.h>
#endif

const int MAX_MAP_BLOCK_SIZE = 500*1024*1024;		// ����ӳ����С

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
    size_t m_nFileLength;			// �ļ��ܳ���
    size_t m_nMapBlockSize;         // ӳ���Ĵ�С
	size_t m_nFileOffset;			// ӳ�俪ʼ���ļ�ƫ�Ƴ���
	size_t m_nMappingLength;		// ӳ����ļ��鳤��
	void * m_lpMappingAddress;		// ӳ���Ļ���ַ

    unsigned long m_dwOpenMode;             // ��ģʽ

private:
#ifdef WIN32
	HANDLE m_hMappingFile;
    HANDLE m_hFile;
#else
    FILE *m_file;
#endif

};
