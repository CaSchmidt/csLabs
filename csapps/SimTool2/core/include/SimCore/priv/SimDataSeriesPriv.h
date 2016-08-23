/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
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

#ifndef __SIMDATASERIESPRIV_H__
#define __SIMDATASERIESPRIV_H__

#include <QtCore/QSharedData>
#include <QtCore/QVector>

class SimDataSeriesPriv : public QSharedData {
public:
  SimDataSeriesPriv()
    : QSharedData()
    , data()
    , mask()
    , pos()
  {
  }

  SimDataSeriesPriv(const SimDataSeriesPriv& other)
    : QSharedData(other)
    , data(other.data)
    , mask(other.mask)
    , pos(other.pos)
  {
  }

  SimDataSeriesPriv(const QVector<double>& _data,
                    const int _mask, const int _pos)
    : QSharedData()
    , data()
    , mask()
    , pos()
  {
    data.append(_data); // work around implicit sharing
    if( !data.isEmpty() ) {
      mask = _mask;
      pos  = _pos;
    }
  }

  ~SimDataSeriesPriv()
  {
  }

  QVector<double> data;
  int mask;
  int pos; // index of next write; "oldest" value
};

#endif // __SIMDATASERIESPRIV_H__
