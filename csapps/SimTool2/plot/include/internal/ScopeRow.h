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

#ifndef __SCOPEROW_H__
#define __SCOPEROW_H__

#include "internal/IPlotElement.h"
#include "internal/SeriesStore.h"

class IPlotImplementation;
class YTitle;
class YAxis;
class Scope;

class ScopeRow : public IPlotElement {
public:
  ScopeRow(IPlotImplementation *plot);
  ~ScopeRow();

  QRectF boundingRect() const;

  void resize(const QPointF& topLeft, const QSizeF& hint);

  void paint(QPainter *painter) const;

  const Scope *scope() const;

  YAxis *yAxis();
  const YAxis *yAxis() const;

  YTitle *yTitle();
  const YTitle *yTitle() const;

  bool insert(const Series& series);

  bool remove(const QString& seriesName);

  const IPlotImplementation *plot() const;

  const SeriesStore& store() const;
  SeriesStore& store();

  QString activeSeriesName() const;
  const Series& activeSeries() const;
  void setActiveSeries(const QString& seriesName);

  SimPlotRange rangeX() const;
  SimPlotRange rangeY() const;

  SimPlotRange viewX() const;
  SimPlotRange viewY() const;

  QTransform mapScaleToScreen() const;
  QTransform mapViewToScreen() const;

  void resetView();

  void rectangularZoom(const QRectF& zoomRect);
  void horizontalZoom(const QRectF& zoomRect);
  void verticalZoom(const QRectF& zoomRect);

  void pan(const QPointF& delta);

private:
  QRectF zoomedView(const QRectF& zoomRect) const;

  YTitle *_yTitle;
  YAxis *_yAxis;
  Scope *_scope;
  IPlotImplementation *_plot;
  SeriesStore _store;
  QString _activeSeriesName;
  SimPlotRange _viewX;
  SimPlotRange _viewY;
};

#endif // __SCOPEROW_H__
