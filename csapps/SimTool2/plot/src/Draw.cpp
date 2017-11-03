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

#include <QtGui/QPainter>

#include <SimPlot/SimPlotTheme.h>

#include "internal/Draw.h"

#include "internal/IAxisElement.h"
#include "internal/Mapping.h"
#include "internal/Series.h"

namespace Draw {

  void frame(QPainter *painter,
             const QRectF& rect, const QPen& pen)
  {
    painter->setPen(pen);
    painter->drawRect(rect.adjusted(0.5, 0.5, -0.5, -0.5));
  }

  void gridX(QPainter *painter,
             const IAxisElement *xAxis, const QTransform& mapping,
             const QRectF& rect, const QPen& pen)
  {
    painter->setPen(pen);

    const AxisLabels xAxisLabels = xAxis->labels();
    for(const AxisLabel& label : xAxisLabels) {
      const qreal x = std::floor(mapping.map(QPointF(label.value(), 0)).x());
      const QPointF begin(rect.left() + x + 0.5, rect.top() + 0.5);
      const QPointF end(begin + QPointF(0, rect.height() - 1));
      painter->drawLine(begin, end);
    }
  }

  void gridY(QPainter *painter,
             const IAxisElement *yAxis, const QTransform& mapping,
             const QRectF& rect, const QPen& pen)
  {
    painter->setPen(pen);

    const AxisLabels yAxisLabels = yAxis->labels();
    for(const AxisLabel& label : yAxisLabels) {
      const qreal y = std::floor(mapping.map(QPointF(0, label.value())).y());
      const QPointF begin(rect.left() + 0.5, rect.top() + y + 0.5);
      const QPointF end(begin + QPointF(rect.width() - 1, 0));
      painter->drawLine(begin, end);
    }
  }

  void series(QPainter *painter,
              const QRectF& screen, const Series& theSeries,
              const SimPlotRange& viewX, const SimPlotRange& viewY,
              const SimPlot::DrawFlags flags)
  {
    if( theSeries.isEmpty() ) {
      return;
    }

    const ISimPlotSeriesData *data = theSeries.constData();

    if( !viewX.isValid()  ||  !viewY.isValid()  ||  screen.isEmpty()  ||
        viewX.min() >= data->rangeX().max()  ||
        viewX.max() <= data->rangeX().min() ) {
      return;
    }

    QPen pen = SimPlotTheme::seriesPen(theSeries.color(),
                                       flags.testFlag(SimPlot::IsActive) ? 2.0 : 1.0);
    pen.setCosmetic(true);
    painter->setPen(pen);

    const QTransform xform =
        Mapping::viewToScreen(screen.size(), viewX, viewY) *
        QTransform::fromTranslate(screen.topLeft().x(), screen.topLeft().y());
    painter->setTransform(xform);

    const int L = data->findLeft(viewX.min()) >= 0
        ? data->findLeft(viewX.min())
        : 0;
    const int R = data->findRight(viewX.max()) >= 0
        ? data->findRight(viewX.max())
        : data->size() - 1;

    if( L >= R ) {
      return;
    }

    data->drawLines(painter, L, R);
  }

} // namespace Draw
