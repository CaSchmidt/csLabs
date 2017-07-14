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
  , _viewX()
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
                 QSizeF(hint.width() - 2.0*LAYOUT_SPACING
                        - _yTitle->size().width() - _yAxis->size().width(),
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

YAxis *ScopeRow::yAxis()
{
  return _yAxis;
}

const YAxis *ScopeRow::yAxis() const
{
  return _yAxis;
}

YTitle *ScopeRow::yTitle()
{
  return _yTitle;
}

const YTitle *ScopeRow::yTitle() const
{
  return _yTitle;
}

bool ScopeRow::insert(const Series& series)
{
  return _store.insert(series);
}

bool ScopeRow::remove(const QString& seriesName)
{
  return _store.remove(seriesName);
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

Series ScopeRow::activeSeries() const
{
  return _store.value(_activeSeriesName);
}

void ScopeRow::setActiveSeries(const QString& seriesName)
{
  if( !seriesName.isEmpty()  &&  !_store.contains(seriesName) ) {
    return;
  }
  _activeSeriesName = seriesName;
  _yTitle->setTitle(_activeSeriesName);
  if( !_activeSeriesName.isEmpty()  &&  !_viewX.isValid() ) {
    _viewX = _store.rangeX(_activeSeriesName);
  }
}

SimPlotRange ScopeRow::rangeX() const
{
  return _viewX;
}

SimPlotRange ScopeRow::rangeY() const
{
  return _store.rangeY(_activeSeriesName).clamped(_viewY, 100);
}

SimPlotRange ScopeRow::viewX() const
{
  return _viewX;
}

SimPlotRange ScopeRow::viewY() const
{
  return _viewY;
}

QTransform ScopeRow::mapToScreen() const
{
  return ::mapToScreen(_scope->size(), rangeX(), rangeY());
}

void ScopeRow::resetView()
{
  _viewX = _store.totalRangeX();

  _viewY.initialize();
  _viewY.update(0);
  _viewY.update(100);
}

void ScopeRow::rectangularZoom(const QRectF& zoomRect)
{
  if( !_viewX.isValid()  ||  !_viewY.isValid() ) {
    return;
  }

  const QRectF newView = zoomedView(zoomRect);
  if( newView.isEmpty() ) {
    return;
  }

  _viewX.initialize();
  _viewX.update(newView.left());
  _viewX.update(newView.right());

  _viewY.initialize();
  _viewY.update(newView.top());
  _viewY.update(newView.bottom());
}

void ScopeRow::horizontalZoom(const QRectF& zoomRect)
{
  if( !_viewX.isValid()  ||  !_viewY.isValid() ) {
    return;
  }

  const QRectF newView = zoomedView(zoomRect);
  if( newView.isEmpty() ) {
    return;
  }

  _viewX.initialize();
  _viewX.update(newView.left());
  _viewX.update(newView.right());
}

void ScopeRow::verticalZoom(const QRectF& zoomRect)
{
  if( !_viewX.isValid()  ||  !_viewY.isValid() ) {
    return;
  }

  const QRectF newView = zoomedView(zoomRect);
  if( newView.isEmpty() ) {
    return;
  }

  _viewY.initialize();
  _viewY.update(newView.top());
  _viewY.update(newView.bottom());
}

void ScopeRow::pan(const QPointF& delta)
{
  if( !_viewX.isValid()  ||  !_viewY.isValid() ) {
    return;
  }

  const QTransform  xform = ::mapToView(_scope->size(), _viewX, _viewY, true);
  const QPointF deltaView = xform.map(delta);

  // Horizontal

  const SimPlotRange boundsX = _store.totalRangeX();

  qreal leftX = _viewX.min() - deltaView.x();
  if(        leftX < boundsX.min() ) {
    leftX += boundsX.min() - leftX;
  } else if( leftX + _viewX.span() > boundsX.max() ) {
    leftX -= leftX + _viewX.span() - boundsX.max();
  }
  _viewX = SimPlotRange(leftX, leftX + _viewX.span());

  // Vertical

  const SimPlotRange boundsY(0, 100);

  qreal bottomY = _viewY.min() + deltaView.y();
  if(        bottomY < boundsY.min() ) {
    bottomY += boundsY.min() - bottomY;
  } else if( bottomY + _viewY.span() > boundsY.max() ) {
    bottomY -= bottomY + _viewY.span() - boundsY.max();
  }
  _viewY = SimPlotRange(bottomY, bottomY + _viewY.span());
}

////// private ///////////////////////////////////////////////////////////////

QRectF ScopeRow::zoomedView(const QRectF& zoomRect) const
{
  QRectF result;

  const QRectF inter = _scope->boundingRect() & zoomRect;
  if( inter.isEmpty() ) {
    return result;
  }

  const QTransform xform = ::mapToView(_scope->size(), _viewX, _viewY);
  const QRectF  zoomView =
      xform.map(inter.translated(-_scope->topLeft())).boundingRect();

  const QRectF view(QPointF(_viewX.min(), _viewY.min()),
                    QPointF(_viewX.max(), _viewY.max()));

  result = view & zoomView;

  return result;
}
