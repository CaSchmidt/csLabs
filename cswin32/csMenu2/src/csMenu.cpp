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

#include "csMenu.h"

#include "command.h"
#include "menu.h"
#include "reg.h"
#include "resource.h"
#include "util.hpp"

////// Global ////////////////////////////////////////////////////////////////

extern HINSTANCE g_hDllInst;
extern LONG      g_lDllRefCount;

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
  , _menuBitmap(nullptr)
{
  InterlockedIncrement(&g_lDllRefCount);

  const int iconWidth  = GetSystemMetrics(SM_CXMENUCHECK);
  const int iconHeight = GetSystemMetrics(SM_CYMENUCHECK);
  HICON icon = (HICON)LoadImageW(g_hDllInst,
                                 MAKEINTRESOURCEW(IDI_csMenu), IMAGE_ICON,
                                 iconWidth, iconHeight, 0);
  if( icon != nullptr ) {
    _menuBitmap = createBitmapFromIcon(icon, iconWidth, iconHeight);
    DestroyIcon(icon);
  }
}

csMenu::~csMenu()
{
  if( _menuBitmap != nullptr ) {
    DeleteObject(_menuBitmap);
  }
  InterlockedDecrement(&g_lDllRefCount);
}

////// IUnknown //////////////////////////////////////////////////////////////

ULONG csMenu::AddRef()
{
  return static_cast<ULONG>(InterlockedIncrement(&m_lRefCount));
}

HRESULT csMenu::QueryInterface(const IID& riid, void **ppvObject)
{
  static const QITAB qit[] = {
    QITABENT(csMenu, IShellExtInit),
    QITABENT(csMenu, IContextMenu),
    { nullptr, 0 }
  };
  return QISearch(this, qit, riid, ppvObject);
}

ULONG csMenu::Release()
{
  const LONG lRet = InterlockedDecrement(&m_lRefCount);
  if( m_lRefCount == 0 ) {
    delete this;
  }
  return static_cast<ULONG>(lRet);
}

////// IShellExtInit /////////////////////////////////////////////////////////

HRESULT csMenu::Initialize(LPCITEMIDLIST /*pidlFolder*/, IDataObject *pdtobj,
                           HKEY /*hkeyProgID*/)
{
  FORMATETC etc = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
  STGMEDIUM stg = { TYMED_HGLOBAL, {nullptr}, nullptr };

  if( FAILED(pdtobj->GetData(&etc, &stg)) ) {
    return E_INVALIDARG;
  }

  HDROP hDrop = reinterpret_cast<HDROP>(GlobalLock(stg.hGlobal));
  if( hDrop == nullptr ) {
    ReleaseStgMedium(&stg);
    return E_INVALIDARG;
  }

  _files.clear();

  csWString filename;
  const UINT uNumFiles = DragQueryFileW(hDrop, UINT_MAX, nullptr, 0);
  for(UINT i = 0; i < uNumFiles; i++) {
    const UINT reqFN = DragQueryFileW(hDrop, i, nullptr, 0);
    if( reqFN < 1 ) {
      continue;
    }

    try {
      filename.resize(reqFN, 0);
    } catch(...) {
      filename.clear();
      continue;
    }

    if( DragQueryFileW(hDrop, i, const_cast<wchar_t*>(filename.data()),
                       static_cast<UINT>(filename.size() + 1)) == 0 ) {
      continue;
    }

    _files.push_back(std::move(filename));
  }

  GlobalUnlock(stg.hGlobal);
  ReleaseStgMedium(&stg);

  _files.sort();

  return _files.size() > 0  ?  S_OK : E_INVALIDARG;
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

  UINT uPos = 0;
  UINT uCmdID = idCmdFirst;

  const DWORD flags = regReadFlags();

  const DWORD parallelCount = regReadParallelCount();
  const bool hasParallel    = parallelCount > 1;

  HMENU submenu = CreatePopupMenu();
  insertMenuItem(submenu, uPos++, uCmdID++, L"List", !_files.empty());
  insertMenuItem(submenu, uPos++, uCmdID++, L"List (path)", !_files.empty());
  insertMenuItem(submenu, uPos++, uCmdID++, L"List (path, tabular)", !_files.empty());
  insertSeparatorMenuItem(submenu, uPos++);
  insertCheckableMenuItem(submenu, uPos++, uCmdID++, L"Batch processing",
                          testFlags(flags, CMD_FLAG_BATCH));
  insertCheckableMenuItem(submenu, uPos++, uCmdID++, L"Parallel execution",
                          testFlags(flags, CMD_FLAG_PARALLEL), hasParallel);
  insertCheckableMenuItem(submenu, uPos++, uCmdID++, L"Resolve UNC paths",
                          testFlags(flags, CMD_FLAG_UNC));
  insertCheckableMenuItem(submenu, uPos++, uCmdID++, L"UN*X path separators",
                          testFlags(flags, CMD_FLAG_UNIX));

  insertSubMenuItem(hmenu, submenu, indexMenu, uCmdID++, L"CS::Menu", _menuBitmap);

  const csWStringList scripts = regReadScripts();
  if( !scripts.empty()  &&  !_files.empty() ) {
    insertSeparatorMenuItem(submenu, uPos++);
    for(csWStringList::const_iterator it = scripts.begin(); it != scripts.end(); it++) {
      insertMenuItem(submenu, uPos++, uCmdID++, it->data());
    }
  }

  return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uCmdID-idCmdFirst);
}
