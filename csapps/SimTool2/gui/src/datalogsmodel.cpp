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

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <csQt/csQtUtil.h>

#include <SimCore/SimContext.h>

#include "datalogsmodel.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CONFIG_REFRESH_RATE  200

////// Private ///////////////////////////////////////////////////////////////

namespace priv {
  enum Columns {
    COL_Name = 0,
    COL_Unit,
    NumColumns
  };

  static const Qt::Alignment ALIGN_X = Qt::AlignBottom;
  static const Qt::Alignment ALIGN_Y = Qt::AlignLeft;
  static const int        NUM_COLORS = 7;

  const Qt::GlobalColor palette[NUM_COLORS] = {
    Qt::blue, Qt::green, Qt::red,
    Qt::cyan, Qt::magenta, Qt::yellow, Qt::black
  };

  void setupFont(QtCharts::QAbstractAxis *axis)
  {
    QFont font = axis->titleFont();
    font.setBold(false);
    axis->setTitleFont(font);
  }

  void setupTitle(QtCharts::QAbstractAxis *axis,
                  const QString& name, const QString& unit)
  {
    if( unit.isEmpty() ) {
      axis->setTitleText(name);
    } else {
      axis->setTitleText(_L1("%1 [%2]").arg(name).arg(unit));
    }
  }
}

////// public ////////////////////////////////////////////////////////////////

DataLogsModel::DataLogsModel(QtCharts::QChart *chart, SimContext *simctx, QObject *parent)
  : QAbstractTableModel(parent)
  , _chart(chart)
  , _colorIndex(0)
  , _entries()
  , _refreshTimer()
  , _simctx(simctx)
  , _timeAxis(0)
{
  _timeAxis = new QtCharts::QValueAxis(this);
  _timeAxis->setLabelsColor(Qt::black);
  _timeAxis->setTitleBrush(Qt::black);
  _timeAxis->setTitleText(tr("Time [s]"));
  priv::setupFont(_timeAxis);
  _chart->addAxis(_timeAxis, priv::ALIGN_X);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(&_simctx->ctrl, &SimControl::stateEntered,
          this, &DataLogsModel::enterState);
  connect(&_simctx->ctrl, &SimControl::stateExited,
          this, &DataLogsModel::exitState);
  connect(&_simctx->env, &SimEnvironment::variablesCleared,
          this, &DataLogsModel::clearVariables);
  connect(&_simctx->env, &SimEnvironment::variableRemoved,
          this, &DataLogsModel::removeVariable);

  // Default State ///////////////////////////////////////////////////////////

  enterState(_simctx->sim.activeState());
}

DataLogsModel::~DataLogsModel()
{
}

bool DataLogsModel::addVariable(const QString& name, QColor color)
{
  if( contains(name) ) {
    return true;
  }

  if( !_simctx->logger.addLog(name) ) {
    return false;
  }
  const SimVariable& var = _simctx->env.variable(name);

  // Color ///////////////////////////////////////////////////////////////////

  if( !color.isValid() ) {
    color = nextColor();
  }

  // Entry ///////////////////////////////////////////////////////////////////

  DataLogEntry entry;
  entry.name = name;
  entry.unit = var.unit();

  // Axis ////////////////////////////////////////////////////////////////////

  entry.yAxis = new QtCharts::QValueAxis(this);
  entry.yAxis->setLabelsColor(color);
  entry.yAxis->setTitleBrush(color);
  priv::setupTitle(entry.yAxis, name, var.unit());
  priv::setupFont(entry.yAxis);
  _chart->addAxis(entry.yAxis, priv::ALIGN_Y);

  // Series //////////////////////////////////////////////////////////////////

  entry.series = new QtCharts::QLineSeries(this);
  entry.series->setColor(color);
  _chart->addSeries(entry.series);
  entry.series->attachAxis(_timeAxis);
  entry.series->attachAxis(entry.yAxis);

  // Insertion ///////////////////////////////////////////////////////////////

  int insertAt = 0;
  while( insertAt < _entries.size()  &&  name > _entries[insertAt].name ) {
    insertAt++;
  }

  beginInsertRows(QModelIndex(), insertAt, insertAt);
  _entries.insert(insertAt, entry);
  endInsertRows();

  // Highlight ///////////////////////////////////////////////////////////////

  highlightEntry(name);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(entry.series, &QtCharts::QLineSeries::clicked,
          this, QOverload<const QPointF&>::of(&DataLogsModel::highlightEntry));

  return true;
}

int DataLogsModel::columnCount(const QModelIndex& /*parent*/) const
{
  return priv::NumColumns;
}

QVariant DataLogsModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  const int col = index.column();
  const int row = index.row();
  if(        role == Qt::DisplayRole ) {
    if(        col == priv::COL_Name ) {
      return _entries[row].name;
    } else if( col == priv::COL_Unit ) {
      return _entries[row].unit;
    }
  } else if( role == Qt::DecorationRole ) {
    if( col == priv::COL_Name ) {
      return _entries[row].series->color();
    }
  }
  return QVariant();
}

QVariant DataLogsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( orientation == Qt::Horizontal ) {
    if( role == Qt::DisplayRole ) {
      if(        section == priv::COL_Name ) {
        return tr("Name");
      } else if( section == priv::COL_Unit ) {
        return tr("Unit");
      }
    } // DisplayRole
  } // Horizontal
  return QVariant();
}

int DataLogsModel::rowCount(const QModelIndex& /*parent*/) const
{
  return _entries.size();
}

////// public slots //////////////////////////////////////////////////////////

void DataLogsModel::enterState(int state)
{
  if( state == StepState ) {
    _refreshTimer.start(CONFIG_REFRESH_RATE, this);
  }
}

void DataLogsModel::exitState(int state)
{
  if( state == StepState ) {
    _refreshTimer.stop();
  }
}

void DataLogsModel::clearVariables()
{
  beginResetModel();
  while( !_entries.isEmpty() ) {
    removeEntry(_entries.front().name);
  }
  endResetModel();
}

void DataLogsModel::removeVariable(const QString& name)
{
  const int index = indexOf(name);
  if( index < 0 ) {
    return;
  }

  beginRemoveRows(QModelIndex(), index, index);
  removeEntry(name);
  endRemoveRows();
}

void DataLogsModel::highlightEntry(const QModelIndex& index)
{
  if( index.isValid() ) {
    highlightEntry(_entries[index.row()].name);
  }
}

////// protected /////////////////////////////////////////////////////////////

void DataLogsModel::timerEvent(QTimerEvent *event)
{
  if( event->timerId() == _refreshTimer.timerId() ) {
    if( _entries.isEmpty() ) {
      return;
    }

    _simctx->logger.lock();

    const SimDataSeries time = _simctx->logger.logTime();
    _timeAxis->setRange(time.value(0), time.value(time.size()-1));

    for(int i = 0; i < _entries.size(); i++) {
      const SimDataSeries data = _simctx->logger.logSeries(_entries[i].name);

      double min = std::numeric_limits<double>::max();
      double max = std::numeric_limits<double>::lowest();

      QVector<QPointF> points(qMin(time.size(), data.size()));
      for(int j = 0; j < points.size(); j++) {
        points[j].rx() = time.value(j);
        points[j].ry() = data.value(j);

        if( points[j].y() < min ) {
          min = points[j].y();
        }
        if( points[j].y() > max ) {
          max = points[j].y();
        }
      }

      if( std::fabs(max-min) < 0.001 ) {
        max += 0.5;
        min -= 0.5;
      }

      _entries[i].series->replace(points);
      _entries[i].yAxis->setRange(min, max);
    } // For Each Entry

    _simctx->logger.unlock();
  }
}

////// private slots /////////////////////////////////////////////////////////

void DataLogsModel::highlightEntry(const QPointF& /*point*/)
{
  QtCharts::QLineSeries *series = qobject_cast<QtCharts::QLineSeries*>(sender());
  highlightEntry(nameOf(series));
}

////// private ///////////////////////////////////////////////////////////////

bool DataLogsModel::contains(const QString& name) const
{
  if( name.isEmpty() ) {
    return false;
  }
  for(int i = 0; i < _entries.size(); i++) {
    if( _entries[i].name == name ) {
      return true;
    }
  }
  return false;
}

void DataLogsModel::highlightEntry(const QString& name)
{
  if( name.isEmpty() ) {
    return;
  }
  foreach(const DataLogEntry& e, _entries) {
    e.yAxis->setVisible(e.name == name);

    const qreal width = e.name == name  ?  2.0 : 1.0;
    QPen pen = e.series->pen();
    pen.setWidthF(width);
    e.series->setPen(pen);
  }

  const int row = indexOf(name);
  if( row < 0 ) {
    return;
  }
  emit entryHighlighted(index(row, 0));
}

int DataLogsModel::indexOf(const QString& name) const
{
  if( name.isEmpty() ) {
    return -1;
  }
  for(int i = 0; i < _entries.size(); i++) {
    if( _entries[i].name == name ) {
      return i;
    }
  }
  return -1;
}

QString DataLogsModel::nameOf(const QtCharts::QLineSeries *series) const
{
  if( series == 0 ) {
    return QString();
  }
  foreach(const DataLogEntry& e, _entries) {
    if( e.series == series ) {
      return e.name;
    }
  }
  return QString();
}

Qt::GlobalColor DataLogsModel::nextColor()
{
  const Qt::GlobalColor c = priv::palette[_colorIndex++];
  _colorIndex %= priv::NUM_COLORS;
  return c;
}

void DataLogsModel::removeEntry(const QString& name)
{
  const int index = indexOf(name);
  if( index < 0 ) {
    return;
  }

  _chart->removeAxis(_entries[index].yAxis);
  delete _entries[index].yAxis;
  _chart->removeSeries(_entries[index].series);
  delete _entries[index].series;
  _entries.removeAt(index);
}
