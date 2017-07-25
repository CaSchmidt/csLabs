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

#include "internal/YAxis.h"

#include "internal/IPlotImplementation.h"
#include "internal/ScopeRow.h"

////// public ////////////////////////////////////////////////////////////////

YAxis::YAxis(ScopeRow *row)
  : _labels()
  , _labelsSize()
  , _rect()
  , _row(row)
{
}

YAxis::~YAxis()
{
}

QRectF YAxis::boundingRect() const
{
  return _rect;
}

void YAxis::resize(const QPointF& topLeft, const QSizeF& hint)
{
  const QFontMetricsF metrics(_row->plot()->widget()->font());
  _rect = QRectF(topLeft, QSizeF(metrics.height(), hint.height()));

  const SimPlotRange rangeY = _row->rangeY();
  if( !rangeY.isValid() ) {
    return;
  }

  updateLabels(hint);
  if( _labelsSize.width() > 0 ) {
    _rect.setWidth(_labelsSize.width());
  }
}

void YAxis::paint(QPainter *painter) const
{
  painter->save();

  painter->setFont(_row->plot()->widget()->font());
  painter->setPen(SimPlotTheme::yAxisPen(_row->activeSeries().color()));

  const QFontMetricsF metrics(_row->plot()->widget()->font());
  const QTransform xform = _row->mapScaleToScreen();
  const qreal     yshift = metrics.ascent() - metrics.height()/2.0;

  for(const AxisLabel& label : _labels) {
    const QPointF p = _rect.topLeft() +
        QPointF(_labelsSize.width() - metrics.width(label.text()), yshift) +
        QPointF(0, xform.map(QPointF(0, label.value())).y());
    painter->drawText(p, label.text());
  }

  painter->restore();
}

AxisLabels YAxis::labels() const
{
  return _labels;
}

void YAxis::clearLabels()
{
  _labels.clear();
}

////// protected /////////////////////////////////////////////////////////////

void YAxis::updateLabels(const QSizeF& newSize)
{
  if( newSize.height() == _labelsSize.height()  &&  !_labels.isEmpty() ) {
    return;
  }

  const QFontMetricsF metrics(_row->plot()->widget()->font());
  const SimPlotRange rangeY = _row->rangeY();

  _labels.clear();
  _labelsSize.setWidth(0);
  _labelsSize.setHeight(0);

  const std::vector<double> factors = { 1, 2, 2.5, 5 };
  for(const double xN : factors) {
    const std::vector<double> values =
        AxisLabel::computeValues(rangeY.min(), rangeY.max(), 10, xN);
    if( values.empty() ) {
      continue;
    }

    const int numValues = (int)values.size();
    if( metrics.height()*(2*numValues - 1) > newSize.height() ) {
      continue;
    }

    _labels = AxisLabel::fromValues(values);
    break;
  }

  qreal minWidth = 0;
  for(const AxisLabel& label : _labels) {
    const qreal width = metrics.width(label.text());
    if( width > minWidth ) {
      minWidth = width;
    }
  }

  _labelsSize.setWidth(minWidth);
  _labelsSize.setHeight(newSize.height());
}
