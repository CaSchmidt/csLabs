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
#include <memory>

#define NOMINMAX
#include <Windows.h>
#include <objbase.h>
#include <ShlObj.h>

#include "csCore2/csFileUtil.h"

#include "command.h"

#include "clipboard.h"
#include "reg.h"
#include "util.hpp"
#include "worker/worker.h"

bool executeCommand(const UINT cmd, const csWStringList& files)
{
  const DWORD parallelCount = regReadParallelCount();
  const bool    hasParallel = parallelCount > 1;

  const DWORD flags      = regReadFlags();
  const bool  isBatch    = testFlags(flags, CMD_FLAG_BATCH);
  const bool  isParallel = testFlags(flags, CMD_FLAG_PARALLEL)  &&  hasParallel;
  const bool  isUnc      = testFlags(flags, CMD_FLAG_UNC)  &&  cmd != Cmd_List;
  const bool  isUnix     = testFlags(flags, CMD_FLAG_UNIX);

  const csWString  scriptPath = regReadScriptsPath();
  const csWStringList scripts = regReadScripts();

  if(        cmd == Cmd_List                ||
             cmd == Cmd_ListWithPath        ||
             cmd == Cmd_ListWithPathTabular ) {
    std::size_t size = 0;
    for(const csWString& filename : files) {
      csWString uncName;
      if( isUnc  &&  std::wcslen((uncName = resolveUNC(filename)).data()) > 0 ) {
        size += lenFN(uncName, cmd);
      } else {
        size += lenFN(filename, cmd);
      }
    }

    try {
      csWString text(size, 0);

      std::size_t pos = 0;
      for(const csWString& filename : files) {
        csWString uncName;
        if( isUnc  &&  std::wcslen((uncName = resolveUNC(filename)).data()) > 0 ) {
          catFN(const_cast<wchar_t*>(text.data()), pos, uncName, cmd);
        } else {
          catFN(const_cast<wchar_t*>(text.data()), pos, filename, cmd);
        }
      }

      if( files.size() == 1 ) {
        // Remove trailing <CR><LF>
        text.erase(size - 2);
      }

      if( isUnix ) {
        std::replace(text.begin(), text.end(), L'\\', L'/');
      }

      setClipboardText(text.data());
    } catch(...) {
      return false;
    }
    return true;

  } else if( cmd == Cmd_CheckBatchProcessing ) {
    regWriteFlags(flags ^ CMD_FLAG_BATCH);
    return true;

  } else if( cmd == Cmd_CheckParallelExecution ) {
    regWriteFlags(flags ^ CMD_FLAG_PARALLEL);
    return true;

  } else if( cmd == Cmd_CheckResolveUncPaths ) {
    regWriteFlags(flags ^ CMD_FLAG_UNC);
    return true;

  } else if( cmd == Cmd_CheckUnixPathSeparators ) {
    regWriteFlags(flags ^ CMD_FLAG_UNIX);
    return true;

  } else if( Cmd_ExecuteScripts <= cmd  &&  cmd < Cmd_ExecuteScripts + scripts.size() ) {
    const csWStringList::const_iterator it = std::next(scripts.begin(), cmd - Cmd_ExecuteScripts);
    const csWString script(scriptPath + L'\\' + *it);

    std::unique_ptr<WorkContext> ctx;
    try {
      ctx = std::make_unique<WorkContext>(script, parallelCount, files);
    } catch(...) {
      return false;
    }

    if( isParallel ) {
      startWork(parallel_work, ctx.release());
    } else { // DO NOT use parallelizer
      if( isBatch ) {
        startWork(batch_work, ctx.release());
      } else { // NO batch processing
        startWork(sequential_work, ctx.release());
      }
    }
    return true;

  }

  return false;
}
