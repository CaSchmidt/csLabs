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

#include <vector>

#include <Windows.h>
#include <winnetwk.h>

#include "csCore2/csFileUtil.h"

#include "util.hpp"

#include "command.h"

std::size_t lenFN(const csWString& filename, const UINT cmd)
{
  using SizeT = csWString::size_type;

  std::size_t size = 0;

  const SizeT len    = std::wcslen(filename.data());
  const SizeT tag    = filename.rfind(L'\\');
  const SizeT tagPos = tag == csWString::npos || cmd != Cmd_List
      ? 0
      : tag + 1;

  size += len - tagPos;
  if( cmd == Cmd_ListWithPathTabular ) {
    size++; // separating '\t'
  }
  if( csIsDirectory(filename.data()) ) {
    size++; // trailing  '\\'
  }
  size += 2; // "\r\n"

  return size;
}

void catFN(wchar_t *text, std::size_t& pos,
           const csWString& filename, const UINT cmd)
{
  using SizeT = csWString::size_type;

  const SizeT len    = std::wcslen(filename.data());
  const SizeT tag    = filename.rfind(L'\\');
  const SizeT tagPos = tag == csWString::npos
      ? 0
      : tag + 1;

  if( cmd != Cmd_List  &&  tagPos > 0 ) {
    std::wcsncpy(&text[pos], filename.data(), tagPos);
    pos += tagPos;
  }

  // QUESTION: What to do in case of missing '\\'?
  if( cmd == Cmd_ListWithPathTabular ) {
    text[pos++] = L'\t';
  }

  std::wcsncpy(&text[pos], &filename[tagPos], len - tagPos);
  pos += len - tagPos;

  if( csIsDirectory(filename.data()) ) {
    text[pos++] = L'\\';
  }

  text[pos++] = L'\r';
  text[pos++] = L'\n';
}

csWString joinFileNames(const csWStringList& files)
{
  using SizeT = csWString::size_type;

  csWString result;

  SizeT size = 0;
  for(const csWString& filename : files) {
    if( std::wcslen(filename.data()) > 0 ) {
      size += 3 + std::wcslen(filename.data()); // '"' + <filename> + '"' + ' '
    }
  }

  if( size < 1 ) {
    return result;
  }

  try {
    result.reserve(size);

    for(const csWString& filename : files) {
      result += L'"';
      result += filename;
      result += L'"';
      result += L' ';
    }

    result[size - 1] = L'\0'; // Overwrite last ' ' with '\0'
  } catch(...) {
    result.clear();
    return result;
  }

  return result;
}

csWString quoteFileName(const csWString& filename)
{
  if( filename.empty() ) {
    return csWString();
  }

  return csWString(L'"' + filename + L'"');
}

csWString resolveUNC(const csWString& filename)
{
  using Buffer = std::vector<uint8_t>;

  csWString result;

  DWORD uncSize = sizeof(UNIVERSAL_NAME_INFOW);
  UNIVERSAL_NAME_INFOW uniwTemp;
  DWORD res = WNetGetUniversalNameW(filename.data(), UNIVERSAL_NAME_INFO_LEVEL,
                                    &uniwTemp, &uncSize);
  if( res != ERROR_MORE_DATA ) {
    return result;
  }

  try {
    uncSize += sizeof(wchar_t);
    Buffer uncData(uncSize, 0);

    res = WNetGetUniversalNameW(filename.data(), UNIVERSAL_NAME_INFO_LEVEL,
                                uncData.data(), &uncSize);
    if( res != NO_ERROR ) {
      return result;
    }

    UNIVERSAL_NAME_INFOW *uniw = reinterpret_cast<UNIVERSAL_NAME_INFOW*>(uncData.data());
    result.append(uniw->lpUniversalName, std::wcslen(uniw->lpUniversalName));
  } catch(...) {
    result.clear();
    return result;
  }

  return result;
}

HBITMAP createBitmapFromIcon(const HICON icon, const int width, const int height)
{
  if( icon == nullptr  ||  width < 1  ||  height < 1 ) {
    return nullptr;
  }

  HDC screen = GetDC(nullptr);
  if( screen == nullptr ) {
    return nullptr;
  }

  // Create a DC compatible with the screen's DC
  HDC dc = CreateCompatibleDC(screen);
  if( dc == nullptr ) {
    ReleaseDC(nullptr, screen);
    return nullptr;
  }

  // Create a BITMAP compatible with the screen's DC
  HBITMAP bitmap = CreateCompatibleBitmap(screen, width, height);
  if( bitmap == nullptr ) {
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);
    return nullptr;
  }

  HBITMAP old = static_cast<HBITMAP>(SelectObject(dc, bitmap));
  DrawIconEx(dc, 0, 0, icon, width, height, 0, nullptr, DI_NORMAL);
  SelectObject(dc, old);

  DeleteDC(dc);
  ReleaseDC(nullptr, screen);

  return bitmap;
}
