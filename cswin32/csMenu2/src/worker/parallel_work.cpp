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

#include <memory>

#include "worker/worker.h"

#include "worker/ParallelExecutor.h"
#include "worker/ProgressUI.h"

////// Global ////////////////////////////////////////////////////////////////

extern HINSTANCE g_hDllInst; // main.cpp

////// Public ////////////////////////////////////////////////////////////////

DWORD WINAPI parallel_work(LPVOID lpParameter)
{
  std::unique_ptr<WorkContext> ctx(reinterpret_cast<WorkContext*>(lpParameter));
  lpParameter = nullptr;

  if( IsGUIThread(TRUE) == FALSE ) {
    MessageBoxW(nullptr, L"IsGUIThread()", L"Error",
                MB_ICONERROR | MB_OK);
    return 0;
  }

  std::unique_ptr<ParallelExecutor> pex;
  try {
    pex = std::make_unique<ParallelExecutor>(ctx->numThreads, ctx->script, ctx->files);
  } catch(...) {
    return 0;
  }

  std::unique_ptr<ProgressUI> pui = ProgressUI::create(pex.release(), g_hDllInst, 480, 48);
  if( !pui ) {
    return 0;
  }

  pui->show();

  MSG msg;
  while( GetMessageW(&msg, nullptr, 0, 0) > 0 ) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  return 0;
}
