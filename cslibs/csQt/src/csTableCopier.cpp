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

#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QItemSelectionModel>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>
#include <QtWidgets/QTableView>

#include "csQt/csTableCopier.h"

#include "csQt/csQtUtil.h"

////// public ////////////////////////////////////////////////////////////////

csTableCopier::csTableCopier(QObject *parent)
  : QObject(parent)
  , extractionRole(Qt::DisplayRole)
  , extractionFlags(cs::ExtractHHeader | cs::ExtractVHeader)
{
}

csTableCopier::~csTableCopier()
{
}

bool csTableCopier::eventFilter(QObject *watched, QEvent *event)
{
  if( event->type() == QEvent::KeyPress ) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    QTableView *tableView = dynamic_cast<QTableView*>(watched);
    if( keyEvent == QKeySequence::Copy  &&  tableView != 0 ) {
      const QModelIndex topLeft     = topLeftSelection(tableView);
      const QModelIndex bottomRight = bottomRightSelection(tableView);

      if( topLeft.isValid()  &&  bottomRight.isValid() ) {
        const QString text = csTableToString(tableView,
                                             topLeft, bottomRight,
                                             extractionRole, extractionFlags);
        QApplication::clipboard()->setText(text);

        return true;
      }
    }
  }

  return QObject::eventFilter(watched, event);
}

////// private ///////////////////////////////////////////////////////////////

QModelIndex csTableCopier::topLeftSelection(QTableView *tableView)
{
  const QModelIndexList selection =
      tableView->selectionModel()->selection().indexes();
  if( selection.isEmpty() ) {
    return QModelIndex();
  }

  QModelIndex topLeft(selection.front());
  foreach(const QModelIndex index, selection) {
    if( index.column() < topLeft.column()  ||
        index.row()    < topLeft.row() ) {
      topLeft = index;
    }
  }

  return topLeft;
}

QModelIndex csTableCopier::bottomRightSelection(QTableView *tableView)
{
  const QModelIndexList selection =
      tableView->selectionModel()->selection().indexes();
  if( selection.isEmpty() ) {
    return QModelIndex();
  }

  QModelIndex bottomRight(selection.front());
  foreach(const QModelIndex index, selection) {
    if( index.column() > bottomRight.column()  ||
        index.row()    > bottomRight.row() ) {
      bottomRight = index;
    }
  }

  return bottomRight;
}
