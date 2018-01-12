/****************************************************************************
** Copyright (c) 2017, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QWidget>

#include "internal/ScopeRow.h"

#include "internal/IPlotImplementation.h"
#include "internal/YTitle.h"
#include "internal/YAxis.h"
#include "internal/Scope.h"
#include "internal/Layout.h"
#include "internal/Mapping.h"

////// public ////////////////////////////////////////////////////////////////

ScopeRow::ScopeRow(IPlotImplementation *plot)
  : _yTitle()
  , _yAxis()
  , _scope()
  , _plot(plot)
  , _store()
  , _activeSeriesName()
  , _viewY()
{
  _yTitle = new YTitle(this);
  _yAxis  = new YAxis(this);
  _scope  = new Scope(this);

  _viewY.update(0);
  _viewY.update(100);
}

ScopeRow::~ScopeRow()
{
  delete _yTitle;
  delete _yAxis;
  delete _scope;
}

QRectF ScopeRow::boundingRect() const
{
  return
      _yTitle->boundingRect() |
      _yAxis->boundingRect()  |
      _scope->boundingRect();
}

void ScopeRow::resize(const QPointF& topLeft, const QSizeF& hint)
{
  _yTitle->resize(topLeft,
                  hint);
  _yAxis->resize(_yTitle->topRight() + QPointF(LAYOUT_SPACING, 0),
                 hint);
  _scope->resize(_yAxis->topRight() + QPointF(LAYOUT_SPACING, 0),
                 QSizeF(qMax(LAYOUT_MIN_SCOPE_WIDTH,
                             hint.width() - 2.0*LAYOUT_SPACING
                             - _yTitle->size().width() - _yAxis->size().width()),
                        hint.height()));
}

void ScopeRow::paint(QPainter *painter) const
{
  _yTitle->paint(painter);
  _yAxis->paint(painter);
  _scope->paint(painter);
}

const Scope *ScopeRow::scope() const
{
  return _scope;
}

IAxisElement *ScopeRow::yAxis()
{
  return _yAxis;
}

const IAxisElement *ScopeRow::yAxis() const
{
  return _yAxis;
}

ITitleElement *ScopeRow::yTitle()
{
  return _yTitle;
}

const ITitleElement *ScopeRow::yTitle() const
{
  return _yTitle;
}

const IPlotImplementation *ScopeRow::plot() const
{
  return _plot;
}

const SeriesStore& ScopeRow::store() const
{
  return _store;
}

SeriesStore& ScopeRow::store()
{
  return _store;
}

QString ScopeRow::activeSeriesName() const
{
  return _activeSeriesName;
}

const Series& ScopeRow::activeSeries() const
{
  return _store.series(_activeSeriesName);
}

void ScopeRow::clearActiveSeries()
{
  _activeSeriesName.clear();
  _yTitle->setTitle(QString());
}

bool ScopeRow::setActiveSeries(const QString& seriesName)
{
  if( seriesName.isEmpty()  ||  !_store.contains(seriesName)  ||
      _activeSeriesName == seriesName ) {
    return false;
  }
  _activeSeriesName = seriesName;
  const Series& s = _store.series(_activeSeriesName);
  _yTitle->setTitle(SimPlotTheme::titleString(s.name(), s.unit()));
  return true;
}

SimPlotRange ScopeRow::rangeY() const
{
  return _store.rangeY(_activeSeriesName).clamped(_viewY, 100);
}

SimPlotRange ScopeRow::viewY() const
{
  return _viewY;
}

void ScopeRow::setViewY(const SimPlotRange& viewY)
{
  _viewY = viewY;
}

QTransform ScopeRow::mapScaleToScreen() const
{
  return Mapping::viewToScreen(_scope->size(), _plot->rangeX(), rangeY());
}

QTransform ScopeRow::mapViewToScreen() const
{
  return Mapping::viewToScreen(_scope->size(), _plot->rangeX(), _viewY);
}

void ScopeRow::resetView()
{
  _viewY.initialize();
  _viewY.update(0);
  _viewY.update(100);
}
