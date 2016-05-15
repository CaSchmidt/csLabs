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

#include "SimCore/SimModulesModel.h"

#include "SimCore/SimContext.h"

////// public ////////////////////////////////////////////////////////////////

SimModulesModel::SimModulesModel(SimContext *simctx, QObject *parent)
  : QAbstractListModel(parent)
  , _names()
  , _simctx(simctx)
{
  connect(&_simctx->env, &SimEnvironment::modulesCleared,
          this, &SimModulesModel::clearModules);
  connect(&_simctx->env, &SimEnvironment::moduleInserted,
          this, &SimModulesModel::insertModule);
  connect(&_simctx->env, &SimEnvironment::moduleRemoved,
          this, &SimModulesModel::removeModule);
}

SimModulesModel::~SimModulesModel()
{
}

QVariant SimModulesModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  if(        role == Qt::DisplayRole ) {
    return _names.at(index.row());
  } else if( role == Qt::CheckStateRole ) {
    return _simctx->env.isModuleActive(_names.at(index.row()))
        ? Qt::Checked
        : Qt::Unchecked;
  }
  return QVariant();
}

Qt::ItemFlags SimModulesModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return 0;
  }
  Qt::ItemFlags f = QAbstractListModel::flags(index);
  f |= Qt::ItemIsUserCheckable;
  return f;
}

int SimModulesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _names.size();
}

bool SimModulesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if( !index.isValid() ) {
    return false;
  }
  if( role == Qt::CheckStateRole ) {
    _simctx->env.setModuleActive(_names.at(index.row()),
                                 value.toInt() != Qt::Unchecked);
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

////// private slots /////////////////////////////////////////////////////////

void SimModulesModel::clearModules()
{
  beginResetModel();
  _names.clear();
  endResetModel();
}

void SimModulesModel::insertModule(const QString& name)
{
  if(_names.contains(name) ) {
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

void SimModulesModel::removeModule(const QString& name)
{
  const int index = _names.indexOf(name);
  if( index < 0 ) {
    return;
  }

  beginRemoveRows(QModelIndex(), index, index);
  _names.removeAt(index);
  endRemoveRows();
}
