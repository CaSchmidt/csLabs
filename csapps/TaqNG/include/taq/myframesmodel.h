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

#ifndef __MYFRAMESMODEL_H__
#define __MYFRAMESMODEL_H__

#include <QtCore/QAbstractTableModel>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

struct MyFrame {
  QString id, frame, content;
};

class MyFramesModel : public QAbstractTableModel {
  Q_OBJECT
public:
  MyFramesModel(const QVector<MyFrame>& myframes,
		QObject *parent = 0)
    : QAbstractTableModel(parent), _myframes(myframes)
  {
  }

  ~MyFramesModel()
  {
  }

  int rowCount(const QModelIndex& parent = QModelIndex()) const
  {
    return _myframes.count();
  }

  int columnCount(const QModelIndex& parent = QModelIndex()) const
  {
    return 3;
  }

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
  {
    if( !index.isValid() )
      return QVariant();

    if( index.row() >= _myframes.count() )
      return QVariant();

    if( index.column() >= 3 )
      return QVariant();

    if( role != Qt::DisplayRole )
      return QVariant();

    if( index.column() == 0 )
      return _myframes[index.row()].id;
    else if( index.column() == 1 )
      return _myframes[index.row()].frame;
    else if( index.column() == 2 )
      return _myframes[index.row()].content;

    return QVariant();
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    if( role != Qt::DisplayRole )
      return QVariant();

    if( orientation == Qt::Horizontal )
      {
	if( section == 0 )
	  return QString(tr("ID"));
	else if( section == 1 )
	  return QString(tr("Frame"));
	else if( section == 2 )
	  return QString(tr("Content"));
      }
    else
      return QString::number(section+1);

    return QVariant();
  }

  void requery()
  {
    beginResetModel();
    endResetModel();
  }

private:
  const QVector<MyFrame>& _myframes;
}; // class MyFramesModel

#endif // __MYFRAMESMODEL_H__
