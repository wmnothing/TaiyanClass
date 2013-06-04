#pragma once
#include <iostream>
using namespace std;

class CTyMemFile
{
public:
	CTyMemFile(void);
	CTyMemFile(const CTyMemFile &tyMemFile);
	CTyMemFile(const char *pszFileName, int nFileLen);
	~CTyMemFile(void);

	bool Open(const char *pszFileName, int nFileLen);
	void Close();
	bool SetFileLength(int nBufLen);
	int  GetFileLength();

	char *GetBasePtr() const;
	char *GetPtr() const;

	bool Seek(int nPosition, unsigned int nFrom);
    int GetPosition();
	int Read(char *pContent, int nLen);
	int Write(const char *pContent, int nLen);

	unsigned long GetFreeBufferSize();
	void ZeorMemoryFile();

	void SetDirty(bool bIsDirty);
	bool IsDirty();

	CTyMemFile& operator= (const CTyMemFile &tyMemFile);

public:
	string m_strFileName;
	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

private:
	char *m_pMemBasePtr;
	char *m_ptrCurent;
	int  m_nFileLen;
	int  m_nMemLen;

	bool m_bIsDirty;
};
