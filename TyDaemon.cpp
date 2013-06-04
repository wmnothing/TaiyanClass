#include "TyDaemon.h"

#ifndef WIN32
#include <iostream>

#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <errno.h>

using namespace std;
#endif

//void Daemonize()
void Daemonize(const char *cmd)
{
#ifndef WIN32
    int                 i, fd0, fd1, fd2;
    pid_t               pid;
    struct rlimit       rl;
    struct sigaction    sa;

    // Clear file creation mask.
    umask(0);
 
   // Get maximum number of file descriptors.
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        exit(0);

    // Become a session leader to lose controlling TTY.
    if ((pid = fork()) < 0)
        exit(0);
    else if (pid != 0) /* parent */
        exit(0);
    setsid();

    // Ensure future opens won't allocate controlling TTYs.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        exit(0);
    if ((pid = fork()) < 0)
        exit(0);
    else if (pid != 0) /* parent */
        exit(0);

    // Change the current working directory to the root so
    // we won't prevent file systems from being unmounted.
    if (chdir("/") < 0)
        exit(0);

    // Close all open file descriptors.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    // Attach file descriptors 0, 1, and 2 to /dev/null.
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // Initialize the log file.
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) 
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
#endif
}

// #define LOCKFILE "/var/run/daemon.pid"
bool IsSingleInstanceRunning(const char * pszLockFileName)
{
#ifndef WIN32
    int fd = open(pszLockFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) 
    {
        cout << "can't open " << pszLockFileName << ": " << strerror(errno);
        return false;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) < 0) 
    {
        if (errno == EACCES || errno == EAGAIN) 
        {
            close(fd);
            return true;
        }
        cout << "can't lock " << pszLockFileName << ": " << strerror(errno);
    }

    close(fd);
#endif

    return false;
}

// #define LOCKFILE "/var/run/daemon.pid"
bool SetSingleInstanceFlag(const char * pszLockFileName)
{
#ifndef WIN32
    int     fd;
    char    buf[16];

    fd = open(pszLockFileName, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) 
    {
        cout << "can't open " << pszLockFileName << ": " << strerror(errno);
        return false;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) < 0) 
    {
        if (errno == EACCES || errno == EAGAIN) 
        {
            close(fd);
            return false;
        }
        cout << "can't lock " << pszLockFileName << ": " << strerror(errno);
        return false;
    }

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
#endif

    return true;
}
