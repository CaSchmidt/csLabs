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

#include <Shlwapi.h>

#include <csCore/csAlloc.h>

#include "csMenu.h"

#include "command.h"
#include "menu.h"
#include "reg.h"
#include "util.hpp"

////// Global ////////////////////////////////////////////////////////////////

extern LONG g_lDllRefCount;

////// GUID //////////////////////////////////////////////////////////////////

const GUID GUID_csMenu = { /* 1A2306A9-DC34-46F0-84CA-7FEE7387CA37 */
                           0x1A2306A9,
                           0xDC34,
                           0x46F0,
                           {0x84, 0xCA, 0x7F, 0xEE, 0x73, 0x87, 0xCA, 0x37}
                         };

////// public ////////////////////////////////////////////////////////////////

csMenu::csMenu()
  : m_lRefCount()
  , _files()
{
  InterlockedIncrement(&g_lDllRefCount);
}

csMenu::~csMenu()
{
  InterlockedDecrement(&g_lDllRefCount);
}

////// IUnknown //////////////////////////////////////////////////////////////

ULONG csMenu::AddRef()
{
  return InterlockedIncrement(&m_lRefCount);
}

HRESULT csMenu::QueryInterface(const IID& riid, void **ppvObject)
{
  static const QITAB qit[] = {
    QITABENT(csMenu, IShellExtInit),
    QITABENT(csMenu, IContextMenu),
    { 0 }
  };
  return QISearch(this, qit, riid, ppvObject);
}

ULONG csMenu::Release()
{
  const LONG lRet = InterlockedDecrement(&m_lRefCount);
  if( m_lRefCount == 0 ) {
    delete this;
  }
  return lRet;
}

////// IShellExtInit /////////////////////////////////////////////////////////

HRESULT csMenu::Initialize(LPCITEMIDLIST /*pidlFolder*/, IDataObject *pdtobj,
                           HKEY /*hkeyProgID*/)
{
  FORMATETC etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  STGMEDIUM stg = { TYMED_HGLOBAL };

  if( FAILED(pdtobj->GetData(&etc, &stg)) ) {
    return E_INVALIDARG;
  }

  HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
  if( hDrop == NULL ) {
    ReleaseStgMedium(&stg);
    return E_INVALIDARG;
  }

  _files.clear();

  UINT   sizeFN   = 0;
  WCHAR *filename = 0;

  const UINT uNumFiles = DragQueryFileW(hDrop, UINT_MAX, NULL, 0);
  for(UINT i = 0; i < uNumFiles; i++) {
    const UINT reqFN = DragQueryFileW(hDrop, i, NULL, 0);
    if( reqFN < 1 ) {
      continue;
    }

    if( reqFN+1 > sizeFN ) {
      sizeFN = 0;
      csFree(filename);
      filename = (WCHAR*)csAlloc((reqFN+1)*sizeof(WCHAR));
      if( filename == 0 ) {
        continue;
      }
      sizeFN = reqFN+1;
    }

    if( DragQueryFileW(hDrop, i, filename, sizeFN) == 0 ) {
      continue;
    }

    _files += filename;
  }

  csFree(filename);

  GlobalUnlock(stg.hGlobal);
  ReleaseStgMedium(&stg);

  return _files.count() > 0  ?  S_OK : E_INVALIDARG;
}

////// IContextMenu //////////////////////////////////////////////////////////

HRESULT csMenu::GetCommandString(UINT_PTR /*idCmd*/, UINT /*uType*/,
                                 UINT * /*pReserved*/,
                                 CHAR * /*pszName*/, UINT /*cchMax*/)
{
  return E_NOTIMPL;
}

HRESULT csMenu::InvokeCommand(CMINVOKECOMMANDINFO *pici)
{
  if( IS_INTRESOURCE(pici->lpVerb) == FALSE ) {
    return E_INVALIDARG;
  }

  const UINT cmd = (ULONG_PTR)pici->lpVerb;
  return executeCommand(cmd, _files)  ?  S_OK : E_INVALIDARG;
}

HRESULT csMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst,
                                 UINT /*idCmdLast*/, UINT uFlags)
{
  if( uFlags & CMF_DEFAULTONLY ) {
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
  }

  UINT uCmdID = idCmdFirst;

  const DWORD flags = regReadFlags();

  HMENU submenu = CreatePopupMenu();
  insertMenuItem(submenu, 0, uCmdID++, L"List");
  insertMenuItem(submenu, 1, uCmdID++, L"List (path)");
  insertMenuItem(submenu, 2, uCmdID++, L"List (path, tabular)");
  insertSeparatorMenuItem(submenu, 3);
  insertCheckableMenuItem(submenu, 4, uCmdID++, L"Batch processing",
                          testFlags(flags, CMD_FLAG_BATCH));
  insertCheckableMenuItem(submenu, 5, uCmdID++, L"Resolve UNC paths",
                          testFlags(flags, CMD_FLAG_UNC));
  insertCheckableMenuItem(submenu, 6, uCmdID++, L"UN*X path separators",
                          testFlags(flags, CMD_FLAG_UNIX));

  insertSubMenuItem(hmenu, submenu, indexMenu, uCmdID++, L"CS::Menu");

  return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uCmdID-idCmdFirst);
}
