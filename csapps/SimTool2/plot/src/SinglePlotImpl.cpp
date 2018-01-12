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

#include "internal/SinglePlotImpl.h"

#include "internal/Layout.h"
#include "internal/PanAndZoom.h"
#include "internal/Scope.h"
#include "internal/ScopeRow.h"
#include "internal/XAxis.h"
#include "internal/XTitle.h"

////// public ////////////////////////////////////////////////////////////////

SinglePlotImpl::SinglePlotImpl(const SimPlot::DrawFlags drawFlags, const SimPlotTheme& theme, Widget *widget)
  : IPlotImplementation(drawFlags, theme)
  , _widget(widget)
  , _rangeX()
  , _rect()
  , _row()
  , _xAxis()
  , _xTitle()
{
  _row    = new ScopeRow(this);
  _xAxis  = new XAxis(this);
  _xTitle = new XTitle(this);
}

SinglePlotImpl::~SinglePlotImpl()
{
  delete _row;
  delete _xAxis;
  delete _xTitle;
}

void SinglePlotImpl::replot()
{
  _xAxis->clearLabels();
  _row->yAxis()->clearLabels();
  resize(_rect.topLeft(), _rect.size());
  QMetaObject::invokeMethod(_widget, "update", Qt::QueuedConnection);
}

void SinglePlotImpl::reset()
{
  _rangeX = totalRangeX();
  _row->resetView();
  replot();
}

const Widget *SinglePlotImpl::widget() const
{
  return _widget;
}

// IPlotElement //////////////////////////////////////////////////////////////

QRectF SinglePlotImpl::boundingRect() const
{
  return _row->boundingRect() |
      _xAxis->boundingRect()  |
      _xTitle->boundingRect();
}

void SinglePlotImpl::resize(const QPointF& topLeft, const QSizeF& hint)
{
  _rect = QRectF(topLeft, hint);

  const QFontMetricsF metrics(_widget->font());

  const qreal rowWidth  = hint.width()
      - 2.0*LAYOUT_MARGIN;
  const qreal rowHeight =
      qMax(LAYOUT_MIN_SCOPE_HEIGHT,
           hint.height()
           - 2.0*LAYOUT_MARGIN - 2.0*LAYOUT_SPACING
           - 2.0*metrics.height());
  _row->resize(QPointF(LAYOUT_MARGIN, LAYOUT_MARGIN),
               QSizeF(rowWidth, rowHeight));

  _xAxis->resize(_row->scope()->bottomLeft() + QPointF(0, LAYOUT_SPACING),
                 QSizeF(_row->scope()->size().width(), 0));
  _xTitle->resize(_xAxis->bottomLeft() + QPointF(0, LAYOUT_SPACING),
                  QSizeF(_row->scope()->size().width(), 0));
}

void SinglePlotImpl::paint(QPainter *painter) const
{
  painter->save();

  painter->setBrush(Qt::NoBrush);

  painter->fillRect(_rect, theme().backgroundBrush());

  painter->restore();

  _row->paint(painter);
  _xAxis->paint(painter);
  _xTitle->paint(painter);
}

// ISeriesControl ////////////////////////////////////////////////////////////

SimPlotSeriesHandle SinglePlotImpl::handle(const QString& name) const
{
  if( !_row->store().contains(name) ) {
    return SimPlotSeriesHandle();
  }
  const IPlotImplementation *impl = dynamic_cast<const IPlotImplementation*>(this);
  return SimPlotSeriesHandle(name, const_cast<IPlotImplementation*>(impl));
}

SimPlotSeriesHandle SinglePlotImpl::insert(ISimPlotSeriesData *data,
                                           const QColor& color)
{
  if( data == 0  ||  data->isEmpty() ) {
    return SimPlotSeriesHandle();
  }

  if( !_row->store().insert(Series(data, color.isValid()
                                   ? color
                                   : theme().nextColor())) ) {
    return SimPlotSeriesHandle();
  }

  replot();

  if( _row->activeSeriesName().isEmpty() ) {
    setActiveSeries(data->name());
  }

  return SimPlotSeriesHandle(data->name(), this);
}

bool SinglePlotImpl::remove(const QString& name)
{
  if( !_row->store().remove(name) ) {
    return false;
  }

  replot();

  if( name == _row->activeSeriesName() ) {
    _row->clearActiveSeries();
    QString newActiveSeriesName;

    QStringList names = _row->store().names();
    if( !names.isEmpty() ) {
      qSort(names);
      newActiveSeriesName = names.front();
    }

    setActiveSeries(newActiveSeriesName);
  }

  return true;
}

const Series& SinglePlotImpl::series(const QString& name) const
{
  return _row->store().series(name);
}

Series& SinglePlotImpl::series(const QString& name)
{
  return _row->store().series(name);
}

bool SinglePlotImpl::setActiveSeries(const QString& name)
{
  if( !_row->setActiveSeries(name) ) {
    return false;
  }
  if( !_rangeX.isValid() ) {
    _rangeX = _row->store().rangeX(_row->activeSeriesName());
  }
  replot();
  return true;
}

// IViewportControl //////////////////////////////////////////////////////////

void SinglePlotImpl::pan(const QPointF& delta)
{
  const SimPlotRange oldX = _rangeX;
  const SimPlotRange oldY = _row->viewY();

  const SimPlotRange newX =
      Pan::horizontal(delta, _row->scope()->size(), oldX, oldY, totalRangeX());
  if( newX.isValid() ) {
    _rangeX = newX;
  }

  const SimPlotRange newY =
      Pan::vertical(delta, _row->scope()->size(), oldX, oldY, SimPlotRange(0, 100));
  if( newY.isValid() ) {
    _row->setViewY(newY);
  }

  replot();
}

void SinglePlotImpl::rectangularZoom(const QRectF& zoomRect)
{
  const QRectF newView =
      ZoomIn::rectangular(zoomRect, _row->scope()->boundingRect(), _rangeX, _row->viewY());
  if( newView.isEmpty() ) {
    return;
  }

  _rangeX = SimPlotRange(newView.left(), newView.right());

  _row->setViewY(SimPlotRange(newView.top(), newView.bottom()));

  replot();
}

void SinglePlotImpl::horizontalZoom(const QRectF& zoomRect)
{
  const QRectF newView =
      ZoomIn::rectangular(zoomRect, _row->scope()->boundingRect(), _rangeX, _row->viewY());
  if( newView.isEmpty() ) {
    return;
  }

  _rangeX = SimPlotRange(newView.left(), newView.right());

  replot();
}

void SinglePlotImpl::verticalZoom(const QRectF& zoomRect)
{
  const QRectF newView =
      ZoomIn::rectangular(zoomRect, _row->scope()->boundingRect(), _rangeX, _row->viewY());
  if( newView.isEmpty() ) {
    return;
  }

  _row->setViewY(SimPlotRange(newView.top(), newView.bottom()));

  replot();
}

// IXAxisControl /////////////////////////////////////////////////////////////

QTransform SinglePlotImpl::mapViewToScreenX() const
{
  return _row->mapViewToScreen();
}

SimPlotRange SinglePlotImpl::rangeX() const
{
  return _rangeX;
}

void SinglePlotImpl::setTitleX(const QString& title)
{
  _xTitle->setTitle(title);
  replot();
}

SimPlotRange SinglePlotImpl::totalRangeX() const
{
  return _row->store().totalRangeX();
}

const IAxisElement *SinglePlotImpl::xAxis() const
{
  return _xAxis;
}
