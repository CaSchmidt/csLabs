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

#include <SimPlot/ISimPlotSeriesData.h>

////// public ////////////////////////////////////////////////////////////////

ISimPlotSeriesData::ISimPlotSeriesData()
  : _rangeX()
  , _rangeY()
{
}

ISimPlotSeriesData::~ISimPlotSeriesData()
{
}

bool ISimPlotSeriesData::isEmpty() const
{
  return name().isEmpty()  ||  size() < 2;
}

int ISimPlotSeriesData::findLeft(const qreal x) const
{
  int L = 0;
  int R = size() - 1;

  while( L < R ) {
    const int M = L + (R - L)/2; // floor()
    if(        x <  valueX(M)   ) { // left
      R = M;
    } else if( x >= valueX(M+1) ) { // right
      L = M + 1;
    } else { // x[i] <= x  &&  x < x[i]
      return M;
    }
  }

  return -1;
}

int ISimPlotSeriesData::findRight(const qreal x) const
{
  int L = 0;
  int R = size() - 1;

  while( L < R ) {
    const int M = L + (R - L + 1)/2; // ceil()
    if(        x <= valueX(M-1) ) { // left
      R = M - 1;
    } else if( x >  valueX(M)   ) { // right
      L = M;
    } else { // x[i-1] < x  &&  x <= x[i]
      return M;
    }
  }

  return -1;
}

void ISimPlotSeriesData::drawLines(QPainter *painter,
                                   const int L, const int R) const
{
  const int Lines = 32;
  QPointF points[Lines+1];

  int i = L;
  points[0] = value(i);
  while( i + Lines <= R ) {
    values(&points[1], i+1, i+Lines);
    painter->drawPolyline(points, Lines+1);
    points[0] = points[Lines];
    i += Lines;
  }

  const int remain = R - i + 1; // Remaining Points!
  if( remain > 1 ) {
    values(&points[1], i+1, R);
    painter->drawPolyline(points, remain);
  }
}

void ISimPlotSeriesData::drawSteps(QPainter *painter,
                                   const int L, const int R) const
{
  const int Steps = 32;
  QPointF points[2*Steps+1];

  int i = L;
  points[0] = value(i);
  while( i + Steps <= R ) {
    values(&points[1], i+1, i+Steps);
    for(int j = Steps; j > 0; j--) {
      points[2*j  ] = points[j];
      points[2*j-1] = QPointF(points[j].x(), points[j-1].y());
    }
    painter->drawPolyline(points, 2*Steps+1);
    points[0] = points[2*Steps];
    i += Steps;
  }

  const int remain = R - i + 1; // Remaining Points!
  if( remain > 1 ) {
    values(&points[1], i+1, R);
    for(int j = remain-1; j > 0; j--) {
      points[2*j  ] = points[j];
      points[2*j-1] = QPointF(points[j].x(), points[j-1].y());
    }
    painter->drawPolyline(points, 2*remain-1);
  }
}

qreal ISimPlotSeriesData::valueX(const int i) const
{
  return value(i).x();
}

qreal ISimPlotSeriesData::valueY(const int i) const
{
  return value(i).y();
}

void ISimPlotSeriesData::values(QPointF *points, const int L, const int R) const
{
  for(int i = L; i <= R; i++) {
    *points++ = value(i);
  }
}

SimPlotRange ISimPlotSeriesData::rangeX() const
{
  return _rangeX;
}

SimPlotRange ISimPlotSeriesData::rangeY() const
{
  return _rangeY;
}

////// protected /////////////////////////////////////////////////////////////

void ISimPlotSeriesData::updateRange()
{
  _rangeX.initialize();
  _rangeY.initialize();
  for(int i = 0; i < size(); i++) {
    _rangeX.update(valueX(i));
    _rangeY.update(valueY(i));
  }
  _rangeX.adjust();
  _rangeY.adjust();
}
