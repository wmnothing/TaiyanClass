#pragma once
//#include <sys\timeb.h>
#include <sys/timeb.h>

class CTyTimer
{
public:
    CTyTimer();
    ~CTyTimer();

    void StartTimer();
    void EndTimer();

    int GetReleasedTime();

private:
    struct timeb m_tmStart;
    struct timeb m_tmEnd;
};
