#pragma once
#include <stdio.h>

class CTyPipe
{
public:
    CTyPipe();
    ~CTyPipe();

    bool OpenPipe(const char *command, const char *mode);
    bool ClosePipe();
    bool IsPipeOpen();

    int ReadPipe(void *buffer, size_t count);
    char * GetLine(char* string, int n);

private:
    FILE * m_pFile;
};
