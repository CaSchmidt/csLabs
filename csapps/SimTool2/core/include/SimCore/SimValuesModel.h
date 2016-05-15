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

#ifndef __SIMVALUESMODEL_H__
#define __SIMVALUESMODEL_H__

#include <QtCore/QAbstractTableModel>
#include <QtCore/QBasicTimer>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include <SimCore/simcore_config.h>
#include <SimCore/ISimValue.h>

class SimContext;

class SIM_CORE_EXPORT SimValuesModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum DataRole {
    GetValuePrecision = Qt::UserRole+1,
    SetValuePrecision,
    GetMinValue,
    GetMaxValue
  };

  SimValuesModel(SimContext *simctx, QObject *parent);
  ~SimValuesModel();

  bool addValue(const QString& name);

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  bool setData(const QModelIndex& idx, const QVariant& data, int role);

public slots:
  void enterState(int state);
  void exitState(int state);
  void clearVariables();
  void removeVariable(const QString& name);

protected:
  void timerEvent(QTimerEvent *event);

private:
  void updateValues();

  QStringList _names;
  QBasicTimer _refreshTimer;
  int _refreshTimerId;
  SimContext *_simctx;
  QHash<QString,SimValueRef> _values;
};

#endif // __SIMVALUESMODEL_H__
