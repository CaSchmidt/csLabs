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

#define NOMINMAX
#include <Windows.h>

#include "clipboard.h"

bool setClipboardText(const wchar_t *text)
{
  const std::size_t len = std::wcslen(text);
  if( len < 1 ) {
    return false;
  }

  if( OpenClipboard(nullptr) == FALSE ) {
    return false;
  }
  EmptyClipboard();
  CloseClipboard();

  if( OpenClipboard(nullptr) == FALSE ) {
    return false;
  }

  HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (len+1)*sizeof(wchar_t));
  if( hGlobal == nullptr ) {
    CloseClipboard();
    return false;
  }

  wchar_t *dest = static_cast<wchar_t*>(GlobalLock(hGlobal));
  CopyMemory(dest, text, len*sizeof(wchar_t));
  dest[len] = L'\0';
  GlobalUnlock(hGlobal);

  SetClipboardData(CF_UNICODETEXT, hGlobal); // TODO: Handle possible errors...
  CloseClipboard();

  return true;
}
