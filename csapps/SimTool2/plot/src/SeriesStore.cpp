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

#include "internal/SeriesStore.h"

////// private static ////////////////////////////////////////////////////////

Series SeriesStore::_nullSeries;

////// public ////////////////////////////////////////////////////////////////

SeriesStore::SeriesStore()
  : _scales()
  , _series()
{
}

SeriesStore::~SeriesStore()
{
}

bool SeriesStore::contains(const QString& seriesName) const
{
  return _series.contains(seriesName);
}

QStringList SeriesStore::names() const
{
  return _series.keys();
}

bool SeriesStore::insert(const Series& series)
{
  if( _series.contains(series.name()) ) {
    return false;
  }
  if( _series.insert(series.name(), series) == _series.end() ) {
    return false;
  }
  if( !addToScales(series.name()) ) {
    _series.remove(series.name());
    return false;
  }
  return true;
}

bool SeriesStore::remove(const QString& seriesName)
{
  if( !_series.contains(seriesName) ) {
    return false;
  }
  removeFromScales(seriesName);
  _series.remove(seriesName);
  return true;
}

SimPlotRange SeriesStore::rangeX(const QString& seriesName) const
{
  SimPlotRange result;

  if( !_series.contains(seriesName) ) {
    return result;
  }

  const QString scaleName = _series[seriesName].unit();
  if( isGroupedScale(seriesName, scaleName) ) {
    result = _scales[scaleName].rangeX();
  } else {
    result = _series[seriesName].constData()->rangeX();
  }

  return result;
}

SimPlotRange SeriesStore::rangeY(const QString& seriesName) const
{
  SimPlotRange result;

  if( !_series.contains(seriesName) ) {
    return result;
  }

  const QString scaleName = _series[seriesName].unit();
  if( isGroupedScale(seriesName, scaleName) ) {
    result = _scales[scaleName].rangeY();
  } else {
    result = _series[seriesName].constData()->rangeY();
  }

  return result;
}

SimPlotRange SeriesStore::totalRangeX() const
{
  SimPlotRange result;

  for(const Series& series : _series) {
    result.update(series.constData()->rangeX());
  }

  return result;
}

Series& SeriesStore::series(const QString& seriesName)
{
  QHash<QString,Series>::iterator it = _series.find(seriesName);
  if( it == _series.end() ) {
    _nullSeries = Series();
    return _nullSeries;
  }
  return it.value();
}

const Series& SeriesStore::series(const QString& seriesName) const
{
  QHash<QString,Series>::const_iterator it = _series.constFind(seriesName);
  if( it == _series.constEnd() ) {
    return _nullSeries;
  }
  return it.value();
}

////// public ////////////////////////////////////////////////////////////////

bool SeriesStore::addToScales(const QString& seriesName)
{
  if( !_series.contains(seriesName) ) {
    return false;
  }
  const QString scaleName = _series[seriesName].unit();
  if( SimPlotTheme::isEmptyUnit(scaleName) ) {
    return true;
  }
  if( !_scales.contains(scaleName)  &&
      _scales.insert(scaleName, Scale(&_series)) == _scales.end() ) {
    return false;
  }
  if( !_scales[scaleName].insert(seriesName) ) {
    if( _scales[scaleName].isEmpty() ) {
      _scales.remove(scaleName);
    }
    return false;
  }
  return true;
}

void SeriesStore::removeFromScales(const QString& seriesName)
{
  if( !_series.contains(seriesName) ) {
    return;
  }
  const QString scaleName = _series[seriesName].unit();
  if( !_scales.contains(scaleName) ) {
    return;
  }
  _scales[scaleName].remove(seriesName);
  if( _scales[scaleName].isEmpty() ) {
    _scales.remove(scaleName);
  }
}

bool SeriesStore::isGroupedScale(const QString& seriesName, const QString& scaleName) const
{
  return _scales.contains(scaleName)  &&  _scales[scaleName].contains(seriesName);
}
