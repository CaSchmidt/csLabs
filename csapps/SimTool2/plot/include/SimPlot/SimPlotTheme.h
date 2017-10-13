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

#ifndef __SIMPLOTTHEME_H__
#define __SIMPLOTTHEME_H__

#include <QtCore/QVector>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QPen>

#include <SimPlot/simplot_config.h>

struct SIM_PLOT_EXPORT SimPlotTheme {
  SimPlotTheme();

  QColor backgroundColor;
  QColor frameColor;
  QColor gridColor;
  QColor rubberColor;
  QColor textColor;
  QVector<QColor> seriesColors;

  QColor nextColor();

  QBrush backgroundBrush() const;
  QPen framePen() const;
  QPen gridPen() const;
  QPen rubberPen() const;
  QPen textPen() const;

  static QPen seriesPen(const QColor& color, const qreal width = 1.0);
  static QPen yAxisPen(const QColor& color);

  static bool isEmptyUnit(const QString& unit);
  static QString cleanUnit(const QString& unit);

  static QString titleString(const QString& name, const QString& unit);

  static SimPlotTheme themeTextbook();
  static SimPlotTheme themeOscilloscope();

private:
  int _colorIndex;
};

#endif // __SIMPLOTTHEME_H__
