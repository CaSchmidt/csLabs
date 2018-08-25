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

#include <Shlwapi.h>

#include "csMenuFactory.h"

#include "csMenu.h"

////// Global ////////////////////////////////////////////////////////////////

extern LONG g_lDllRefCount;

////// public ////////////////////////////////////////////////////////////////

csMenuFactory::csMenuFactory()
  : m_lRefCount()
{
  InterlockedIncrement(&g_lDllRefCount);
}

csMenuFactory::~csMenuFactory()
{
  InterlockedDecrement(&g_lDllRefCount);
}

////// IUnknown //////////////////////////////////////////////////////////////

ULONG csMenuFactory::AddRef()
{
  return static_cast<ULONG>(InterlockedIncrement(&m_lRefCount));
}

HRESULT csMenuFactory::QueryInterface(const IID& riid, void **ppvObject)
{
  static const QITAB qit[] = {
    QITABENT(csMenuFactory, IClassFactory),
    { nullptr, 0 }
  };
  return QISearch(this, qit, riid, ppvObject);
}

ULONG csMenuFactory::Release()
{
  const LONG lRet = InterlockedDecrement(&m_lRefCount);
  if( m_lRefCount == 0 ) {
    delete this;
  }
  return static_cast<ULONG>(lRet);
}

////// IClassFactory /////////////////////////////////////////////////////////

HRESULT csMenuFactory::CreateInstance(IUnknown *pUnkOuter, const IID& riid, void **ppvObject)
{
  if( pUnkOuter != nullptr ) {
    return CLASS_E_NOAGGREGATION;
  }

  if( IsBadWritePtr(ppvObject, sizeof(void*)) ) {
    return E_POINTER;
  }
  *ppvObject = nullptr;

  csMenu *menu = nullptr;
  try {
    menu = new csMenu();
  } catch(...) {
    return E_OUTOFMEMORY;
  }

  const HRESULT hrRet = menu->QueryInterface(riid, ppvObject);
  if( FAILED(hrRet) ) {
    delete menu;
  }

  return hrRet;
}

HRESULT csMenuFactory::LockServer(BOOL fLock)
{
  if( fLock != FALSE ) {
    InterlockedIncrement(&g_lDllRefCount);
  } else {
    InterlockedDecrement(&g_lDllRefCount);
  }
  return S_OK;
}
