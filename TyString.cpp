///////////////////////////////////////////////////////////////////////////////
// 泰岩网络工作室 Copyright (c) 2012.
// All right reserved!
// 文 件 名：TyString.cpp
// 版    本：1.0
// 功能描述：
// 创建日期：2012-2-28 13:23:42
// 修改日期：
// 作    者： 吴旻( I_Have_Nothing@163.com  主页 http://www.taiyan.net/) 北京
// 口    号： 我 写 的 代 码 ， 是 个 人 就 能 看 懂 ！
// 修 改 者：
///////////////////////////////////////////////////////////////////////////////
#include "TyString.h"


char * trim(const char *s)
{
    return trim_left(trim_right(s));
}

char * trim_left(const char *s)
{
    if (!s || !*s)
        return (char *)s;

    // trim left
    char *left = (char *)s;

    while (*left && isspace(*left))
        left++;

    // if there is left space, copy mid char to left
    if(s != left)
    {
        char *src = left;
        char *dest = (char *)s;

        while(*src)
            *dest++ = *src++;
        *dest = 0;
    }
    return (char *)s;
}

char * trim_right(const char *s)
{
    if (!s || !*s)
        return (char *)s;

    // trim right
    char *right = (char *)s+strlen(s)-1;
    while(right>=s && isspace(*right))
        right--;
    *(right+1) = 0;

    return (char *)s;
}

char * tolower(const char *s)
{
    //transform(t.begin(), t.end(), t.begin(), tolower);

    if (s && *s)
    {
        char *p = (char *)s;
        while(*p)
            *p++ = ::tolower(*p);
    }

    return (char *)s;
}

char * toupper(const char *s)
{
    if (s && *s)
    {
        char *p = (char *)s;
        while(*p)
            *p++ = ::toupper(*p);
    }

    return (char *)s;
}

string& trim_left(string& s)
{
    trim_left((char *)s.c_str());
    s.resize(strlen(s.c_str()));

    return s;
}

string& trim_right(string& s)
{
    trim_right((char *)s.c_str());
    s.resize(strlen(s.c_str()));

    return s;
}

string& trim(string& s)
{
    trim((char *)s.c_str());
    s.resize(strlen(s.c_str()));

    return s;
}

string& tolower(string &s)
{
    tolower((char *)s.c_str());
    return s;
}

string& toupper(string &s)
{
    toupper((char *)s.c_str());
    return s;
}

void replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat)
{
    int count = 0;
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for(size_t pos = str.find(pattern, 0); 
        pos != std::string::npos;
        pos = str.find(pattern,pos + nsize))
    {
        str.replace(pos, psize, newpat);
        count++;
    }

    //return count;
} 

void replace_all( std::string& str, const char * pOldStr, const char *pNewStr )
{
    string old_str = pOldStr;
    string new_str = pNewStr;

    replace_all(str, old_str, new_str);
}
