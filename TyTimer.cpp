#include "TyTimer.h"

CTyTimer::CTyTimer()
{
}

CTyTimer::~CTyTimer()
{
}

// 开始计时
void CTyTimer::StartTimer()
{
    ftime(&m_tmStart);
}

// 结束计时
void CTyTimer::EndTimer()
{
    ftime(&m_tmEnd);
}

// 获得计时时间(毫秒)
int CTyTimer::GetReleasedTime()
{
    return (m_tmEnd.time-m_tmStart.time)*1000 + (m_tmEnd.millitm- m_tmStart.millitm);
}

