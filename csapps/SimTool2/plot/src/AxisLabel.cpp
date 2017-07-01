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

#include "internal/AxisLabel.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  double intg(const double x)
  {
    double i;
    std::modf(x, &i);
    return i;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

AxisLabel::AxisLabel(const qreal value,
                     const int prec, const char fmt)
  : _value(value)
  , _text()
{
  _text = format(value, prec, fmt);
}

AxisLabel::~AxisLabel()
{
}

qreal AxisLabel::value() const
{
  return _value;
}

QString AxisLabel::text() const
{
  return _text;
}

std::vector<double> AxisLabel::computeValues(const double min, const double max,
                                             const double N, const double xN)
{
  std::vector<double> result;

  if( max <= min ) {
    return result;
  }

  const double rawSpan = max - min;

  const double x10 = std::pow(10.0, std::floor(std::log10(rawSpan)));

  const double startStep = std::round(min/x10)*x10;
  const double   endStep = std::round(max/x10)*x10;
  const double  stepSpan = endStep - startStep;

  const double step = stepSpan/N*xN;

  const double startLabel = std::ceil( min/step)*step;
  const double   endLabel = std::floor(max/step)*step;
  const double  labelSpan = endLabel - startLabel;

  const int num = (int)priv::intg(labelSpan/step) + 1;
  result.resize(num, 0);
  for(int i = 0; i < num; i++) {
    result[i] = startLabel + (double)i*step;
  }

  return result;
}

QString AxisLabel::format(const qreal value,
                          const int prec, const char fmt)
{
  QString text = QString::number(value, fmt, prec);

  const int hit = text.lastIndexOf(QLatin1Char('.'));
  if( fmt == 'f'  &&  hit > 0 ) {
    // for each trailing zero
    for(int i = text.size()-1; i > hit; i--) {
      if( text.endsWith(QLatin1Char('0')) ) {
        text.chop(1);
      } else {
        break;
      }
    }
    // remove possible trailing dot
    if( hit == text.size()-1 ) {
      text.chop(1);
    }
  }

  return text;
}

QList<AxisLabel> AxisLabel::fromValues(const std::vector<double>& values,
                                       const int prec, const char fmt)
{
  AxisLabels result;

  if( values.empty() ) {
    return result;
  }

  result.reserve((int)values.size());
  for(const double value : values) {
    result.push_back(AxisLabel(value, prec, fmt));
  }

  return result;
}
