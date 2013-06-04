///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2008.
// All right reserved!
// 文 件 名：Conf.cpp
// 版    本：1.0
// 功能描述：对 .conf 格式的配置文件进行解析 (不支持中文数据)
// 创建日期：2009-3-3 11:07:50
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
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

// 加载一个配置文件
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


// 解析一行配置参数 (不支持中文数据)
bool CTyConf::ParseLine(char *pszLine)
{
	if (*pszLine == '#')									// 注释行
		return true;

	if (*pszLine == '\0')									// 空行
	{
		return true;
	}

	char *pszComment = strchr((char *)pszLine, '#');
	if (pszComment)											// 如果行中有注释
		*pszComment = '\0';

	char *pszConfLine = TrimLine(pszLine);

	// 以 = 为分割符，前部为名称，后部为数据
	char *pszEqualMark = strchr(pszConfLine, '=');
	if (NULL == pszEqualMark)
		return false;

	*pszEqualMark = '\0';
	m_CurrentItem.Name  = TrimLine(pszConfLine);				// 配置的名称
	m_CurrentItem.Value = NormalizeValue(pszEqualMark + 1);		// 配置的数据

	if (strcasecmp(m_CurrentItem.Name.c_str(), "group") == 0)
	{
		m_CurrentItem.GroupName = m_CurrentItem.Value;
		return true;
	}

	m_listConfItem.push_back(m_CurrentItem);
	return true;
}

// 对设置的参数进行正常化
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

// 将一行字符前后两端的空字符去掉
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

// 查找一个配置条目的值
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

// 查找多个配置条目的值
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

