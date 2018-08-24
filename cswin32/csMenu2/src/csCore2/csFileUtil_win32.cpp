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

#include <Windows.h>

#include "csCore2/csFileUtil.h"

bool csFileExists(const wchar_t *name)
{
  return GetFileAttributesW(name) != INVALID_FILE_ATTRIBUTES;
}

bool csIsDirectory(const wchar_t *name)
{
  const DWORD attr = GetFileAttributesW(name);
  if( attr == INVALID_FILE_ATTRIBUTES ) {
    return false;
  }
  return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool csIsFile(const wchar_t *name)
{
  const DWORD attr = GetFileAttributesW(name);
  if( attr == INVALID_FILE_ATTRIBUTES ) {
    return false;
  }
  const DWORD nofilemask =
      FILE_ATTRIBUTE_DEVICE      | FILE_ATTRIBUTE_DIRECTORY |
      FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_VIRTUAL;
  return (attr & nofilemask) == 0;
}

csWStringList csListDirectory(const wchar_t *path)
{
  csWStringList result;

  WIN32_FIND_DATAW data;
  HANDLE handle;
  if( (handle = FindFirstFileW(path, &data)) != INVALID_HANDLE_VALUE ) {
    result.emplace_back(data.cFileName);

    while( FindNextFileW(handle, &data) != 0 ) {
      result.emplace_back(data.cFileName);
    }

    FindClose(handle);
  }

  return result;
}
