#include "TyTimer.h"

CTyTimer::CTyTimer()
{
}

CTyTimer::~CTyTimer()
{
}

// ��ʼ��ʱ
void CTyTimer::StartTimer()
{
    ftime(&m_tmStart);
}

// ������ʱ
void CTyTimer::EndTimer()
{
    ftime(&m_tmEnd);
}

// ��ü�ʱʱ��(����)
int CTyTimer::GetReleasedTime()
{
    return (m_tmEnd.time-m_tmStart.time)*1000 + (m_tmEnd.millitm- m_tmStart.millitm);
}

