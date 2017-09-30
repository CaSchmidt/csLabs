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

#include <SimPlot/SimPlotTheme.h>

#include "internal/Scope.h"

#include "internal/Mapping.h"
#include "internal/ScopeRow.h"

////// public ////////////////////////////////////////////////////////////////

Scope::Scope(ScopeRow *row)
  : _rect()
  , _row(row)
{
}

Scope::~Scope()
{
}

QRectF Scope::boundingRect() const
{
  return _rect;
}

void Scope::resize(const QPointF& topLeft, const QSizeF& hint)
{
  _rect = QRectF(topLeft, hint);
}

void Scope::paint(QPainter *painter) const
{
  painter->save();

  painter->setBrush(Qt::NoBrush);

  const Series& activeSeries = _row->activeSeries();

  painter->resetTransform();
  painter->setClipRect(_rect);
  for(const QString& seriesName : _row->store().names()) {
    const Series& series = _row->store().series(seriesName);
    if( series.isEmpty()  ||  series == activeSeries ) {
      continue;
    }
    draw(painter, series);
  }

  if( !activeSeries.isEmpty() ) {
    painter->resetTransform();
    painter->setClipRect(_rect.adjusted(-1, -1, 1, 1));
    draw(painter, activeSeries, true);
  }

  painter->restore();
}

////// private ///////////////////////////////////////////////////////////////

void Scope::draw(QPainter *painter, const Series& series,
                 const bool isActive) const
{
  QPen pen = SimPlotTheme::seriesPen(series.color(), isActive ? 2.0 : 1.0);
  pen.setCosmetic(true);
  painter->setPen(pen);

  const SimPlotRange rangeY = _row->store().rangeY(series.name());
  const QTransform xform =
      mapViewToScreen(_rect.size(),
                      _row->rangeX(),
                      rangeY.clamped(_row->viewY(), 100)) *
      QTransform::fromTranslate(_rect.topLeft().x(),
                                _rect.topLeft().y());
  painter->setTransform(xform);

  const ISimPlotSeriesData *data = series.constData();

  if( !_row->viewX().isValid()  ||
      _row->viewX().min() >= data->rangeX().max()  ||
      _row->viewX().max() <= data->rangeX().min() ) {
    return;
  }

  const int L = data->findLeft(_row->viewX().min()) >= 0
      ? data->findLeft(_row->viewX().min())
      : 0;
  const int R = data->findRight(_row->viewX().max()) >= 0
      ? data->findRight(_row->viewX().max())
      : data->size() - 1;

  if( L >= R ) {
    return;
  }

  data->drawLines(painter, L, R);
}
