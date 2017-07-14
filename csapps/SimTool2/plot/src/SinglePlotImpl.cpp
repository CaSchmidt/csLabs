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
#include "internal/Scope.h"
#include "internal/ScopeRow.h"
#include "internal/XAxis.h"
#include "internal/XTitle.h"
#include "internal/YAxis.h"
#include "internal/YTitle.h"

////// public ////////////////////////////////////////////////////////////////

SinglePlotImpl::SinglePlotImpl(const SimPlotTheme& theme, Widget *widget)
  : IPlotImplementation(theme)
  , _widget(widget)
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

QRectF SinglePlotImpl::boundingRect() const
{
  return _row->boundingRect() |
      _xAxis->boundingRect()  |
      _xTitle->boundingRect();
}

void SinglePlotImpl::resize(const QPointF& topLeft, const QSizeF& hint)
{
  Q_UNUSED(topLeft);

  _rect = QRectF(topLeft, hint);

  const QFontMetricsF metrics(_widget->font());

  const qreal rowWidth  = hint.width()
      - 2.0*LAYOUT_MARGIN;
  const qreal rowHeight = hint.height()
      - 2.0*LAYOUT_MARGIN - 2.0*LAYOUT_SPACING
      - 2.0*metrics.height();
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
  // background
  painter->fillRect(_rect, theme().backgroundBrush());
  // grid
  painter->setPen(theme().gridPen());
  const QTransform mapping = _row->mapToScreen();

  const AxisLabels xAxisLabels = _xAxis->labels();
  for(const AxisLabel& label : xAxisLabels) {
    const qreal x = std::floor(mapping.map(QPointF(label.value(), 0)).x()) + 0.5;
    const QPointF begin(_row->scope()->topLeft().x() + x,
                        _row->scope()->topLeft().y() + 0.5);
    const QPointF end(begin + QPointF(0, _row->scope()->size().height() - 1));
    painter->drawLine(begin, end);
  }

  const AxisLabels yAxisLabels = _row->yAxis()->labels();
  for(const AxisLabel& label : yAxisLabels) {
    const qreal y = std::floor(mapping.map(QPointF(0, label.value())).y()) + 0.5;
    const QPointF begin(_row->scope()->topLeft().x() + 0.5,
                        _row->scope()->topLeft().y() + y);
    const QPointF end(begin + QPointF(_row->scope()->size().width() - 1, 0));
    painter->drawLine(begin, end);
  }
  // frame
  painter->setPen(theme().framePen());
  painter->drawRect(_row->scope()->boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));

  painter->restore();

  _row->paint(painter);
  _xAxis->paint(painter);
  _xTitle->paint(painter);
}

bool SinglePlotImpl::insert(ISimPlotSeriesData *data, const QColor& color)
{
  if( data == 0  ||  data->isEmpty() ) {
    return false;
  }

  if( !_row->insert(Series(data, color.isValid()
                           ? color
                           : theme().nextColor())) ) {
    return false;
  }

  update();

  if( _row->activeSeriesName().isEmpty() ) {
    setActiveSeries(data->name());
  }

  return true;
}

bool SinglePlotImpl::remove(const QString& seriesName)
{
  if( !_row->remove(seriesName) ) {
    return false;
  }

  update();

  if( seriesName == _row->activeSeriesName() ) {
    QString newActiveSeriesName;

    QStringList names = _row->store().keys();
    if( !names.isEmpty() ) {
      qSort(names);
      newActiveSeriesName = names.front();
    }

    setActiveSeries(newActiveSeriesName);
  }

  return true;
}

const Widget *SinglePlotImpl::widget() const
{
  return _widget;
}

void SinglePlotImpl::reset()
{
  _row->resetView();
  update();
}

void SinglePlotImpl::update()
{
  _xAxis->clearLabels();
  _row->yAxis()->clearLabels();
  resize(_rect.topLeft(), _rect.size());
  _widget->update();
}

void SinglePlotImpl::setActiveSeries(const QString& seriesName)
{
  const QString oldActiveSeriesName = _row->activeSeriesName();
  _row->setActiveSeries(seriesName);
  if( oldActiveSeriesName != _row->activeSeriesName() ) {
    update();
  }
}

void SinglePlotImpl::setXTitle(const QString& title)
{
  _xTitle->setTitle(title);
}

SimPlotRange SinglePlotImpl::rangeX() const
{
  return _row->rangeX();
}

QTransform SinglePlotImpl::mapToScreen() const
{
  return _row->mapToScreen();
}

void SinglePlotImpl::rectangularZoom(const QRectF& zoomRect)
{
  _row->rectangularZoom(zoomRect);
  update();
}

void SinglePlotImpl::horizontalZoom(const QRectF& zoomRect)
{
  _row->horizontalZoom(zoomRect);
  update();
}

void SinglePlotImpl::verticalZoom(const QRectF& zoomRect)
{
  _row->verticalZoom(zoomRect);
  update();
}

void SinglePlotImpl::pan(const QPointF& delta)
{
  _row->pan(delta);
  update();
}

QColor SinglePlotImpl::seriesColor(const QString& seriesName) const
{
  const Series& s = _row->store().series(seriesName);
  if( s.isEmpty() ) {
    return QColor();
  }
  return s.color();
}

void SinglePlotImpl::setSeriesColor(const QString& seriesName, const QColor& color)
{
  Series& s = _row->store().series(seriesName);
  if( s.isEmpty() ) {
    return;
  }
  s.setColor(color);
  update();
}

const ISimPlotSeriesData *SinglePlotImpl::seriesData(const QString& seriesName) const
{
  const Series& s = _row->store().series(seriesName);
  if( s.isEmpty() ) {
    return 0;
  }
  return s.data().data();
}

ISimPlotSeriesData *SinglePlotImpl::seriesData(const QString& seriesName)
{
  Series& s = _row->store().series(seriesName);
  if( s.isEmpty() ) {
    return 0;
  }
  return s.data().data();
}
