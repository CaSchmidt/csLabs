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

#include <vector>

#include <OleCtl.h>

#include "csCore2/csFileUtil.h"

#include "reg.h"

////// Macros ////////////////////////////////////////////////////////////////

#define KEY_HKCU_CSMENU  L"Software\\csLabs\\csMenu"

////// Private ///////////////////////////////////////////////////////////////

namespace priv_reg {

  csWString readSzValue(HKEY key, const wchar_t *name)
  {
    DWORD dataSize = 0;
    if( RegGetValueW(key, nullptr, name, RRF_RT_REG_SZ, nullptr, nullptr, &dataSize)
        != ERROR_SUCCESS ) {
      return csWString();
    }

    std::vector<uint8_t> data;
    try {
      data.resize(dataSize, 0);

      if( RegGetValueW(key, nullptr, name, RRF_RT_REG_SZ, nullptr, data.data(), &dataSize) != ERROR_SUCCESS ) {
        return csWString();
      }
    } catch(...) {
      return csWString();
    }

    return csWString(reinterpret_cast<const wchar_t*>(data.data()));
  }

  HRESULT setValue(HKEY key, const wchar_t *name, const DWORD value)
  {
    return HRESULT_FROM_WIN32(RegSetValueExW(key, name, 0, REG_DWORD,
                                             reinterpret_cast<const BYTE*>(&value),
                                             sizeof(DWORD)));
  }

  HRESULT setValue(HKEY key, const wchar_t *name, const wchar_t *value)
  {
    const std::size_t len = std::wcslen(value);
    if( len < 1 ) {
      return E_POINTER;
    }
    return HRESULT_FROM_WIN32(RegSetValueExW(key, name, 0, REG_SZ,
                                             reinterpret_cast<const BYTE*>(value),
                                             (len+1)*sizeof(wchar_t)));
  }

}; // namespace priv_reg

////// Public ////////////////////////////////////////////////////////////////

HRESULT registerWithFiles(const wchar_t *clsId)
{
  HKEY key;
  HRESULT hr;

  csWString keyPath(L"*\\shellex\\ContextMenuHandlers\\");
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.data(),
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = priv_reg::setValue(key, nullptr, L"CS::Menu");
  if( FAILED(hr) ) {
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(key);

  return S_OK;
}

HRESULT registerWithDirectories(const wchar_t *clsId)
{
  HKEY key;
  HRESULT hr;

  csWString keyPath(L"Directory\\shellex\\ContextMenuHandlers\\");
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.data(),
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = priv_reg::setValue(key, nullptr, L"CS::Menu");
  if( FAILED(hr) ) {
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(key);

  return S_OK;
}

HRESULT registerWithCLSID(const wchar_t *clsId,
                      const wchar_t *moduleFilename)
{
  HKEY key;
  HRESULT hr;

  csWString keyPath(L"CLSID\\");
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.data(),
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = priv_reg::setValue(key, nullptr, L"CS::Menu");
  if( FAILED(hr) ) {
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  // InprocServer32 //////////////////////////////////////////////////////////

  HKEY is32Key;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(key, L"InprocServer32",
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, nullptr, &is32Key, nullptr));
  if( FAILED(hr) ) {
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  hr = priv_reg::setValue(is32Key, nullptr, moduleFilename);
  if( FAILED(hr) ) {
    RegCloseKey(is32Key);
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  hr = priv_reg::setValue(is32Key, L"ThreadingModel", L"Apartment");
  if( FAILED(hr) ) {
    RegCloseKey(is32Key);
    RegCloseKey(key);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(is32Key);
  RegCloseKey(key);

  return S_OK;
}

DWORD regReadFlags()
{
  HKEY    key;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, KEY_HKCU_CSMENU,
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_READ, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return 0;
  }

  DWORD flags = 0;
  DWORD size  = sizeof(DWORD);
  hr = HRESULT_FROM_WIN32(RegGetValueW(key, nullptr, L"Flags",
                                       RRF_RT_REG_DWORD, nullptr,
                                       &flags, &size));
  RegCloseKey(key);
  if( FAILED(hr) ) {
    return 0;
  }

  return flags;
}

void regWriteFlags(const DWORD flags)
{
  HKEY    key;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, KEY_HKCU_CSMENU,
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return;
  }

  hr = priv_reg::setValue(key, L"Flags", flags);
  RegCloseKey(key);
}

DWORD regReadParallelCount()
{
  HKEY    key;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, KEY_HKCU_CSMENU,
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_READ, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return 0;
  }

  DWORD count = 0;
  DWORD size  = sizeof(DWORD);
  hr = HRESULT_FROM_WIN32(RegGetValueW(key, nullptr, L"ParallelCount",
                                       RRF_RT_REG_DWORD, nullptr,
                                       &count, &size));
  RegCloseKey(key);
  if( FAILED(hr) ) {
    return 0;
  }

  return count;
}

csWString regReadScriptsPath()
{
  HKEY    key;
  HRESULT hr;

  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, KEY_HKCU_CSMENU,
                                          0, nullptr, REG_OPTION_NON_VOLATILE,
                                          KEY_READ, nullptr, &key, nullptr));
  if( FAILED(hr) ) {
    return csWString();
  }

  const csWString scriptsPath = priv_reg::readSzValue(key, L"Scripts");
  RegCloseKey(key);

  if( !csIsDirectory(scriptsPath) ) {
    return csWString();
  }

  return scriptsPath;
}

csWStringList regReadScripts()
{
  const csWString scriptsPath = regReadScriptsPath();
  if( scriptsPath.empty() ) {
    return csWStringList();
  }

  csWStringList scripts = csListDirectory(scriptsPath + L"\\*");
  for(csWStringList::iterator it = scripts.begin(); it != scripts.end(); ) {
    if( csIsFile(scriptsPath + L'\\' + *it) ) {
      it++;
    } else {
      it = scripts.erase(it);
    }
  }

  scripts.sort();

  return scripts;
}
