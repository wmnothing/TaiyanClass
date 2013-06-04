#pragma once

#ifdef WIN32
    #include <Windows.h>
    static const char DIR_SLASH = '\\';
#else
#define Sleep(x) usleep((x)*1000)
    static const char DIR_SLASH = '/';
#endif

#ifdef WIN32
    #include <process.h>
    typedef void thread_void;
    #define thread_return return 
#else
    #include <pthread.h>
    typedef void * thread_void;
    #define thread_return return 0
#endif

static int StartThread(thread_void(*start_routine)(void*), void * lpParam)
{
#ifdef WIN32
    _beginthread(start_routine, 0, lpParam);
    return GetLastError();
#else
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    pthread_t infoThread;
    int nRetVal = pthread_create(&infoThread, &attr, start_routine, lpParam);

    pthread_attr_destroy (&attr);
    return nRetVal;
#endif
}


