///////////////////////////////////////////////////////////////////////////////
// ̩�����繤���� Copyright (c) 2008.
// All right reserved!
// �� �� ����Conf.cpp
// ��    ����1.0
// ������������ .conf ��ʽ�������ļ����н��� (��֧����������)
// �������ڣ�2009-3-3 11:07:50
// �޸����ڣ�
// ��    �ߣ� ��F( I_Have_Nothing@163.com  ��ҳ http://www.taiyan.net/) ����
// ��    �ţ� �� д �� �� �� �� �� �� �� �� �� �� �� ��
// �� �� �ߣ�
///////////////////////////////////////////////////////////////////////////////
#include "TyConf.h"
#include "TyFile.h"

#include <ctype.h>
#include <string.h>

#ifdef WIN32
    #define strcasecmp strcmp
#endif

#define MAX_LINE_BUFFER 1024*5

CTyConf::CTyConf(void)
{
}

CTyConf::~CTyConf(void)
{
}

// ����һ�������ļ�
bool CTyConf::Load(const char * pszFileName)
{
	CTyFile confFile;
	if (confFile.Open(pszFileName, "r"))
	{
		m_listConfItem.clear();
		char szLineBuf[MAX_LINE_BUFFER];
		while (confFile.GetLine(szLineBuf, MAX_LINE_BUFFER))
		{
			ParseLine(szLineBuf);
		}
		return true;
	}

	return false;
}


// ����һ�����ò��� (��֧����������)
bool CTyConf::ParseLine(char *pszLine)
{
	if (*pszLine == '#')									// ע����
		return true;

	if (*pszLine == '\0')									// ����
	{
		return true;
	}

	char *pszComment = strchr((char *)pszLine, '#');
	if (pszComment)											// ���������ע��
		*pszComment = '\0';

	char *pszConfLine = TrimLine(pszLine);

	// �� = Ϊ�ָ����ǰ��Ϊ���ƣ���Ϊ����
	char *pszEqualMark = strchr(pszConfLine, '=');
	if (NULL == pszEqualMark)
		return false;

	*pszEqualMark = '\0';
	m_CurrentItem.Name  = TrimLine(pszConfLine);				// ���õ�����
	m_CurrentItem.Value = NormalizeValue(pszEqualMark + 1);		// ���õ�����

	if (strcasecmp(m_CurrentItem.Name.c_str(), "group") == 0)
	{
		m_CurrentItem.GroupName = m_CurrentItem.Value;
		return true;
	}

	m_listConfItem.push_back(m_CurrentItem);
	return true;
}

// �����õĲ�������������
char *CTyConf::NormalizeValue(char *pszValue)
{
	char *str = TrimLine(pszValue);
	if (*str != '"')
		return str;

	str++; 	// skip leading '"' 
	char *pszNolizedValue = new char [strlen(str) + 1];
    memset(pszNolizedValue, 0, strlen(str) + 1);
	char *p = pszNolizedValue;

	while (*str != '"' && *str != '\0')
	{
		switch (*str)
		{
		case '\\':
			switch (str[1]) 
			{
			case '\0':
				*p++ = '\\';
				str += 1;
				break;
			case '\\':
				*p++ = '\\';
				str += 2;
				break;
			case '"':
				*p++ = '"';
				str += 2;
				break;
			default:
				*p++ = '\\';
				*p++ = str[1];
				str += 2;
				break;
			}
			break;
		default:
			*p++ = *str++;
			break;
		}
	}
	*p = '\0';

	strcpy(pszValue, pszNolizedValue);
	delete [] pszNolizedValue;

	return pszValue;
}

// ��һ���ַ�ǰ�����˵Ŀ��ַ�ȥ��
char * CTyConf::TrimLine(char *pszLine)
{
	unsigned char *begin = (unsigned char *)pszLine;
	while (isspace(*begin))
		begin++;

	unsigned char *end = (unsigned char *)strchr((char *)begin, '\0');
	while (begin < end && isspace(*(end-1)))
		end--;

	*end = '\0';

	return (char *)begin;
}

// ����һ��������Ŀ��ֵ
bool CTyConf::FindValue(const char *pszGroupName, const char *pszName, string &strValue)
{
	list<CONF_ITEM>::iterator it;
	for (it=m_listConfItem.begin(); it!=m_listConfItem.end(); ++it)
	{
		if (strcasecmp(pszGroupName, it->GroupName.c_str()) == 0
			&& strcasecmp(pszName, it->Name.c_str()) == 0)
		{
			strValue = it->Value;
			return true;
		}
	}

	return false;
}

// ���Ҷ��������Ŀ��ֵ
bool CTyConf::FindValue(const char *pszGroupName, const char *pszName, list<string> &strValueArray)
{
	list<CONF_ITEM>::iterator it;
	for (it=m_listConfItem.begin(); it!=m_listConfItem.end(); ++it)
	{
		if (strcasecmp(pszGroupName, it->GroupName.c_str()) == 0
			&& strcasecmp(pszName, it->Name.c_str()) == 0)
		{
			strValueArray.push_back(it->Value);
		}
	}

	return !strValueArray.empty();
}

