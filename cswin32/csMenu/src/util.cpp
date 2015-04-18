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
#include <winnetwk.h>
#include <csCore/csAlloc.h>
#include <csCore/csStringLib.h>

#include "util.hpp"

#include "command.h"

bool isDirectory(const wchar_t *filename)
{
  const DWORD attributes = GetFileAttributesW(filename);
  if( attributes == INVALID_FILE_ATTRIBUTES ) {
    return false;
  }
  return testFlags(attributes, FILE_ATTRIBUTE_DIRECTORY);
}

void replace(wchar_t *text, const int size,
             const wchar_t ch, const wchar_t after)
{
  for(int i = 0; i < size; i++) {
    if( text[i] == ch ) {
      text[i] = after;
    }
  }
}

int lenFN(const csString& filename, const UINT cmd)
{
  int size = 0;

  const int len    = filename.length();
  const int tag    = filename.lastIndexOf(L'\\');
  const int tagPos = tag < 0 || cmd != Cmd_List  ?  0 : tag+1;

  size += len - tagPos;
  if( cmd == Cmd_ListWithPathTabular ) {
    size++; // separating '\t'
  }
  if( isDirectory(filename) ) {
    size++; // trailing  '\\'
  }
  size += 2; // "\r\n"

  return size;
}

void catFN(wchar_t *text, int& pos,
           const csString& filename, const UINT cmd)
{
  const int len    = filename.length();
  const int tag    = filename.lastIndexOf(L'\\');
  const int tagPos = tag < 0  ?  0 : tag+1;

  if( cmd != Cmd_List  &&  tagPos > 0 ) {
    csStringNCpy(&text[pos], filename, tagPos);
    pos += tagPos;
  }

  // QUESTION: What to do in case of missing '\\'?
  if( cmd == Cmd_ListWithPathTabular ) {
    text[pos++] = L'\t';
  }

  csStringNCpy(&text[pos], &filename[tagPos], len-tagPos);
  pos += len - tagPos;

  if( isDirectory(filename) ) {
    text[pos++] = L'\\';
  }

  text[pos++] = L'\r';
  text[pos++] = L'\n';
}

wchar_t *resolveUNC(const wchar_t *filename)
{
  UNIVERSAL_NAME_INFOW uniwTemp;

  DWORD uncSize = sizeof(UNIVERSAL_NAME_INFOW);
  DWORD res = WNetGetUniversalNameW(filename, UNIVERSAL_NAME_INFO_LEVEL,
                                    &uniwTemp, &uncSize);
  if( res != ERROR_MORE_DATA ) {
    return 0;
  }

  uncSize += sizeof(wchar_t);
  void *uncData = csAlloc(uncSize);
  if( uncData == 0 ) {
    return 0;
  }

  res = WNetGetUniversalNameW(filename, UNIVERSAL_NAME_INFO_LEVEL,
                              uncData, &uncSize);
  if( res != NO_ERROR ) {
    csFree(uncData);
    return 0;
  }

  UNIVERSAL_NAME_INFOW *uniw = (UNIVERSAL_NAME_INFOW*)uncData;
  const size_t len = csStringLen(uniw->lpUniversalName);
  if( len < 1 ) {
    csFree(uncData);
    return 0;
  }

  wchar_t *uncName = (wchar_t*)csAlloc((len+1)*sizeof(wchar_t));
  if( uncName == 0 ) {
    csFree(uncData);
    return 0;
  }

  csStringNCpy(uncName, uniw->lpUniversalName, len);
  uncName[len] = L'\0';

  csFree(uncData);

  return uncName;
}

HBITMAP createBitmapFromIcon(const HICON icon, const int width, const int height)
{
  if( icon == NULL  ||  width < 1  ||  height < 1 ) {
    return NULL;
  }

  HDC screen = GetDC(NULL);
  if( screen == NULL ) {
    return NULL;
  }

  // Create a DC compatible with the screen's DC
  HDC dc = CreateCompatibleDC(screen);
  if( dc == NULL ) {
    ReleaseDC(NULL, screen);
    return NULL;
  }

  // Create a BITMAP compatible with the screen's DC
  HBITMAP bitmap = CreateCompatibleBitmap(screen, width, height);
  if( bitmap == NULL ) {
    DeleteDC(dc);
    ReleaseDC(NULL, screen);
    return NULL;
  }

  HBITMAP old = (HBITMAP)SelectObject(dc, bitmap);
  DrawIconEx(dc, 0, 0, icon, width, height, 0, NULL, DI_NORMAL);
  SelectObject(dc, old);

  DeleteDC(dc);
  ReleaseDC(NULL, screen);

  return bitmap;
}
