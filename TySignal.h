#pragma once
#include <signal.h>
#include <map>

using namespace std;

typedef void (*SignalProcHandler)(int);

class CTySignal
{
public:
    CTySignal(void);
    virtual ~CTySignal(void);

    void MonitorAllSignal(SignalProcHandler funProcHandler=NULL);
    void MonitorSignal(int signalID);

    static void TySignalProc(int signalID);
    static void DefaultSignalProc(int signalID);

public:
    map<int, SignalProcHandler> m_mapOldHandleProc;
private:
    SignalProcHandler m_funDefaultSignalProc;
};

 
