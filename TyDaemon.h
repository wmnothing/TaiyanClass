#pragma once

//void Daemonize();
void Daemonize(const char *cmd);
bool IsSingleInstanceRunning(const char * pszLockFileName);
bool SetSingleInstanceFlag(const char * pszLockFileName);
