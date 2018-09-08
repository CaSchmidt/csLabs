/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "worker/ProgressUI.h"

////// public ////////////////////////////////////////////////////////////////

ProgressUI::ProgressUI(IJobQueue *jobs, const ProgressUI::ctor_tag&)
  : _jobs(jobs)
  , _mainWnd(nullptr)
  , _progWnd(nullptr)
{
}

ProgressUI::~ProgressUI() = default;

void ProgressUI::show() const
{
  ShowWindow(_mainWnd, SW_SHOWNORMAL);
}

std::unique_ptr<ProgressUI> ProgressUI::create(IJobQueue *jobs,
                                               HINSTANCE hInstance,
                                               const int width, const int height)
{
  std::unique_ptr<ProgressUI> result;

  // (1) Create object ///////////////////////////////////////////////////////

  try {
    result = std::make_unique<ProgressUI>(jobs, ctor_tag()); // NOTE: ProgressUI takes ownership of 'jobs'!
  } catch(...) {
    delete jobs; // NOTE: Assume ownership takeover failed...
    result.reset(nullptr);
    return result;
  }

  // (2) Register window class ///////////////////////////////////////////////

  const wchar_t *wndClassName = L"ProgressUI Class";

  WNDCLASSEXW wcex;
  wcex.cbSize        = sizeof(WNDCLASSEXW);
  wcex.style         = 0;
  wcex.lpfnWndProc   = ProgressUI::WindowProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = sizeof(LONG_PTR);
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
  wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = nullptr;
  wcex.lpszClassName = wndClassName;
  wcex.hIconSm       = LoadIcon(nullptr, IDI_APPLICATION);

  if( RegisterClassExW(&wcex) == 0 ) {
    MessageBoxW(nullptr, L"RegisterClassExW()", L"Error",
                MB_ICONERROR | MB_OK);
    result.reset(nullptr);
    return result;
  }

  // (3) Create main window //////////////////////////////////////////////////

  result->_mainWnd = CreateWindowExW(0, wndClassName, L"Progress", WS_BORDER,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     width, height + GetSystemMetrics(SM_CYSIZE),
                                     nullptr, nullptr, hInstance, nullptr);
  if( result->_mainWnd == nullptr ) {
    MessageBoxW(nullptr, L"CreateWindowExW(_mainWnd)", L"Error",
                MB_ICONERROR | MB_OK);
    result.reset(nullptr);
    return result;
  }

  SetWindowLongPtrW(result->_mainWnd, 0, reinterpret_cast<LONG_PTR>(result.get()));
  result->_jobs->setProgressWindow(result->_mainWnd);

  // (4) Initialize common controls //////////////////////////////////////////

  INITCOMMONCONTROLSEX iccex;
  iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  iccex.dwICC  = ICC_PROGRESS_CLASS;
  if( InitCommonControlsEx(&iccex) == FALSE ) {
    MessageBoxW(nullptr, L"InitCommonControlsEx()", L"Error",
                MB_ICONERROR | MB_OK);
    result.reset(nullptr);
    return result;
  }

  // (5) Create & initialize progress bar ////////////////////////////////////

  RECT rect;
  GetClientRect(result->_mainWnd, &rect);

  result->_progWnd = CreateWindowExW(0, PROGRESS_CLASSW, nullptr,
                                     WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                     rect.left, rect.top,
                                     rect.right-rect.left, rect.bottom-rect.top,
                                     result->_mainWnd, nullptr, hInstance, nullptr);
  if( result->_progWnd == nullptr ) {
    DestroyWindow(result->_mainWnd);
    MessageBoxW(nullptr, L"CreateWindowExW(_progWnd)", L"Error",
                MB_ICONERROR | MB_OK);
    result.reset(nullptr);
    return result;
  }

  PostMessageW(result->_progWnd, PBM_SETRANGE32, 0, (LPARAM)result->_jobs->count());
  PostMessageW(result->_progWnd, PBM_SETSTEP, (WPARAM)1, 0);

  return result;
}

////// private ///////////////////////////////////////////////////////////////

ProgressUI::ctor_tag::ctor_tag() = default;

LRESULT CALLBACK ProgressUI::WindowProc(HWND hWnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam)
{
  ProgressUI *pui = reinterpret_cast<ProgressUI*>(GetWindowLongPtrW(hWnd, 0));

  LRESULT result = 0;
  switch( msg ) {
  case PUI_WM_JOBDONE:
    PostMessageW(pui->_progWnd, PBM_STEPIT, 0, 0);
    if( pui->_jobs->isFinished() ) {
      pui->_jobs->postFinished();
      PostMessageW(pui->_mainWnd, WM_CLOSE, 0, 0);
    } else {
      pui->_jobs->next();
    }
    break;

  case WM_CREATE:
    break;

  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_SHOWWINDOW:
    pui->_jobs->start();
    break;

  default:
    result = DefWindowProcW(hWnd, msg, wParam, lParam);
    break;
  }

  return result;
}
