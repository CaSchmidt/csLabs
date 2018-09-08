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

#include <cwchar>

#include <algorithm>
#include <iterator>
#include <thread>

#include "worker/ParallelExecutor.h"

#include "csCore2/csProcess.h"
#include "worker/ProgressUI.h"

////// static ////////////////////////////////////////////////////////////////

const unsigned int ParallelExecutor::_numMinThreads = 1;
const unsigned int ParallelExecutor::_numMaxThreads = 8;

////// public ////////////////////////////////////////////////////////////////

ParallelExecutor::ParallelExecutor(const unsigned int numThreads,
                                   const std::wstring& cmd,
                                   const std::list<std::wstring>& files,
                                   HWND window)
  : _numThreads(0)
  , _cmd(cmd)
  , _files(files)
  , _numFiles(0)
  , _cntFiles(0)
  , _cntFilesPending(0)
  , _window(window)
{
  _numThreads = std::max(_numMinThreads, std::min(numThreads, _numMaxThreads));

  _numFiles = static_cast<unsigned int>(_files.size());
  _cntFilesPending = _numFiles;
}

ParallelExecutor::~ParallelExecutor() = default;

unsigned int ParallelExecutor::count() const
{
  return _numFiles;
}

bool ParallelExecutor::isFinished() const
{
  return _cntFilesPending < 1;
}

void ParallelExecutor::start()
{
  for(unsigned int i = 0; i < std::min(_numFiles, _numThreads); i++) {
    next();
  }
}

void ParallelExecutor::next()
{
  if( _cntFiles >= _numFiles ) {
    return;
  }

  const unsigned int index = _cntFiles++;
  const auto it = std::next(_files.begin(), index);
  try {
    std::thread t(ParallelExecutor::worker, this, it->data());
    t.detach();
  } catch(...) {
    finishWork();
  }
}

void ParallelExecutor::postFinished()
{
}

void ParallelExecutor::setProgressWindow(HWND window)
{
  _window = window;
}

////// private ///////////////////////////////////////////////////////////////

void ParallelExecutor::finishWork()
{
  --_cntFilesPending;
  if( _window != nullptr ) {
    PostMessageW(_window, PUI_WM_JOBDONE, 0, 0);
  }
}

void ParallelExecutor::worker(ParallelExecutor *ptr, const wchar_t *rawFilename)
{
  try {
    std::wstring filename;
    filename.reserve(std::wcslen(rawFilename) + 3);

    filename += L'"';
    filename += rawFilename;
    filename += L'"';

    csExecProcess(ptr->_cmd.data(), filename.data());
  } catch(...) {
  }

  ptr->finishWork();
}
