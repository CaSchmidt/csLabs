/****************************************************************************
** Copyright (c) 2015, Carsten Schmidt. All rights reserved.
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

#include <Windows.h>
#include <objbase.h>
#include <ShlObj.h>

#include <csCore2/csFile.h>

#include "command.h"

#include "clipboard.h"
#include "reg.h"
#include "util.hpp"

bool executeCommand(const UINT cmd, const csWStringList& files)
{
  const csWString parallel  = regReadParallel();
  const DWORD parallelCount = regReadParallelCount();
  const bool    hasParallel = !parallel.empty()  &&  parallelCount > 1;

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
    int size = 0;
    for(csWStringList::const_iterator it = files.begin(); it != files.end(); it++) {
      wchar_t *uncName = 0;
      if( isUnc  &&  (uncName = resolveUNC(it->c_str())) != 0 ) {
        size += lenFN(csWString(uncName), cmd);
        delete[] uncName;
      } else {
        size += lenFN(*it, cmd);
      }
    }

    wchar_t *text = new wchar_t[size+1];
    if( text == 0 ) {
      return false;
    }

    int pos = 0;
    for(csWStringList::const_iterator it = files.begin(); it != files.end(); it++) {
      wchar_t *uncName = 0;
      if( isUnc  &&  (uncName = resolveUNC(it->c_str())) != 0 ) {
        catFN(text, pos, csWString(uncName), cmd);
        delete[] uncName;
      } else {
        catFN(text, pos, *it, cmd);
      }
    }
    text[size] = L'\0';

    if( files.size() == 1 ) {
      // Overwrite trailing <CR><LF>
      text[size-1] = text[size-2] = L'\0';
    }

    if( isUnix ) {
      replace(text, size, L'\\', L'/');
    }

    setClipboardText(text);
    delete[] text;

    return true;

  } else if( cmd == Cmd_CreateSymbolicLink ) {

    csWString symLink;

    IFileSaveDialog *saveDialog = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IFileSaveDialog, (LPVOID*)&saveDialog);
    if( hr == S_OK ) {
      saveDialog->SetTitle(L"Create symbolic link");

      const COMDLG_FILTERSPEC filterSpec = {
        L"All files", L"*.*"
      };
      saveDialog->SetFileTypes(1, &filterSpec);

      const FILEOPENDIALOGOPTIONS opts
          = FOS_OVERWRITEPROMPT
          | FOS_FORCEFILESYSTEM
          | FOS_PATHMUSTEXIST
          | FOS_CREATEPROMPT
          | FOS_NOREADONLYRETURN
          | FOS_NODEREFERENCELINKS
          | FOS_DONTADDTORECENT;
      saveDialog->SetOptions(opts);

      if( saveDialog->Show(NULL) == S_OK ) {
        IShellItem *item = NULL;
        if( saveDialog->GetResult(&item) == S_OK ) {
          wchar_t *filename = NULL;
          if( item->GetDisplayName(SIGDN_FILESYSPATH, &filename) == S_OK ) {
            symLink = filename;
            CoTaskMemFree(filename);
          }

          item->Release();
        }
      }

      saveDialog->Release();
    }

    if( !symLink.empty() ) {
      if( csFileExists(symLink.c_str()) ) {
        MessageBoxW(NULL, L"Symbolic link target already exists!",
                    L"Error", MB_OK | MB_ICONERROR);
        return false;
      }

      const DWORD linkFlags = csIsDirectory(files.front().c_str())
          ? SYMBOLIC_LINK_FLAG_DIRECTORY
          : 0;

      if( CreateSymbolicLinkW(symLink.c_str(),
                              files.front().c_str(),
                              linkFlags) == 0 ) {
        const DWORD lastError = GetLastError();

        csWString msg(L"ERROR(0x");
        msg += csWString::number(lastError, 16);
        msg += L"): ";
        msg += formatError(lastError);

        MessageBoxW(NULL, msg.c_str(),
                    L"Error", MB_OK | MB_ICONERROR);
        return false;
      }
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

  } else if( Cmd_ExecuteScripts <= cmd  &&  cmd < Cmd_ExecuteScripts+scripts.size() ) {
    csWString script(scriptPath + L"\\");
    UINT i = 0;
    for(csWStringList::const_iterator it = scripts.begin(); it != scripts.end(); it++) {
      if( i == cmd-Cmd_ExecuteScripts ) {
        script += *it;
        break;
      }
      i++;
    }

    if( isParallel ) {
      csWStringList args(files);
      args.push_front(script);
      args.push_front(csWString::number(parallelCount));
      ShellExecuteW(NULL, L"open", parallel.c_str(), joinFileNames(args).c_str(),
                    NULL, SW_SHOWNORMAL);
    } else { // DO NOT use parallelizer
      if( isBatch ) {
        const csWString args = joinFileNames(files);
        ShellExecuteW(NULL, L"open", script.c_str(), args.c_str(), NULL, SW_SHOWNORMAL);
      } else { // NO batch processing
        for(csWStringList::const_iterator it = files.begin();
            it != files.end(); it++) {
          ShellExecuteW(NULL, L"open", script.c_str(), quoteFileName(*it).c_str(),
                        NULL, SW_SHOWNORMAL);
        }
      }
    }
    return true;

  }

  return false;
}
