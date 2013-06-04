///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2008.
// All right reserved!
// �� �� ����TyMemFile.cpp
// ��    ����1.0
// �����������ڴ��ļ���
// �������ڣ�2009-3-9 10:50:48
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////
#include "TyMemFile.h"
#include <assert.h>

CTyMemFile::CTyMemFile(void)
{
	m_pMemBasePtr    = NULL;
	m_ptrCurent = NULL;
	m_nFileLen  = 0;
	m_nMemLen   = 0;
	m_bIsDirty  = false;
}

CTyMemFile::CTyMemFile(const CTyMemFile &tyMemFile)
{
	m_pMemBasePtr    = NULL;
	m_ptrCurent = NULL;
	m_nFileLen  = 0;
	m_nMemLen   = 0;
	m_bIsDirty  = false;

	*this = tyMemFile;
}

CTyMemFile::CTyMemFile(const char *pszFileName, int nFileLen)
{
	m_pMemBasePtr    = NULL;
	m_ptrCurent = NULL;
	m_nFileLen   = 0;
	m_nMemLen   = 0;
	m_bIsDirty  = false;

	Open(pszFileName, nFileLen);
}

CTyMemFile::~CTyMemFile(void)
{
	Close();
}

CTyMemFile& CTyMemFile::operator= (const CTyMemFile &tyMemFile)
{
	Open(tyMemFile.m_strFileName.c_str(), tyMemFile.m_nFileLen);
	if (tyMemFile.m_nFileLen > 0)
		memcpy(m_pMemBasePtr, tyMemFile.m_pMemBasePtr, tyMemFile.m_nFileLen);

	return *this;
}

// ���ļ�
bool CTyMemFile::Open(const char *pszFileName, int nFileLen)
{
	assert(pszFileName);

	m_strFileName = pszFileName;
	SetFileLength(nFileLen);
    Seek(0, CTyMemFile::begin);
    return true;
}

// ����ļ�����
int CTyMemFile::GetFileLength()
{
	return m_nFileLen;
}

// �����ļ�����
bool CTyMemFile::SetFileLength(int nFileLen)
{
	if (nFileLen <= 0)
		return true;

	if (nFileLen <= m_nMemLen)
	{
		m_nFileLen  = nFileLen;
		m_bIsDirty = true;
		return true;
	}

	char *temp   = new char [nFileLen];
	if (m_pMemBasePtr)
	{
		memcpy(temp, m_pMemBasePtr, m_nFileLen);
		Close();
	}

	m_pMemBasePtr    = temp;
	m_ptrCurent = m_pMemBasePtr;
	m_nFileLen  = nFileLen;
	m_nMemLen   = m_nFileLen;

	m_bIsDirty = true;
	return true;
}

// �ر��ļ�
void CTyMemFile::Close()
{
	if (m_pMemBasePtr)
	{
		delete [] m_pMemBasePtr;
		m_pMemBasePtr    = NULL;
		m_ptrCurent = NULL;
		m_nFileLen  = 0;
		m_nMemLen   = 0;
	}
}

// ��ȡ�ļ�
int CTyMemFile::Read(char *pContent, int nLen)
{
	assert(pContent);
	assert(nLen >= 0);

	int nFreeLen = m_nFileLen - (m_ptrCurent - m_pMemBasePtr);
	int nReadLen = nFreeLen < nLen ? nFreeLen : nLen;

	memcpy(pContent, m_ptrCurent, nReadLen);
	m_ptrCurent += nReadLen;

	return nReadLen;
}


// д�ļ�
int CTyMemFile::Write(const char *pContent, int nLen)
{
	assert(pContent);

	int nFreeLen = m_nFileLen - (m_ptrCurent - m_pMemBasePtr);
	int nWrittenLen = nFreeLen < nLen ? nFreeLen : nLen;

	memcpy(m_ptrCurent, pContent, nWrittenLen);
	m_ptrCurent += nWrittenLen;

	m_bIsDirty = true;
	return nWrittenLen;
}

// �ض�λ��
bool CTyMemFile::Seek(int nPosition, unsigned int nFrom)
{
	switch (nFrom)
	{
	case begin:
		if (nPosition < 0 || nPosition >= m_nFileLen)
			m_ptrCurent = m_pMemBasePtr + m_nFileLen;
		else
			m_ptrCurent = m_pMemBasePtr + nPosition;
		break;
	case end:
		m_ptrCurent = m_pMemBasePtr + (m_nMemLen - nPosition);
		break;
	default:
		m_ptrCurent += nPosition;
		break;
	}

	return true;
}

// �õ���ǰλ��
int CTyMemFile::GetPosition()
{
    return m_ptrCurent - m_pMemBasePtr;
}


unsigned long CTyMemFile::GetFreeBufferSize()
{
	return (GetFileLength() - GetPosition());
}



// �õ��ڴ濪ʼ��ָ��
char *CTyMemFile::GetBasePtr() const
{
	return m_pMemBasePtr;
}

// �õ���ǰ�ڴ�ָ��
char *CTyMemFile::GetPtr() const
{
	return m_ptrCurent;
}

// �ڴ����
void CTyMemFile::ZeorMemoryFile()
{
	memset(m_pMemBasePtr, 0, m_nFileLen);
	m_bIsDirty = true;
}

// �����Ƿ��޸ı�־
void CTyMemFile::SetDirty(bool bIsDirty)
{
	m_bIsDirty = bIsDirty;
}

// �Ƿ����޸�
bool CTyMemFile::IsDirty()
{
	return m_bIsDirty;
}
