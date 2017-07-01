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

#include "internal/XTitle.h"

#include "internal/IPlotImplementation.h"

////// public ////////////////////////////////////////////////////////////////

XTitle::XTitle(IPlotImplementation *plot)
  : _rect()
  , _title()
  , _plot(plot)
{
}

XTitle::~XTitle()
{
}

QRectF XTitle::boundingRect() const
{
  return _rect;
}

void XTitle::resize(const QPointF& topLeft, const QSizeF& hint)
{
  const QFontMetricsF metrics(_plot->widget()->font());
  _rect = QRectF(topLeft, QSizeF(hint.width(), metrics.height()));
}

void XTitle::paint(QPainter *painter) const
{
  if( _title.isEmpty() ) {
    return;
  }

  const QFontMetricsF metrics(_plot->widget()->font());

  painter->save();

  painter->setFont(_plot->widget()->font());
  painter->setPen(_plot->theme().textColor);

  painter->drawText(QPointF(_rect.center()) +
                    QPointF(-metrics.width(_title)/2.0,
                            metrics.ascent() - metrics.height()/2.0),
                    _title);

  painter->restore();
}

QString XTitle::title() const
{
  return _title;
}

void XTitle::setTitle(const QString& title)
{
  _title = title;
}
