/****************************************************************************
** Copyright (c) 2016, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QMenu>

#include "csQt/csMultiToolButton.h"

////// public ////////////////////////////////////////////////////////////////

csMultiToolButton::csMultiToolButton(const csMultiItems& items, const int invalidId,
                                     QWidget *parent)
  : QToolButton(parent)
  , _actions()
  , _currentSelection(invalidId)
  , _invalidId(invalidId)
{
  setAutoRaise(true);
  setPopupMode(QToolButton::InstantPopup);
  setToolButtonStyle(Qt::ToolButtonTextOnly);

  QAction *defaultAction(0);

  QMenu *menu = new QMenu(this);
  foreach (const csMultiItem& item, items) {
    QAction *action = menu->addAction(item.first);
    action->setData(item.second);
    connect(action, &QAction::triggered, this, &csMultiToolButton::activateSelection);
    if( defaultAction == 0 ) {
      defaultAction = action;
    }
    _actions.push_back(action);
  }
  setMenu(menu);

  if( defaultAction != 0 ) {
    defaultAction->trigger();
  }
}

csMultiToolButton::~csMultiToolButton()
{
}

int csMultiToolButton::currentSelection()
{
  return _currentSelection;
}

////// public slots //////////////////////////////////////////////////////////

void csMultiToolButton::setCurrentSelection(int id)
{
  _currentSelection = _invalidId;
  foreach(QAction *action, _actions) {
    if( action->data().toInt() == id ) {
      setText(action->text());
      _currentSelection = action->data().toInt();
      return;
    }
  }
}

////// private slots /////////////////////////////////////////////////////////

void csMultiToolButton::activateSelection()
{
  QAction *action = dynamic_cast<QAction*>(sender());
  if( action == 0 ) {
    _currentSelection = _invalidId;
    return;
  }
  setText(action->text());
  _currentSelection = action->data().toInt();
  emit selectionChanged(_currentSelection);
}
