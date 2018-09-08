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

#ifndef PROGRESSUI_H
#define PROGRESSUI_H

#include <memory>

#define NOMINMAX
#include <Windows.h>
#include <CommCtrl.h>

#include "worker/IJobQueue.h"

#define PUI_WM_JOBDONE  (WM_USER + 1)

class ProgressUI {
private:
  struct ctor_tag {
    ctor_tag();
  };

public:
  ProgressUI(IJobQueue *jobs, const ctor_tag&);
  ~ProgressUI();

  void show() const;

  static std::unique_ptr<ProgressUI> create(IJobQueue *jobs,
                                            HINSTANCE hInstance,
                                            const int width, const int height);

private:  
  ProgressUI() = delete;
  ProgressUI(const ProgressUI&) = delete;
  ProgressUI& operator=(const ProgressUI&) = delete;
  ProgressUI(ProgressUI&&) = delete;
  ProgressUI& operator=(ProgressUI&&) = delete;

  static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg,
                                     WPARAM wParam, LPARAM lParam);

  std::unique_ptr<IJobQueue> _jobs;
  HWND _mainWnd;
  HWND _progWnd;
};

#endif // PROGRESSUI_H
