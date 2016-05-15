/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QMimeData>

#include "SimCore/SimVariablesModel.h"

#include "SimCore/SimContext.h"

////// Macros ////////////////////////////////////////////////////////////////

#define COLUMN_NAME   0
#define COLUMN_Count  1

////// public ////////////////////////////////////////////////////////////////

SimVariablesModel::SimVariablesModel(SimContext *simctx, QObject *parent)
  : QAbstractListModel(parent)
  , _names()
{
  connect(&simctx->env, &SimEnvironment::variablesCleared,
          this, &SimVariablesModel::clearVariables);
  connect(&simctx->env, &SimEnvironment::variableInserted,
          this, &SimVariablesModel::insertVariable);
  connect(&simctx->env, &SimEnvironment::variableRemoved,
          this, &SimVariablesModel::removeVariable);
}

SimVariablesModel::~SimVariablesModel()
{
}

QVariant SimVariablesModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  if( role == Qt::DisplayRole  &&  index.column() == COLUMN_NAME ) {
    return _names.at(index.row());
  }
  return QVariant();
}

Qt::ItemFlags SimVariablesModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags f = QAbstractListModel::flags(index);
  f |= Qt::ItemIsDragEnabled;
  return f;
}

QMimeData *SimVariablesModel::mimeData(const QModelIndexList& indexes) const
{
  if( indexes.isEmpty() ) {
    return 0;
  }

  QStringList list;
  foreach(const QModelIndex& index, indexes) {
    if( index.column() != COLUMN_NAME ) {
      continue;
    }
    list.push_back(_names.at(index.row()));
  }
  if( list.isEmpty() ) {
    return 0;
  }

  QByteArray encoded = list.join(QChar::fromLatin1('\n')).toUtf8();
  QMimeData *data = new QMimeData();
  data->setData(QStringLiteral(SIM_MIME_VARIABLES), encoded);
  return data;
}

QStringList SimVariablesModel::mimeTypes() const
{
  return QStringList(QStringLiteral(SIM_MIME_VARIABLES));
}

int SimVariablesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _names.size();
}

////// private slots /////////////////////////////////////////////////////////

void SimVariablesModel::clearVariables()
{
  beginResetModel();
  _names.clear();
  endResetModel();
}

void SimVariablesModel::insertVariable(const QString& name)
{
  if( _names.contains(name) ) {
    return;
  }

  int insertAt = 0;
  while( insertAt < _names.size()  &&  name > _names.at(insertAt) ) {
    insertAt++;
  }

  beginInsertRows(QModelIndex(), insertAt, insertAt);
  _names.insert(insertAt, name);
  endInsertRows();
}

void SimVariablesModel::removeVariable(const QString& name)
{
  const int index = _names.indexOf(name);
  if( index < 0 ) {
    return;
  }

  beginRemoveRows(QModelIndex(), index, index);
  _names.removeAt(index);
  endRemoveRows();
}
