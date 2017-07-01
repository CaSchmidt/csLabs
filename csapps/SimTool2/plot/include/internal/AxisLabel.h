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

#ifndef __AXISLABEL_H__
#define __AXISLABEL_H__

#include <vector>

#include <QtCore/QList>
#include <QtCore/QString>

#define AXISLABEL_PREC  3
#define AXISLABEL_FMT  'f'

class AxisLabel {
public:
  AxisLabel(const qreal value,
            const int prec = AXISLABEL_PREC, const char fmt = AXISLABEL_FMT);
  ~AxisLabel();

  qreal value() const;
  QString text() const;

  // N == 10  ->  xN = { 1, 2, 2.5, 5 }
  static std::vector<double> computeValues(const double min, const double max,
                                           const double N, const double xN);
  static QString format(const qreal value,
                        const int prec = AXISLABEL_PREC, const char fmt = AXISLABEL_FMT);

  static QList<AxisLabel> fromValues(const std::vector<double>& values,
                                     const int prec = AXISLABEL_PREC, const char fmt = AXISLABEL_FMT);

private:
  AxisLabel();

  qreal _value;
  QString _text;
};

typedef QList<AxisLabel> AxisLabels;

#endif // __AXISLABEL_H__
