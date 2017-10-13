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

////// public ////////////////////////////////////////////////////////////////

SimPlotTheme::SimPlotTheme()
  : backgroundColor()
  , frameColor()
  , gridColor()
  , rubberColor()
  , textColor()
  , seriesColors()
  , _colorIndex(0)
{
}

QColor SimPlotTheme::nextColor()
{
  QColor result = seriesColors[_colorIndex];
  _colorIndex += 1;
  _colorIndex %= seriesColors.size();
  return result;
}

QBrush SimPlotTheme::backgroundBrush() const
{
  return QBrush(backgroundColor, Qt::SolidPattern);
}

QPen SimPlotTheme::framePen() const
{
  return QPen(QBrush(frameColor, Qt::SolidPattern), 1.0,
              Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
}

QPen SimPlotTheme::gridPen() const
{
  return QPen(QBrush(gridColor, Qt::SolidPattern), 1.0,
              Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
}

QPen SimPlotTheme::rubberPen() const
{
  return QPen(QBrush(rubberColor, Qt::SolidPattern), 1.0,
              Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
}

QPen SimPlotTheme::textPen() const
{
  return QPen(QBrush(textColor, Qt::SolidPattern), 0.0);
}

QPen SimPlotTheme::seriesPen(const QColor& color, const qreal width)
{
  return QPen(QBrush(color, Qt::SolidPattern), width,
              Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

QPen SimPlotTheme::yAxisPen(const QColor& color)
{
  return QPen(QBrush(color, Qt::SolidPattern), 0.0);
}

bool SimPlotTheme::isEmptyUnit(const QString& unit)
{
  for(const QChar& c : unit) {
    if( !c.isSpace()  &&  c != QLatin1Char('-') ) {
      return false;
    }
  }
  return true;
}

QString SimPlotTheme::cleanUnit(const QString& unit)
{
  QString result(unit);
  if( isEmptyUnit(unit) ) {
    result = QStringLiteral("-");
  }
  return result;
}

QString SimPlotTheme::titleString(const QString& name, const QString& unit)
{
  QString result;
  result = QStringLiteral("%1 [%2]").arg(name).arg(cleanUnit(unit));
  return result;
}

SimPlotTheme SimPlotTheme::themeTextbook()
{
  SimPlotTheme result;

  result.backgroundColor = Qt::white;
  result.frameColor      = Qt::black;
  result.gridColor       = Qt::black;
  result.rubberColor     = Qt::black;
  result.textColor       = Qt::black;
  result.seriesColors    = {
    Qt::blue, Qt::green, Qt::red, Qt::cyan, Qt::magenta, Qt::yellow, Qt::black
  };

  return result;
}

SimPlotTheme SimPlotTheme::themeOscilloscope()
{
  SimPlotTheme result;

  result.backgroundColor = Qt::black;
  result.frameColor      = Qt::white;
  result.gridColor       = Qt::white;
  result.rubberColor     = Qt::white;
  result.textColor       = Qt::white;
  result.seriesColors    = {
    Qt::blue, Qt::green, Qt::red, Qt::cyan, Qt::magenta, Qt::yellow, Qt::white
  };

  return result;
}
