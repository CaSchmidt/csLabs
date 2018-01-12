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

#include "internal/Scale.h"

////// public ////////////////////////////////////////////////////////////////

Scale::Scale(QHash<QString,Series> *seriesStore)
  : _rangeX()
  , _rangeY()
  , _ranges()
  , _seriesStore(seriesStore)
{
}

Scale::~Scale()
{
}

bool Scale::isEmpty() const
{
  return _ranges.isEmpty();
}

bool Scale::contains(const QString& seriesName) const
{
  return _ranges.contains(seriesName);
}

bool Scale::insert(const QString& seriesName)
{
  if( _seriesStore == 0  ||  !_seriesStore->contains(seriesName) ) {
    return false;
  }
  if( _ranges.insert(seriesName) == _ranges.end() ) {
    return false;
  }
  updateRange();
  return true;
}

void Scale::remove(const QString& seriesName)
{
  if( !_ranges.remove(seriesName) ) {
    return;
  }
  updateRange();
}

SimPlotRange Scale::rangeX() const
{
  return _rangeX;
}

SimPlotRange Scale::rangeY() const
{
  return _rangeY;
}

////// private ///////////////////////////////////////////////////////////////

void Scale::updateRange()
{
  if( _seriesStore == 0 ) {
    return;
  }
  _rangeX.initialize();
  _rangeY.initialize();
  for(const QString& seriesName : _ranges) {
    const ISimPlotSeriesData *data = _seriesStore->value(seriesName).constData();
    _rangeX.update(data->rangeX());
    _rangeY.update(data->rangeY());
  }
  _rangeX.adjust();
  _rangeY.adjust();
}
