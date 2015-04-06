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
#include <OleCtl.h>
#include <csCore/csString.h>
#include <csCore/csStringLib.h>

#include "csMenu.h"
#include "csMenuFactory.h"

HINSTANCE g_hDllInst     = NULL;
LONG      g_lDllRefCount = 0;

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
  if( !InlineIsEqualGUID(rclsid, __uuidof(csMenu)) ) {
    return CLASS_E_CLASSNOTAVAILABLE;
  }

  if( IsBadWritePtr(ppv, sizeof(void*)) ) {
    return E_POINTER;
  }
  *ppv = NULL;

  csMenuFactory *factory = new csMenuFactory();
  if( factory == 0 ) {
    return E_OUTOFMEMORY;
  }
  factory->AddRef();

  const HRESULT hrRet = factory->QueryInterface(riid, ppv);
  factory->Release();

  return hrRet;
}

HRESULT __stdcall DllCanUnloadNow(void)
{
  return g_lDllRefCount > 0  ?  S_FALSE : S_OK;
}

HRESULT __stdcall DllRegisterServer(void)
{
  wchar_t moduleFilename[MAX_PATH];
  if( GetModuleFileNameW(g_hDllInst, moduleFilename, MAX_PATH-1) == 0 ) {
    return SELFREG_E_CLASS;
  }

  HKEY hkey;
  HRESULT hr;

  wchar_t clsId[64];
  StringFromGUID2(GUID_csMenu, clsId, 64);

  csString keyPath;

  // (1) All files ///////////////////////////////////////////////////////////

  keyPath  = L"*\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.c_str(),
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, NULL, &hkey, NULL));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueW(hkey, NULL, REG_SZ,
                                       L"CS::Menu", 0));
  if( FAILED(hr) ) {
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(hkey);
  hkey = NULL;

  // (2) File folders ////////////////////////////////////////////////////////

  keyPath  = L"Directory\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.c_str(),
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, NULL, &hkey, NULL));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueW(hkey, NULL, REG_SZ,
                                       L"CS::Menu", 0));
  if( FAILED(hr) ) {
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(hkey);
  hkey = NULL;

  // (3) CLSID ///////////////////////////////////////////////////////////////

  keyPath  = L"CLSID\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath.c_str(),
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, NULL, &hkey, NULL));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueW(hkey, NULL, REG_SZ,
                                       L"CS::Menu", 0));
  if( FAILED(hr) ) {
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  // (4) CLSID\InprocServer32 ////////////////////////////////////////////////

  HKEY is32Key;
  hr = HRESULT_FROM_WIN32(RegCreateKeyExW(hkey, L"InprocServer32",
                                          0, NULL, REG_OPTION_NON_VOLATILE,
                                          KEY_WRITE, NULL, &is32Key, NULL));
  if( FAILED(hr) ) {
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueW(is32Key, NULL, REG_SZ,
                                       moduleFilename, 0));
  if( FAILED(hr) ) {
    RegCloseKey(is32Key);
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  hr = HRESULT_FROM_WIN32(RegSetValueExW(is32Key, L"ThreadingModel", 0, REG_SZ,
                                         (const BYTE*)L"Apartment",
                                         (csStringLen(L"Apartment")+1)*sizeof(wchar_t)));
  if( FAILED(hr) ) {
    RegCloseKey(is32Key);
    RegCloseKey(hkey);
    return SELFREG_E_CLASS;
  }

  RegCloseKey(is32Key);
  is32Key = NULL;

  RegCloseKey(hkey);
  hkey = NULL;

  return S_OK;
}

HRESULT __stdcall DllUnregisterServer(void)
{
  HRESULT hr;

  wchar_t clsId[64];
  StringFromGUID2(GUID_csMenu, clsId, 64);

  csString keyPath;

  // (1) All files ///////////////////////////////////////////////////////////

  keyPath  = L"*\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.c_str()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  // (2) File folders ////////////////////////////////////////////////////////

  keyPath  = L"Directory\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.c_str()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  // (3) CLSID ///////////////////////////////////////////////////////////////

  keyPath  = L"CLSID\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.c_str()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  return S_OK;
}

BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
                       LPVOID /*lpvReserved*/)
{
  if( fdwReason == DLL_PROCESS_ATTACH ) {
    g_hDllInst     = hinstDLL;
    g_lDllRefCount = 0;
    DisableThreadLibraryCalls(hinstDLL);
  }
  return TRUE;
}
