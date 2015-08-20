#include <csCore2/csUtil.h>

#include "gui.h"

#include "env.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CS_WIDTH   480
#define CS_HEIGHT   48

////// Global ////////////////////////////////////////////////////////////////

LONG g_cntFiles = 0;
LONG g_cntFilesToGo = 0;

HWND g_hMainWnd = NULL;
HWND g_hProgWnd = NULL;

////// Private ///////////////////////////////////////////////////////////////

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT Msg,
                                WPARAM wParam, LPARAM lParam)
{
#ifdef HAVE_PAINT
  COLORREF color = RGB(255, 0, 0);
  HBRUSH brush;
  HDC dc;
  PAINTSTRUCT ps;
  RECT rect;
#endif

  switch( Msg ) {
  case CS_WM_FILEDONE:
    if( g_cntFiles < (LONG)g_numFiles ) {
      SubmitThreadpoolWork(g_work);
    }
    if( g_cntFilesToGo < 1 ) {
      SendMessageW(g_hMainWnd, WM_CLOSE, 0, 0);
    }
    return 0;

  case WM_CREATE:
    return 0;

  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_SHOWWINDOW:
    g_cntFilesToGo = g_numFiles;
    for(int i = 0; i < csMin(g_numFiles, g_numThreads); i++) {
      SubmitThreadpoolWork(g_work);
    }
    return 0;

#ifdef HAVE_PAINT
  case WM_PAINT:
    GetClientRect(hWnd, &rect);

    dc = BeginPaint(hWnd, &ps);

    brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);

    /*
    MoveToEx(dc, rect.left, rect.top+1, NULL); // V
    LineTo(dc,   rect.left, rect.bottom-1-1);

    MoveToEx(dc, rect.right-1, rect.top+1, NULL); // V
    LineTo(dc,   rect.right-1, rect.bottom-1-1);

    MoveToEx(dc, rect.left+1,    rect.top, NULL); // H
    LineTo(dc,   rect.right-1-1, rect.top);

    MoveToEx(dc, rect.left+1,    rect.bottom-1, NULL); // H
    LineTo(dc,   rect.right-1-1, rect.bottom-1);
    */

    EndPaint(hWnd, &ps);
    return 0;
#endif

  default:
    return DefWindowProcW(hWnd, Msg, wParam, lParam);
  }

  return 0;
}

////// Public ////////////////////////////////////////////////////////////////

bool initGui(HINSTANCE hInstance)
{
  const wchar_t *wndClassName = L"csParallel Class";

  WNDCLASSEXW wcex;
  wcex.cbSize        = sizeof(WNDCLASSEXW);
  wcex.style         = 0;
  wcex.lpfnWndProc   = MainWindowProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = wndClassName;
  wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

  if( !RegisterClassExW(&wcex) ) {
    MessageBoxW(NULL, L"RegisterClassExW()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  g_hMainWnd = CreateWindowExW(0, wndClassName, L"CS::Parallel", WS_BORDER,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               CS_WIDTH, CS_HEIGHT+GetSystemMetrics(SM_CYSIZE),
                               NULL, NULL, hInstance, NULL);
  if( g_hMainWnd == NULL ) {
    MessageBoxW(NULL, L"CreateWindowExW(g_hMainWnd)", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  INITCOMMONCONTROLSEX iccex;
  iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  iccex.dwICC  = ICC_PROGRESS_CLASS;
  if( !InitCommonControlsEx(&iccex) ) {
    MessageBoxW(NULL, L"InitCommonControlsEx()", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  RECT rect;
  GetClientRect(g_hMainWnd, &rect);

  g_hProgWnd = CreateWindowExW(0, PROGRESS_CLASSW, NULL,
                               WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                               rect.left, rect.top,
                               rect.right-rect.left, rect.bottom-rect.top,
                               g_hMainWnd, NULL, hInstance, NULL);
  if( g_hProgWnd == NULL ) {
    DestroyWindow(g_hMainWnd);
    MessageBoxW(NULL, L"CreateWindowExW(g_hProgWnd)", L"Error",
                MB_ICONERROR | MB_OK);
    return false;
  }

  ShowWindow(g_hMainWnd, SW_SHOWNORMAL);
  // UpdateWindow(g_hMainWnd); // Send WM_PAINT

  SendMessageW(g_hProgWnd, PBM_SETRANGE32, 0, (LPARAM)g_numFiles);
  SendMessageW(g_hProgWnd, PBM_SETSTEP, (WPARAM)1, 0);

  return true;
}
