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

#include "SimCore/SimDataSeries.h"

#include "SimCore/priv/SimDataSeriesPriv.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CONFIG_MIN_DEPTH  10
#define CONFIG_MAX_DEPTH  20

////// public ////////////////////////////////////////////////////////////////

SimDataSeries::SimDataSeries(SimDataSeriesPriv *data)
  : d(data)
{
}

SimDataSeries::SimDataSeries(const SimDataSeries& other)
  : d(other.d)
{
}

SimDataSeries::~SimDataSeries()
{
}

SimDataSeries& SimDataSeries::operator=(const SimDataSeries& other)
{
  if( this != &other ) {
    d = other.d;
  }
  return *this;
}

bool SimDataSeries::initialize(const int depth, const double value)
{
  const int size = calculateSize(depth);

  d->data.fill(value, size);
  d->mask = size-1;
  d->pos  = 0;

  return d->data.size() == size;
}

bool SimDataSeries::isNull() const
{
  return d.constData() == 0;
}

void SimDataSeries::clear()
{
  d = 0;
}

SimDataSeries SimDataSeries::copy() const
{
  return SimDataSeries(new SimDataSeriesPriv(d->data, d->mask, d->pos));
}

int SimDataSeries::depth() const
{
  return calculateDepth(d->data.size());
}

int SimDataSeries::mask() const
{
  return d->mask;
}

int SimDataSeries::position() const
{
  return d->pos;
}

int SimDataSeries::size() const
{
  return d->data.size();
}

void SimDataSeries::append(const double value)
{
  d->data[d->pos] = value;
  d->pos = (d->pos + 1) & d->mask;
}

double SimDataSeries::value(const int i) const
{
  return d->data[(d->pos + i) & d->mask];
}

int SimDataSeries::minDepth()
{
  return CONFIG_MIN_DEPTH;
}

int SimDataSeries::maxDepth()
{
  return CONFIG_MAX_DEPTH;
}

int SimDataSeries::calculateDepth(const int size)
{
  for(int i = 0; i < sizeof(int)*8; i++) {
    const int bit = 1 << i;
    if( (size & bit) != 0 ) {
      return i;
    }
  }
  return 0;
}

int SimDataSeries::calculateSize(const int depth)
{
  return 1 << qBound(minDepth(), depth, maxDepth());
}
