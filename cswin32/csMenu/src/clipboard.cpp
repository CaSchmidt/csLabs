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

#include <csCore2/csStringLib.h>

#include "clipboard.h"

bool setClipboardText(const wchar_t *text)
{
  const size_t len = csStringLen(text);
  if( len < 1 ) {
    return false;
  }

  if( OpenClipboard(NULL) == FALSE ) {
    return false;
  }
  EmptyClipboard();
  CloseClipboard();

  if( OpenClipboard(NULL) == FALSE ) {
    return false;
  }

  HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (len+1)*sizeof(wchar_t));
  if( hGlobal == NULL ) {
    CloseClipboard();
    return false;
  }

  wchar_t *dest = (wchar_t*)GlobalLock(hGlobal);
  CopyMemory(dest, text, len*sizeof(wchar_t));
  dest[len] = '\0';
  GlobalUnlock(hGlobal);

  SetClipboardData(CF_UNICODETEXT, hGlobal);
  CloseClipboard();

  return true;
}
