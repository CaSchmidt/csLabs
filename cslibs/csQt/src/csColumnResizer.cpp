/****************************************************************************
** Copyright (c) 2013-2014, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QHeaderView>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>

#include "csQt/csColumnResizer.h"

////// public ////////////////////////////////////////////////////////////////

csColumnResizer::csColumnResizer(QObject *parent)
  : QObject(parent)
{
}

csColumnResizer::~csColumnResizer()
{
}

bool csColumnResizer::eventFilter(QObject *watched, QEvent *event)
{
  if( event->type() == QKeyEvent::KeyPress ) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    if( keyEvent->modifiers() == Qt::NoModifier  &&
        keyEvent->key() == Qt::Key_F9 ) {
      QHeaderView *headerView(0);

      QTableView *tableView = dynamic_cast<QTableView*>(watched);
      if( tableView != 0 ) {
        headerView = tableView->horizontalHeader();
      }

      QTreeView *treeView = dynamic_cast<QTreeView*>(watched);
      if( treeView != 0 ) {
        headerView = treeView->header();
      }

      if( headerView != 0  &&  headerView->isVisible() ) {
        headerView->resizeSections(QHeaderView::ResizeToContents);
        return true;
      }
    }
  }

  return QObject::eventFilter(watched, event);
}
