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

#include "internal/Series.h"

////// public ////////////////////////////////////////////////////////////////

Series::Series(ISimPlotSeriesData *data, const QColor& color)
  : _dataPtr(data)
  , _color(color)
  , _scaleName()
{
  if( !isEmpty() ) {
    _scaleName = _dataPtr->unit();
  }
}

Series::~Series()
{
}

bool Series::isEmpty() const
{
  return _dataPtr.isNull()  ||  _dataPtr->name().isEmpty();
}

QColor Series::color() const
{
  return _color;
}

void Series::setColor(const QColor& color)
{
  _color = color;
}

SeriesDataPtr Series::data() const
{
  return _dataPtr;
}

QPainterPath Series::path(const SimRange& viewX) const
{
  QPainterPath result;

  if( !viewX.isValid()  ||
      viewX.min() >= _dataPtr->rangeX().max()  ||
      viewX.max() <= _dataPtr->rangeX().min() ) {
    return result;
  }

  const int L = _dataPtr->findLeft(viewX.min()) >= 0
      ? _dataPtr->findLeft(viewX.min())
      : 0;
  const int R = _dataPtr->findRight(viewX.max()) >= 0
      ? _dataPtr->findRight(viewX.max())
      : _dataPtr->size() - 1;

  if( L >= R ) {
    return result;
  }

  result.moveTo(_dataPtr->value(L));
  for(int i = L + 1; i <= R; i++) {
    result.lineTo(_dataPtr->value(i));
  }

  return result;
}

QString Series::name() const
{
  return _dataPtr->name();
}

QString Series::unit() const
{
  return _dataPtr->unit();
}

QString Series::scale() const
{
  return _scaleName;
}

void Series::setScale(const QString& scaleName)
{
  _scaleName = scaleName;
}

void Series::resetScale()
{
  _scaleName = _dataPtr->unit();
}
