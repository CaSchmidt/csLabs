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
#include <OleCtl.h>

#include "csCore2/csWString.h"
#include "csMenu.h"
#include "csMenuFactory.h"
#include "reg.h"

HINSTANCE g_hDllInst     = nullptr;
LONG      g_lDllRefCount = 0;

HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
  if( !InlineIsEqualGUID(rclsid, __uuidof(csMenu)) ) {
    return CLASS_E_CLASSNOTAVAILABLE;
  }

  if( IsBadWritePtr(ppv, sizeof(void*)) ) {
    return E_POINTER;
  }
  *ppv = nullptr;

  csMenuFactory *factory = nullptr;
  try {
    factory = new csMenuFactory();
    factory->AddRef();
  } catch(...) {
    return E_OUTOFMEMORY;
  }

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
  HRESULT hr;

  wchar_t moduleFilename[MAX_PATH];
  if( GetModuleFileNameW(g_hDllInst, moduleFilename, MAX_PATH-1) == 0 ) {
    return SELFREG_E_CLASS;
  }

  wchar_t clsId[64];
  StringFromGUID2(GUID_csMenu, clsId, 64);

  // (1) All files ///////////////////////////////////////////////////////////

  hr = registerWithFiles(clsId);
  if( FAILED(hr) ) {
    return hr;
  }

  // (2) File folders ////////////////////////////////////////////////////////

  hr = registerWithDirectories(clsId);
  if( FAILED(hr) ) {
    return hr;
  }

  // (3) CLSID ///////////////////////////////////////////////////////////////

  hr = registerWithCLSID(clsId, moduleFilename);
  if( FAILED(hr) ) {
    return hr;
  }

  return S_OK;
}

HRESULT __stdcall DllUnregisterServer(void)
{
  HRESULT hr;

  wchar_t clsId[64];
  StringFromGUID2(GUID_csMenu, clsId, 64);

  csWString keyPath;

  // (1) All files ///////////////////////////////////////////////////////////

  keyPath  = L"*\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.data()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  // (2) File folders ////////////////////////////////////////////////////////

  keyPath  = L"Directory\\shellex\\ContextMenuHandlers\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.data()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  // (3) CLSID ///////////////////////////////////////////////////////////////

  keyPath  = L"CLSID\\";
  keyPath += clsId;
  hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, keyPath.data()));
  if( FAILED(hr) ) {
    return SELFREG_E_CLASS;
  }

  return S_OK;
}

BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
  if( fdwReason == DLL_PROCESS_ATTACH ) {
    g_hDllInst     = hinstDLL;
    g_lDllRefCount = 0;
    DisableThreadLibraryCalls(hinstDLL);
  }
  return TRUE;
}
