#ifndef __ENV_H__
#define __ENV_H__

#include <Windows.h>

#define CS_ARGV_FIRSTFILE  3

extern wchar_t **g_argv;

extern PTP_WORK g_work;

extern int g_numFiles;
extern int g_numThreads;

bool initEnv();
void freeEnv();

#endif // __ENV_H__
