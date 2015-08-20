#ifndef __GUI_H__
#define __GUI_H__

#include <Windows.h>
#include <CommCtrl.h>

#define CS_WM_FILEDONE  (WM_USER+1)

extern LONG g_cntFiles;
extern LONG g_cntFilesToGo;

extern HWND g_hMainWnd;
extern HWND g_hProgWnd;

bool initGui(HINSTANCE hInstance);

#endif // __GUI_H__
