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

#include "menu.h"

void insertMenuItem(HMENU menu,
                    const UINT pos, const UINT id, const wchar_t *text,
                    const bool enabled)
{
  MENUITEMINFOW mii;
  SecureZeroMemory(&mii, sizeof(MENUITEMINFOW));

  mii.cbSize     = sizeof(MENUITEMINFOW);
  mii.fMask      = MIIM_ID | MIIM_STRING;
  mii.wID        = id;
  mii.dwTypeData = const_cast<LPWSTR>(text);

  if( !enabled ) {
    mii.fMask  |= MIIM_STATE;
    mii.fState |= MFS_DISABLED;
  }

  InsertMenuItemW(menu, pos, TRUE, &mii);
}

void insertCheckableMenuItem(HMENU menu,
                             const UINT pos, const UINT id, const wchar_t *text,
                             const bool state, const bool enabled)
{
  MENUITEMINFOW mii;
  SecureZeroMemory(&mii, sizeof(MENUITEMINFOW));

  mii.cbSize     = sizeof(MENUITEMINFOW);
  mii.fMask      = MIIM_CHECKMARKS | MIIM_ID | MIIM_STATE | MIIM_STRING;
  mii.wID        = id;
  mii.fState     = state  ?  MFS_CHECKED : MFS_UNCHECKED;
  mii.dwTypeData = const_cast<LPWSTR>(text);

  if( !enabled ) {
    mii.fMask  |= MIIM_STATE;
    mii.fState |= MFS_DISABLED;
  }

  InsertMenuItemW(menu, pos, TRUE, &mii);
}

void insertSeparatorMenuItem(HMENU menu,
                             const UINT pos)
{
  MENUITEMINFOW mii;
  SecureZeroMemory(&mii, sizeof(MENUITEMINFOW));

  mii.cbSize = sizeof(MENUITEMINFOW);
  mii.fMask  = MIIM_FTYPE;
  mii.fType  = MFT_SEPARATOR;

  InsertMenuItemW(menu, pos, TRUE, &mii);
}

void insertSubMenuItem(HMENU menu, HMENU submenu,
                       const UINT pos, const UINT id, const wchar_t *text,
                       const HBITMAP bitmap)
{
  MENUITEMINFOW mii;
  SecureZeroMemory(&mii, sizeof(MENUITEMINFOW));

  mii.cbSize     = sizeof(MENUITEMINFOW);
  mii.fMask      = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
  mii.wID        = id;
  mii.dwTypeData = const_cast<LPWSTR>(text);
  mii.hSubMenu   = submenu;

  InsertMenuItemW(menu, pos, TRUE, &mii);

  if( bitmap != nullptr ) {
    SetMenuItemBitmaps(menu, pos, MF_BYPOSITION, bitmap, nullptr);
  }
}
