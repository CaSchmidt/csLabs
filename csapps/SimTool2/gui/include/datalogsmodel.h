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

#ifndef __DATALOGSMODEL_H__
#define __DATALOGSMODEL_H__

#include <QtCore/QAbstractTableModel>
#include <QtCore/QBasicTimer>
#include <QtCore/QList>
#include <QtGui/QColor>

namespace QtCharts {
  class QChart;
  class QLineSeries;
  class QValueAxis;
}

class SimContext;

struct DataLogEntry {
  DataLogEntry()
    : name()
    , series(0)
    , unit()
    , yAxis(0)
  {
  }

  QString name;
  QtCharts::QLineSeries *series;
  QString unit;
  QtCharts::QValueAxis *yAxis;
};

class DataLogsModel : public QAbstractTableModel {
  Q_OBJECT
public:
  DataLogsModel(QtCharts::QChart *chart, SimContext *simctx, QObject *parent);
  ~DataLogsModel();

  bool addVariable(const QString& name, QColor color = QColor());

  int columnCount(const QModelIndex& parent) const;
  QVariant data(const QModelIndex& index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
  void enterState(int state);
  void exitState(int state);
  void clearVariables();
  void removeVariable(const QString& name);
  void highlightEntry(const QModelIndex& index);

protected:
  void timerEvent(QTimerEvent *event);

private slots:
  void highlightEntry(const QPointF& point);

private:
  bool contains(const QString& name) const;
  void highlightEntry(const QString& name);
  int indexOf(const QString& name) const;
  QString nameOf(const QtCharts::QLineSeries *series) const;
  Qt::GlobalColor nextColor();
  void removeEntry(const QString& name);

  QtCharts::QChart *_chart;
  int _colorIndex;
  QList<DataLogEntry> _entries;
  QBasicTimer _refreshTimer;
  SimContext *_simctx;
  QtCharts::QValueAxis *_timeAxis;

signals:
  void entryHighlighted(const QModelIndex& index);
};

#endif // __DATALOGSMODEL_H__
