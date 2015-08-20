#include <csCore2/csAlphaNum.h>
#include <csCore2/csFile.h>
#include <csCore2/csUtil.h>

#include "env.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CS_MIN_THREAD  ((int)1)
#define CS_MAX_THREAD  ((int)8)

////// Extern ////////////////////////////////////////////////////////////////

extern VOID CALLBACK ThreadPoolWorker(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK);

////// Global ////////////////////////////////////////////////////////////////

int       g_argc = 0;
wchar_t **g_argv = NULL;

PTP_WORK g_work = NULL;

int g_numFiles   = 0;
int g_numThreads = 0;

////// Public ////////////////////////////////////////////////////////////////

bool initEnv()
{
  // (1) Command Line ////////////////////////////////////////////////////////

  const wchar_t *cmdLine = GetCommandLineW();
  if( cmdLine == NULL ) {
    MessageBoxW(NULL, L"GetCommandLineW()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  g_argv = CommandLineToArgvW(cmdLine, &g_argc);
  if( g_argv == NULL ) {
    MessageBoxW(NULL, L"CommandLineToArgvW()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  if( g_argc < CS_ARGV_FIRSTFILE+1 ) {
    MessageBoxW(NULL,
                L"SYNTAX:\n\n"
                L"csParallel.EXE <count> <script> <file1> <...> <fileN>",
                L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  for(int i = CS_ARGV_FIRSTFILE-1; i < g_argc; i++) {
    if( !csIsFile(g_argv[i]) ) {
      MessageBoxW(NULL,
                  L"INPUT:\n\n"
                  L"csParallel.EXE <count> <script> <file1> <...> <fileN>",
                  L"Error",
                  MB_ICONERROR | MB_OK);
      return false;
    }
  }

  g_numFiles = g_argc-CS_ARGV_FIRSTFILE;

  g_numThreads = csToUInt(g_argv[1]);
  g_numThreads = csBound(CS_MIN_THREAD, g_numThreads, CS_MAX_THREAD);

  // (2) Thread Pool /////////////////////////////////////////////////////////

  g_work = CreateThreadpoolWork(ThreadPoolWorker, NULL, NULL);
  if( g_work == NULL ) {
    MessageBoxW(NULL, L"CreateThreadPoolWork()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

#if 0
  // (3) Processor Information ///////////////////////////////////////////////

  DWORD slpiSize = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION *slpi = NULL;
  if( GetLogicalProcessorInformation(slpi, &slpiSize) == FALSE  &&
      GetLastError() != ERROR_INSUFFICIENT_BUFFER ) {
    MessageBoxW(NULL, L"GetLogicalProcessorInformation(NULL,0)", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  slpi = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)csAlloc(slpiSize);
  if( slpi == NULL ) {
    MessageBoxW(NULL, L"csMalloc()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  if( GetLogicalProcessorInformation(slpi, &slpiSize) == FALSE ) {
    csFree(slpi);
    MessageBoxW(NULL, L"GetLogicalProcessorInformation()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  const int numProcessors = slpiSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
  for(int i = 0; i < numProcessors; i++) {
    printf("mask=0x%08X\n", slpi[i].ProcessorMask);
    printf("node=%u\n", slpi[i].NumaNode.NodeNumber);
    fflush(stdout);
  }
  csFree(slpi);
#endif

  return true;
}

void freeEnv()
{
  g_argc = 0;
  if( g_argv != NULL ) {
    LocalFree(g_argv);
  }
  g_argv = NULL;

  if( g_work != NULL ) {
    CloseThreadpoolWork(g_work);
  }
  g_work = NULL;

  g_numFiles = 0;

  g_numThreads = 0;
}
