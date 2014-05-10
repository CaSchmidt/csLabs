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

#include "taq/mp3filesmodel.h"

#include "taq/wmainwindow.h"

////// public //////////////////////////////////////////////////////////////////

Mp3FilesModel::Mp3FilesModel(const QStringList& strings, const bool validate,
			     QObject *parent)
  : QAbstractListModel(parent), _files(), _validate(validate)
{
  setStringList(strings);
}

Mp3FilesModel::~Mp3FilesModel()
{
}

QStringList Mp3FilesModel::stringList() const
{
  QStringList list;

  foreach(Mp3File f, _files)
    list.push_back(f.filename);

  return list;
}

void Mp3FilesModel::setStringList(const QStringList& list)
{
  removeRows(0, rowCount());
  insertRows(0, list.size());

  for(int i = 0; i < _files.size(); i++)
    _files[i] = Mp3File(list.at(i), true);

  emit dataChanged(index(0), index(rowCount()-1));
}

QStringList Mp3FilesModel::activeFiles() const
{
  QStringList list;

  foreach(Mp3File f, _files)
    if( f.active )
      list.push_back(f.filename);

  return list;
}

QVariant Mp3FilesModel::data(const QModelIndex& index, int role) const
{
  if( index.isValid()  &&  role == Qt::DisplayRole ) {
    return QVariant(_files.at(index.row()).filename);
  } else if( index.isValid()  &&  role == Qt::CheckStateRole ) {
    const int i =
        _files.at(index.row()).active  ?  Qt::Checked  :  Qt::Unchecked;

    return QVariant(i);
  } else if( index.isValid() && role == Qt::ForegroundRole  &&  _validate ) {
    WMainWindow *m = dynamic_cast<WMainWindow*>(QObject::parent());
    if( m != 0 ) {
      const QRegExp rx = m->currentTag().regexp;
      if( rx.isEmpty()  ||  !rx.isValid()  ||
          rx.indexIn(_files.at(index.row()).filename) < 0 ) {
        const QVariant var = QColor(Qt::red);
        return var;
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags Mp3FilesModel::flags(const QModelIndex& /*index*/) const
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

bool Mp3FilesModel::insertRows(int row, int count, const QModelIndex& /*parent*/)
{
  if( row < 0  ||  row > rowCount()  ||  count <= 0 ) {
    return false;
  }

  beginInsertRows(QModelIndex(), row, row + count - 1);
  for(int r = 0; r < count; r++) {
    _files.insert(row, Mp3File());
  }
  endInsertRows();

  return true;
}

bool Mp3FilesModel::removeRows(int row, int count, const QModelIndex& /*parent*/)
{
  const int from = row;
  const int   to = row + count - 1;
  if( from < 0  ||  from >= rowCount()  ||  to < 0  ||  to >= rowCount() ) {
    return false;
  }

  beginRemoveRows(QModelIndex(), from, to);
  for(int r = 0; r < count; r++) {
    _files.removeAt(row);
  }
  endRemoveRows();

  return true;
}

int Mp3FilesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _files.size();
}

bool Mp3FilesModel::setData(const QModelIndex& index, const QVariant& value,
			    int role)
{
  if( index.isValid()  &&  role == Qt::EditRole )
    {
      _files[index.row()].filename = value.toString();
      emit dataChanged(index, index);

      return true;
    }
  else if( index.isValid()  &&  role == Qt::CheckStateRole )
    {
      _files[index.row()].active = value.toInt() == Qt::Checked;
      emit dataChanged(index, index);

      return true;
    }

  return false;
}
