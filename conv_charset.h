#pragma once

#ifdef WIN32
#include <Windows.h>

static char * ConvCharset(unsigned int uCodePageFrom, unsigned int uCodePageTo, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    int iLen = MultiByteToWideChar(uCodePageFrom, 0, inbuf, -1, NULL, 0) ;
    WCHAR* bufWideChar = new WCHAR[iLen];
    MultiByteToWideChar(uCodePageFrom, 0, inbuf, -1, bufWideChar, iLen);

    iLen = WideCharToMultiByte(uCodePageTo, 0, (LPWSTR)bufWideChar, -1, NULL, 0, NULL, NULL) ;
    if (iLen > outlen)
        return "";

    char* bufMultiByte = outbuf;
    WideCharToMultiByte(uCodePageTo, 0, (LPWSTR)bufWideChar, -1, bufMultiByte, iLen, NULL, NULL);
    //CString strMultiByte(bufMultiByte);

    delete []bufWideChar;

	return outbuf;
}

#define ConvertGb2Utf(szSrc, szDest, nDestLen) ConvCharset(CP_ACP, CP_UTF8, szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertUtf2Gb(szSrc, szDest, nDestLen) ConvCharset(CP_UTF8, CP_ACP, szSrc, strlen(szSrc), szDest, nDestLen)
//#define ConvertUni2Utf(szSrc, szDest, nDestLen) ConvCharset("UNICODE", "UTF-8", szSrc, strlen(szSrc), szDest, nDestLen)
//#define ConvertUtf2Uni(szSrc, szDest, nDestLen) ConvCharset("UTF-8", "UNICODE", szSrc, strlen(szSrc), szDest, nDestLen)
//#define ConvertGb2Uni(szSrc, szDest, nDestLen) ConvCharset("GB2312", "UNICODE", szSrc, strlen(szSrc), szDest, nDestLen)
//#define ConvertUni2Gb(szSrc, szDest, nDestLen) ConvCharset("UNICODE", "GB2312", szSrc, strlen(szSrc), szDest, nDestLen)

#else
#include <iconv.h>
#include <stddef.h>

static char * ConvCharset(const char *from_charset, const char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    char *pRetBuf = outbuf;

	iconv_t cd = iconv_open(to_charset,from_charset);
	if (cd == iconv_t(-1))
		return NULL;

	iconv(cd, NULL, NULL, NULL, NULL);

	char **pin = &inbuf;
	char **pout = &outbuf;
	int nRetLen =  iconv(cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen);
    iconv_close(cd);

	if (nRetLen == -1)
		return NULL;

    return pRetBuf;
}
#define ConvertGb2Utf(szSrc, szDest, nDestLen) ConvCharset("GB2312", "UTF-8", szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertUtf2Gb(szSrc, szDest, nDestLen) ConvCharset("UTF-8", "GB2312", szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertUni2Utf(szSrc, szDest, nDestLen) ConvCharset("UNICODE", "UTF-8", szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertUtf2Uni(szSrc, szDest, nDestLen) ConvCharset("UTF-8", "UNICODE", szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertGb2Uni(szSrc, szDest, nDestLen) ConvCharset("GB2312", "UNICODE", szSrc, strlen(szSrc), szDest, nDestLen)
#define ConvertUni2Gb(szSrc, szDest, nDestLen) ConvCharset("UNICODE", "GB2312", szSrc, strlen(szSrc), szDest, nDestLen)
#endif

