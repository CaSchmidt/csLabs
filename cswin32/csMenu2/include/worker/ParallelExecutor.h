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

#ifndef PARALLELEXECUTOR_H
#define PARALLELEXECUTOR_H

#include <atomic>
#include <list>
#include <string>

#include "worker/IJobQueue.h"

class ParallelExecutor : public IJobQueue {
public:
  ParallelExecutor(const unsigned int numThreads,
                   const std::wstring& cmd,
                   const std::list<std::wstring>& files,
                   HWND window = nullptr);
  ~ParallelExecutor();

  unsigned int count() const final;
  bool isFinished() const final;
  void start() final;
  void next() final;
  void postFinished() final;
  void setProgressWindow(HWND window) final;

private:
  ParallelExecutor() = delete;
  ParallelExecutor(const ParallelExecutor&) = delete;
  ParallelExecutor& operator=(const ParallelExecutor&) = delete;
  ParallelExecutor(ParallelExecutor&&) = delete;
  ParallelExecutor& operator=(ParallelExecutor&&) = delete;

  void finishWork();
  static void worker(ParallelExecutor *ptr, const wchar_t *rawFilename);

  static const unsigned int _numMinThreads;
  static const unsigned int _numMaxThreads;

  unsigned int _numThreads;
  std::wstring _cmd;
  std::list<std::wstring> _files;
  unsigned int _numFiles;
  std::atomic_uint _cntFiles;
  std::atomic_uint _cntFilesPending;
  HWND _window;
};

#endif // PARALLELEXECUTOR_H
