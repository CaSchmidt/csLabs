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

#include "internal/YTitle.h"

#include "internal/IPlotImplementation.h"
#include "internal/ScopeRow.h"

////// public ////////////////////////////////////////////////////////////////

YTitle::YTitle(ScopeRow *row)
  : _rect()
  , _row(row)
  , _title()
{
}

YTitle::~YTitle()
{
}

QRectF YTitle::boundingRect() const
{
  return _rect;
}

void YTitle::resize(const QPointF& topLeft, const QSizeF& hint)
{
  const QFontMetricsF metrics(_row->plot()->widget()->font());
  _rect = QRectF(topLeft, QSizeF(metrics.height(), hint.height()));
}

void YTitle::paint(QPainter *painter) const
{
  if( _title.isEmpty() ) {
    return;
  }

  const QFontMetricsF metrics(_row->plot()->widget()->font());
  const Series& series = _row->activeSeries();

  painter->save();

  painter->setFont(_row->plot()->widget()->font());
  painter->setPen(SimPlotTheme::yAxisPen(series.color()));

  const QTransform xform =
      QTransform(0, -1,
                 1,  0,
                 0,  0) *
      QTransform::fromTranslate(_rect.center().x(),
                                _rect.center().y()) *
      QTransform::fromTranslate(metrics.ascent() - metrics.height()/2.0,
                                metrics.width(_title)/2.0);
  painter->setTransform(xform);

  painter->drawText(QPointF(0, 0), _title);

  painter->restore();
}

QString YTitle::title() const
{
  return _title;
}

void YTitle::setTitle(const QString& title)
{
  _title = title;
}
