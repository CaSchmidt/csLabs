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

#include "internal/XAxis.h"

#include "internal/IPlotImplementation.h"

////// public ////////////////////////////////////////////////////////////////

XAxis::XAxis(IPlotImplementation *plot)
  : _labels()
  , _labelsSize()
  , _rect()
  , _plot(plot)
{
}

XAxis::~XAxis()
{
}

QRectF XAxis::boundingRect() const
{
  return _rect;
}

void XAxis::resize(const QPointF& topLeft, const QSizeF& hint)
{
  const QFontMetricsF metrics(_plot->widget()->font());
  _rect = QRectF(topLeft, QSizeF(hint.width(), metrics.height()));

  const SimPlotRange rangeX = _plot->rangeX();
  if( !rangeX.isValid() ) {
    return;
  }

  updateLabels(hint);
}

void XAxis::paint(QPainter *painter) const
{
  painter->save();

  painter->setFont(_plot->widget()->font());
  painter->setPen(_plot->theme().textPen());

  const QFontMetricsF metrics(_plot->widget()->font());
  const QTransform xform = _plot->mapViewToScreenX();
  const qreal     yshift = metrics.ascent();

  for(const AxisLabel& label : _labels) {
    const qreal xshift = -metrics.width(label.text())/2.0;
    const QPointF p = _rect.topLeft() +
        QPointF(xform.map(QPointF(label.value(), 0)).x() + xshift,
                yshift);
    painter->drawText(p, label.text());
  }

  painter->restore();
}

AxisLabels XAxis::labels() const
{
  return _labels;
}

void XAxis::clearLabels()
{
  _labels.clear();
}

////// protected /////////////////////////////////////////////////////////////

void XAxis::updateLabels(const QSizeF& newSize)
{
  if( newSize.width() == _labelsSize.width()  &&  !_labels.isEmpty() ) {
    return;
  }

  const QFontMetricsF metrics(_plot->widget()->font());
  const SimPlotRange rangeX = _plot->rangeX();

  _labels.clear();
  _labelsSize.setWidth(0);
  _labelsSize.setHeight(0);

  const std::vector<double> factors = { 1, 2, 2.5, 5 };
  for(const double xN : factors) {
    const std::vector<double> values =
        AxisLabel::computeValues(rangeX.min(), rangeX.max(), 10, xN);
    if( values.empty() ) {
      continue;
    }

    const int numValues = (int)values.size();

    qreal width = metrics.averageCharWidth()*(numValues - 1);

    _labels = AxisLabel::fromValues(values);
    for(const AxisLabel& label : _labels) {
      width += metrics.width(label.text());
    }

    if( width > newSize.width() ) {
      _labels.clear();
      continue;
    }

    break;
  }

  _labelsSize.setWidth(newSize.width());
  _labelsSize.setHeight(metrics.height());
}
