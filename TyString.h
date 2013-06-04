#pragma once

#include <string>
using namespace std;

class CTyString 
 {
public:
    CTyString(void);
    virtual ~CTyString(void);

};


char * trim(const char *s);
char * trim_left(const char *s);
char * trim_right(const char *s);
char * tolower(const char *s);
char * toupper(const char *s);

string& tolower(string &s);
string& toupper(string &s);
string& trim_left(string& s);
string& trim_right(string& s);
string& trim(string& s);

void replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat);
void replace_all( std::string& str, const char * pOldStr, const char *pNewStr );
