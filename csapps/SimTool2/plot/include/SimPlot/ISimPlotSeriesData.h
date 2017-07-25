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

#ifndef __ISIMPLOTSERIESDATA_H__
#define __ISIMPLOTSERIESDATA_H__

#include <QtCore/QPointF>
#include <QtCore/QString>

#include <SimPlot/SimPlotRange.h>

class QPainter;

class SIM_PLOT_EXPORT ISimPlotSeriesData {
public:
  ISimPlotSeriesData();
  virtual ~ISimPlotSeriesData();

  bool isEmpty() const;

  // Returns i := x[i]   <= x  &&  x <  x[i+1]
  int findLeft(const qreal x) const;
  // Returns i := x[i-1] <  x  &&  x <= x[i]
  int findRight(const qreal x) const;

  void drawLines(QPainter *painter, const int L, const int R) const;
  void drawSteps(QPainter *painter, const int L, const int R) const;

  virtual QString name() const = 0;
  virtual QString unit() const = 0;

  virtual QPointF value(const int i) const = 0;
  virtual int size() const = 0;

  virtual qreal valueX(const int i) const;
  virtual qreal valueY(const int i) const;

  // NOTE: QPointF points[R - L + 1] required!
  virtual void values(QPointF *points, const int L, const int R) const;

  virtual SimPlotRange rangeX() const;
  virtual SimPlotRange rangeY() const;

protected:
  virtual void updateRange();

private:
  SimPlotRange _rangeX;
  SimPlotRange _rangeY;
};

#endif // __ISIMPLOTSERIESDATA_H__
