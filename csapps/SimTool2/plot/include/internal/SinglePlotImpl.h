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

#ifndef __SINGLEPLOTIMPL_H__
#define __SINGLEPLOTIMPL_H__

#include "internal/IPlotImplementation.h"

class ScopeRow;
class XAxis;
class XTitle;

class SinglePlotImpl : public IPlotImplementation {
public:
  SinglePlotImpl(const SimPlotTheme& theme, Widget *widget);
  ~SinglePlotImpl();

  QRectF boundingRect() const;

  void resize(const QPointF& topLeft, const QSizeF& hint);

  void paint(QPainter *painter) const;

  bool insert(ISimPlotSeriesData *data, const QColor& color);

  bool remove(const QString& seriesName);

  const Widget *widget() const;

  void reset();

  void update();

  void setActiveSeries(const QString& seriesName);

  void setXTitle(const QString& title);

  SimPlotRange rangeX() const;

  QTransform mapToScreen() const;

  void rectangularZoom(const QRectF& zoomRect);
  void horizontalZoom(const QRectF& zoomRect);
  void verticalZoom(const QRectF& zoomRect);

  void pan(const QPointF& delta);

  QColor seriesColor(const QString& seriesName) const;
  void setSeriesColor(const QString& seriesName, const QColor& color);

private:
  Widget *_widget;
  QRectF _rect;
  ScopeRow *_row;
  XAxis *_xAxis;
  XTitle *_xTitle;
};

#endif // __SINGLEPLOTIMPL_H__
