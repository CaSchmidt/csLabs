#include <Windows.h>
#include <CommCtrl.h>

#include <csCore2/csStringLib.h>
#include <csCore2/csProcess.h>

#include "env.h"
#include "gui.h"

////// Thread pool's worker function /////////////////////////////////////////

VOID CALLBACK ThreadPoolWorker(PTP_CALLBACK_INSTANCE /*Instance*/,
                               PVOID /*Context*/,
                               PTP_WORK /*Work*/)
{
  const LONG id = InterlockedIncrement(&g_cntFiles);

  const wchar_t *filenameRaw = g_argv[id-1+CS_ARGV_FIRSTFILE];
  const size_t len = csStringLen(filenameRaw);
  wchar_t *filename = new wchar_t[len+3];
  if( filename != NULL ) {
    filename[0] = L'"';
    csStringNCpy(&filename[1], filenameRaw, len);
    filename[len+1] = L'"';
    filename[len+2] = L'\0';
    csExecProcess(g_argv[CS_ARGV_FIRSTFILE-1], filename);
    delete[] filename;
  }

  /*
  MessageBoxW(NULL, filenameRaw, L"Info", MB_ICONINFORMATION | MB_OK);
  Sleep(id*1000);
  */

  SendMessageW(g_hProgWnd, PBM_STEPIT, 0, 0);
  InterlockedDecrement(&g_cntFilesToGo);
  SendMessageW(g_hMainWnd, CS_WM_FILEDONE, 0, 0);
}

////// Main //////////////////////////////////////////////////////////////////

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
                      LPWSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
  if( !initEnv() ) {
    return 0;
  }

  if( !initGui(hInstance) ) {
    freeEnv();
    return 0;
  }

  MSG Msg;
  while( GetMessageW(&Msg, NULL, 0, 0) > 0 ) {
    TranslateMessage(&Msg);
    DispatchMessageW(&Msg);
  }

  freeEnv();

  return Msg.wParam;
}
