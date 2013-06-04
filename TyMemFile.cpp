///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2008.
// All right reserved!
// 文 件 名：TyMemFile.cpp
// 版    本：1.0
// 功能描述：内存文件类
// 创建日期：2009-3-9 10:50:48
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
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

// 打开文件
bool CTyMemFile::Open(const char *pszFileName, int nFileLen)
{
	assert(pszFileName);

	m_strFileName = pszFileName;
	SetFileLength(nFileLen);
    Seek(0, CTyMemFile::begin);
    return true;
}

// 获得文件长度
int CTyMemFile::GetFileLength()
{
	return m_nFileLen;
}

// 设置文件长度
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

// 关闭文件
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

// 读取文件
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


// 写文件
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

// 重定位到
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

// 得到当前位置
int CTyMemFile::GetPosition()
{
    return m_ptrCurent - m_pMemBasePtr;
}


unsigned long CTyMemFile::GetFreeBufferSize()
{
	return (GetFileLength() - GetPosition());
}



// 得到内存开始处指针
char *CTyMemFile::GetBasePtr() const
{
	return m_pMemBasePtr;
}

// 得到当前内存指针
char *CTyMemFile::GetPtr() const
{
	return m_ptrCurent;
}

// 内存清空
void CTyMemFile::ZeorMemoryFile()
{
	memset(m_pMemBasePtr, 0, m_nFileLen);
	m_bIsDirty = true;
}

// 设置是否修改标志
void CTyMemFile::SetDirty(bool bIsDirty)
{
	m_bIsDirty = bIsDirty;
}

// 是否已修改
bool CTyMemFile::IsDirty()
{
	return m_bIsDirty;
}
