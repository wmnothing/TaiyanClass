#ifdef WIN32
    #include <io.h>
#else
    #include <iostream>
    #include <fcntl.h>
#endif

#include "TyPipe.h"

CTyPipe::CTyPipe()
{
    m_pFile = NULL;
}

CTyPipe::~CTyPipe()
{
    ClosePipe();
}

bool CTyPipe::OpenPipe( const char *command, const char *mode )
{
    ClosePipe();

#ifdef WIN32
    m_pFile = _popen(command, mode);
#else
    m_pFile = popen(command, mode);
    //fcntl(m_pFile->_fileno, F_SETFL, O_NONBLOCK);
#endif

    return (m_pFile!=NULL);
}

bool CTyPipe::ClosePipe()
{
    if (m_pFile)
    {
#ifdef WIN32
        int ret = _pclose(m_pFile);
#else
        int ret = pclose(m_pFile);
#endif
        m_pFile = NULL;
        return (ret!=-1);
    }

    return true;
}

int CTyPipe::ReadPipe(void *buffer, size_t count)
{
    if (m_pFile == NULL)
        return 0;

#ifdef WIN32
    return _read(fileno(m_pFile), buffer, count);
#else
    return read(fileno(m_pFile), buffer, count);
#endif


}

bool CTyPipe::IsPipeOpen()
{
    return (m_pFile!=NULL);
}

char * CTyPipe::GetLine( char* string, int n )
{
    return fgets(string, n, m_pFile);
}


