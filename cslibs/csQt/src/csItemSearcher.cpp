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

#include <QtCore/QAbstractItemModel>
#include <QtWidgets/QAbstractItemView>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>

#include "csQt/csItemSearcher.h"

////// public ////////////////////////////////////////////////////////////////

csItemSearcher::csItemSearcher(QObject *parent)
  : QObject(parent)
  , matchColumn(0)
  , matchRole(Qt::DisplayRole)
  , matchRow(0)
  , matchString()
{
}

csItemSearcher::~csItemSearcher()
{
}

bool csItemSearcher::eventFilter(QObject *watched, QEvent *event)
{
  QAbstractItemView *view = dynamic_cast<QAbstractItemView*>(watched);
  if( event->type() == QEvent::KeyPress  &&  view != 0 ) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);

    if( (Qt::Key_A <= keyEvent->key() && keyEvent->key() <= Qt::Key_Z) ||
        (Qt::Key_0 <= keyEvent->key() && keyEvent->key() <= Qt::Key_9) ||
        keyEvent->key() == Qt::Key_BracketLeft  ||
        keyEvent->key() == Qt::Key_BracketRight ||
        keyEvent->key() == Qt::Key_Period       ||
        keyEvent->key() == Qt::Key_Underscore ) {
      return match(view, matchString+keyEvent->text(), matchRow);
    } else if( keyEvent->modifiers() == Qt::NoModifier  &&
               keyEvent->key() == Qt::Key_Backspace ) {
      matchRow = 0;
      matchString.chop(1);

      emit matched(matchString);

      return true;
    } else if( keyEvent->modifiers() == Qt::NoModifier  &&
               keyEvent->key() == Qt::Key_Escape ) {
      clear();
      return true;
    } else if( keyEvent == QKeySequence::FindNext ) {
      return match(view, matchString, matchRow+1);
    }
  }

  return QObject::eventFilter(watched, event);
}

////// public slots //////////////////////////////////////////////////////////

void csItemSearcher::clear()
{
  matchRow = 0;
  matchString.clear();

  emit matched(matchString);
}

void csItemSearcher::setColumn(int column)
{
  matchColumn = column;
  clear();
}

////// private ///////////////////////////////////////////////////////////////

bool csItemSearcher::match(QAbstractItemView *view, const QString& string, const int row)
{
  QAbstractItemModel *model = view->model();
  const QModelIndex start = model->index(row, matchColumn);
  const QModelIndexList hits = model->match(start, matchRole, string, 1,
                                            Qt::MatchFlags(Qt::MatchStartsWith));
  if( hits.size() != 1 ) {
    return false;
  }

  view->scrollTo(hits.front(), QAbstractItemView::PositionAtTop);
  view->selectionModel()->setCurrentIndex(hits.front(), QItemSelectionModel::NoUpdate);

  matchRow    = hits.front().row();
  matchString = string;

  emit matched(matchString);

  return true;
}
