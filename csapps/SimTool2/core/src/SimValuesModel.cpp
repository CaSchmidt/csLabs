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

#include <QtCore/QTimerEvent>

#include "SimCore/SimValuesModel.h"

#include "SimCore/Sim.h"
#include "SimCore/SimContext.h"

////// Macros ////////////////////////////////////////////////////////////////

#define COLUMN_NAME   0
#define COLUMN_VALUE  1
#define COLUMN_UNIT   2
#define COLUMN_Count  3

#define CONFIG_REFRESH_RATE  200

////// public ////////////////////////////////////////////////////////////////

SimValuesModel::SimValuesModel(SimContext *simctx, QObject *parent)
  : QAbstractTableModel(parent)
  , _names()
  , _refreshTimer()
  , _refreshTimerId(0)
  , _simctx(simctx)
  , _values()
{
  connect(&_simctx->ctrl, &SimControl::stateEntered,
          this, &SimValuesModel::enterState);
  connect(&_simctx->ctrl, &SimControl::stateExited,
          this, &SimValuesModel::exitState);
  connect(&simctx->env, &SimEnvironment::variablesCleared,
          this, &SimValuesModel::clearVariables);
  connect(&simctx->env, &SimEnvironment::variableRemoved,
          this, &SimValuesModel::removeVariable);

  // Default State ///////////////////////////////////////////////////////////

  enterState(_simctx->sim.activeState());
}

SimValuesModel::~SimValuesModel()
{
}

bool SimValuesModel::addValue(const QString& name)
{
  if( _values.contains(name) ) {
    return true;
  }

  SimValueRef value = _simctx->db.value(name);
  if( value.isNull() ) {
    return false;
  }

  value->get();

  int insertAt = 0;
  while( insertAt < _names.size()  &&  name > _names.at(insertAt) ) {
    insertAt++;
  }

  beginInsertRows(QModelIndex(), insertAt, insertAt);
  _names.insert(insertAt, name);
  _values.insert(name, value);
  endInsertRows();

  return true;
}

int SimValuesModel::columnCount(const QModelIndex& /*parent*/) const
{
  return COLUMN_Count;
}

QVariant SimValuesModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  if(        role == Qt::DisplayRole ) {
    const SimValueRef value = _values[_names.at(index.row())];
    if(        index.column() == COLUMN_NAME ) {
      return value->name();
    } else if( index.column() == COLUMN_VALUE ) {
      return value->display();
    } else if( index.column() == COLUMN_UNIT ) {
      const SimVariable var = _simctx->env.variable(_names.at(index.row()));
      return var.unit();
    }
  } else if( role == Qt::EditRole  &&  index.column() == COLUMN_VALUE ) {
    const SimValueRef value = _values[_names.at(index.row())];
    return value->value();
  } else if( role == GetValuePrecision ) {
    const SimValueRef value = _values[_names.at(index.row())];
    return value->precision();
  } else if( role == GetMinValue ) {
    const SimValueRef value = _values[_names.at(index.row())];
    return value->minValue();
  } else if( role ==GetMaxValue ) {
    const SimValueRef value = _values[_names.at(index.row())];
    return value->maxValue();
  }
  return QVariant();
}

Qt::ItemFlags SimValuesModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  if( index.column() == COLUMN_VALUE ) {
    f |= Qt::ItemIsEditable;
  }
  return f;
}

QVariant SimValuesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( orientation == Qt::Horizontal  &&  role == Qt::DisplayRole ) {
    if(        section == COLUMN_NAME ) {
      return tr("Name");
    } else if( section == COLUMN_VALUE ) {
      return tr("Value");
    } else if( section == COLUMN_UNIT ) {
      return tr("Unit");
    }
  }
  return QVariant();
}

int SimValuesModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _names.size();
}

bool SimValuesModel::setData(const QModelIndex& idx, const QVariant& data, int role)
{
  if( !idx.isValid() ) {
    return false;
  }
  if( role == Qt::EditRole  &&  idx.column() == COLUMN_VALUE ) {
    SimValueRef value = _values[_names.at(idx.row())];
    value->setValue(data.toDouble());
    value->set();
    emit dataChanged(idx, idx);
    return true;
  } else if( role == SetValuePrecision ) {
    SimValueRef value = _values[_names.at(idx.row())];
    value->setPrecision(data.toInt());
    const QModelIndex update = index(idx.row(), COLUMN_VALUE);
    emit dataChanged(update, update);
    return true;
  }
  return false;
}

////// public slots //////////////////////////////////////////////////////////

void SimValuesModel::enterState(int state)
{
  if( state == StepState ) {
    _refreshTimer.start(CONFIG_REFRESH_RATE, this);
    _refreshTimerId = _refreshTimer.timerId();
  }
}

void SimValuesModel::exitState(int state)
{
  if(        state == InitState ) {
    updateValues();
  } else if( state == StartState ) {
    updateValues();
  } else if( state == StepState ) {
    _refreshTimer.stop();
    _refreshTimerId = 0;
    updateValues();
  } else if( state == StopState ) {
    updateValues();
  }
}

void SimValuesModel::clearVariables()
{
  beginResetModel();
  _names.clear();
  _values.clear();
  endResetModel();
}

void SimValuesModel::removeVariable(const QString& name)
{
  const int index = _names.indexOf(name);
  if( index < 0 ) {
    return;
  }

  beginRemoveRows(QModelIndex(), index, index);
  _names.removeAt(index);
  _values.remove(name);
  endRemoveRows();
}

////// protected /////////////////////////////////////////////////////////////

void SimValuesModel::timerEvent(QTimerEvent *event)
{
  if( event->timerId() == _refreshTimerId ) {
    updateValues();
  }
}

////// private ///////////////////////////////////////////////////////////////

void SimValuesModel::updateValues()
{
  const int rows = rowCount();
  if( rows < 1 ) {
    return;
  }
  _simctx->db.lock();
  foreach(const SimValueRef& value, _values.values()) {
    value->getAsync();
  }
  _simctx->db.unlock();
  const QModelIndex from = index(0, 1);
  const QModelIndex   to = index(rows-1, 1);
  emit dataChanged(from, to);
}
