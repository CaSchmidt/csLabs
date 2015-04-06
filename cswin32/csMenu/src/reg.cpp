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

#include "reg.h"

DWORD regReadFlags()
{
  HKEY    hkey;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\csLabs\\csMenu",
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_READ, NULL, &hkey, NULL));
  if( FAILED(hr) ) {
    return 0;
  }

  DWORD flags = 0;
  DWORD size  = sizeof(DWORD);
  hr = HRESULT_FROM_WIN32(RegGetValueW(hkey, NULL, L"Flags",
                                       RRF_RT_REG_DWORD, NULL,
                                       &flags, &size));
  RegCloseKey(hkey);
  if( FAILED(hr) ) {
    return 0;
  }

  return flags;
}

void regWriteFlags(const DWORD flags)
{
  HKEY    hkey;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\csLabs\\csMenu",
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, NULL, &hkey, NULL));
  if( FAILED(hr) ) {
    return;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueExW(hkey, L"Flags", 0, REG_DWORD,
                                         (const BYTE*)&flags, sizeof(DWORD)));
  RegCloseKey(hkey);
}
