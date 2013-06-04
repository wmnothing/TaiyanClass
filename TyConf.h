#pragma once
#include <iostream>
#include <list>
using namespace std;

typedef struct _CONF_ITEM
{
	string GroupName;			// 组名称
	string Name;				// 名称
	string Value;				// 值
}CONF_ITEM;

class CTyConf
{
public:
	CTyConf(void);
	~CTyConf(void);

	bool Load(const char * pszFileName);
	bool FindValue(const char *pszGroupName, const char *pszName, string &strValue);
	bool FindValue(const char *pszGroupName, const char *pszName, list<string> &strValueArray);

private:
	bool ParseLine(char *pszLine);
	char * TrimLine(char *pszLine);
	char * NormalizeValue(char *str);

	list <CONF_ITEM> m_listConfItem;
	CONF_ITEM m_CurrentItem;
};


