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

#ifndef CSMENU_H
#define CSMENU_H

#include <Windows.h>
#include <ShObjIdl.h>

#include "csCore2/csWString.h"

class csMenu : public IShellExtInit, public IContextMenu {
public:
  csMenu();
  virtual ~csMenu();

  // IUnknown

  ULONG AddRef();
  HRESULT QueryInterface(const IID &riid, void **ppvObject);
  ULONG Release();

  // IShellExtInit

  HRESULT Initialize(LPCITEMIDLIST pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID);

  // IContextMenu

  HRESULT GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pReserved,
                           CHAR *pszName, UINT cchMax);
  HRESULT InvokeCommand(CMINVOKECOMMANDINFO *pici);
  HRESULT QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst,
                           UINT idCmdLast, UINT uFlags);

private:
  LONG m_lRefCount;
  csWStringList _files;
  HBITMAP _menuBitmap;
};

class __declspec(uuid("{1A2306A9-DC34-46F0-84CA-7FEE7387CA37}")) csMenu;

extern const GUID GUID_csMenu;

#endif // CSMENU_H
