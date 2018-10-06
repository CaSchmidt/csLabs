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

#define NOMINMAX
#include <Windows.h>

#include "args.h"

Args commandLineArgs() noexcept
{
  Args result;

  const wchar_t *cmdLine = GetCommandLineW();
  if( cmdLine == nullptr ) {
    MessageBoxW(nullptr, L"GetCommandLineW()", L"Error",
                MB_ICONERROR | MB_OK);
    return result;
  }

  int argc = 0;
  wchar_t **argv = CommandLineToArgvW(cmdLine, &argc);
  if( argv == nullptr ) {
    MessageBoxW(nullptr, L"CommandLineToArgvW()", L"Error",
                MB_ICONERROR | MB_OK);
    return result;
  }

  try {
    result.resize(static_cast<Args::size_type>(argc));
    for(Args::size_type i = 0; i < result.size(); i++) {
      result[i] = argv[i];
    }
  } catch(...) {
    result.clear();
  }

  LocalFree(argv);
  argv = nullptr;
  argc = 0;

  return result;
}
