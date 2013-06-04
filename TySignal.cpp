#include "TySignal.h"
#include <iostream>
#include <time.h>

CTySignal::CTySignal(void)
{
    m_funDefaultSignalProc = DefaultSignalProc;
}

CTySignal::~CTySignal(void)
{
    m_mapOldHandleProc.clear();
}

// 监控信号
void CTySignal::MonitorSignal(int signalID)
{
    m_mapOldHandleProc[signalID] = signal(signalID, m_funDefaultSignalProc);
}

// 监控所有信号
void CTySignal::MonitorAllSignal(SignalProcHandler funProcHandler)
{
    if(funProcHandler)
        m_funDefaultSignalProc = funProcHandler;

    MonitorSignal(SIGINT);
    MonitorSignal(SIGILL);
    MonitorSignal(SIGFPE);
    MonitorSignal(SIGSEGV);
    MonitorSignal(SIGTERM);
    MonitorSignal(SIGABRT);
}

void CTySignal::DefaultSignalProc(int signalID)
{
    TySignalProc(signalID);
    static int repeat_count = 0;

    if(++repeat_count > 10)
        exit(0);
}

// 新的处理过程
void CTySignal::TySignalProc(int signalID)
{
    char bufTime[32] = {0};
    time_t timeNow = time(NULL);
    strftime(bufTime, 32, "%Y%m%d-%H:%M:%S ", localtime( &timeNow));
    cout << bufTime << "\t";

    switch(signalID)
    {
    case SIGINT:        // CTRL+C signal
        cout << "CTRL+C signal ";
        break;
    case SIGILL:        // Illegal instruction
        cout << "Illegal instruction ";
        break;
    case SIGFPE:        // Floating-point error
        cout << "Floating-point error ";
        break;
    case SIGSEGV:       // Illegal storage access
       cout << "Illegal storage access ";
       break;
    case SIGTERM:       // Termination request
        cout << "Termination request ";
        break;
    case SIGABRT:       // Abnormal termination
        cout << "Abnormal termination ";
        break;
    default:
        cout << "Unknown signal ID ";
        break;
    }

    cout << "(" << signalID << ")" << endl;
}

