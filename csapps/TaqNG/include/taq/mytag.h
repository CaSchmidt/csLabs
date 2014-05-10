/****************************************************************************
** Copyright (c) 2005-2014, Carsten Schmidt. All rights reserved.
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

#ifndef __MYTAG_H__
#define __MYTAG_H__

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QString>

struct MyTag {
  QString name;
  QRegExp regexp;
  int cap_title,   cap_album, cap_artist, cap_genre,    cap_year,
      cap_comment, cap_track, cap_disc,   cap_composer, cap_conductor;
  QString str_title,   str_album, str_artist, str_genre,    str_year,
          str_comment, str_track, str_disc,   str_composer, str_conductor;

  MyTag()
    : name(), regexp(),
      cap_title(), cap_album(), cap_artist(), cap_genre(), cap_year(),
      cap_comment(), cap_track(), cap_disc(), cap_composer(), cap_conductor(),
      str_title(), str_album(), str_artist(), str_genre(), str_year(),
      str_comment(), str_track(), str_disc(), str_composer(), str_conductor()
  {
  }
};

class MyTagModel : public QAbstractListModel {
  Q_OBJECT
public:
  MyTagModel(const QList<MyTag>& expressions, QObject *parent = 0)
    : QAbstractListModel(parent), _expressions(expressions)
  {
  }

  ~MyTagModel()
  {
  }

  void requery()
  {
    beginResetModel();
    endResetModel();
  }

  int rowCount(const QModelIndex& parent = QModelIndex()) const
  {
    return _expressions.count();
  }

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
  {
    if( !index.isValid() )
      return QVariant();

    if( role != Qt::DisplayRole )
      return QVariant();

    if( index.row() >= _expressions.count() )
      return QVariant();

    return _expressions[index.row()].name;
  }

private:
  const QList<MyTag>& _expressions;
}; // class MyTagModel

#endif // __MYTAG_H__
