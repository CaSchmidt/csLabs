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

#include <limits>

#include <SimPlot/SimRange.h>

////// public ////////////////////////////////////////////////////////////////

SimRange::SimRange()
  : _min()
  , _max()
{
  initialize();
}

SimRange::SimRange(const qreal min, const qreal max)
  : _min(min)
  , _max(max)
{
}

SimRange::~SimRange()
{
}

bool SimRange::isValid() const
{
  return _min < _max;
}

void SimRange::adjust()
{
  if(        _min == _max ) {
    _min -= 0.5;
    _max += 0.5;
  } else if( _max < _min ) {
    _min = -0.5;
    _max =  0.5;
  }
}

void SimRange::initialize()
{
  _min = std::numeric_limits<qreal>::max();
  _max = std::numeric_limits<qreal>::lowest();
}

void SimRange::update(const qreal value)
{
  if( value < _min ) {
    _min = value;
  }
  if( value > _max ) {
    _max = value;
  }
}

void SimRange::update(const SimRange& other)
{
  if( !other.isValid() ) {
    return;
  }
  if( other._min < _min ) {
    _min = other._min;
  }
  if( other._max > _max ) {
    _max = other._max;
  }
}

SimRange SimRange::clamped(const SimRange& other, const qreal norm) const
{
  SimRange result;

  if( !isValid()  ||  !other.isValid() ) {
    return result;
  }

  const qreal ZERO(0);
  const qreal  ONE(1);
  const qreal from = qBound(ZERO, other.min()/norm, ONE);
  const qreal   to = qBound(ZERO, other.max()/norm, ONE);

  result._min = _min + from*span();
  result._max = _min +   to*span();

  return result;
}

qreal SimRange::span() const
{
  return _max - _min;
}

qreal SimRange::min() const
{
  return _min;
}

qreal SimRange::max() const
{
  return _max;
}
