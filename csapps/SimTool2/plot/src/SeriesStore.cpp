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

#include "internal/SeriesStore.h"

////// Macros ////////////////////////////////////////////////////////////////

#define _VAL(key)  (*this)[key]

////// private static ////////////////////////////////////////////////////////

Series SeriesStore::_nullSeries;

////// public ////////////////////////////////////////////////////////////////

SeriesStore::SeriesStore()
  : QHash<QString,Series>()
  , _scales()
{
}

SeriesStore::~SeriesStore()
{
}

bool SeriesStore::insert(const Series& series)
{
  if( contains(series.name()) ) {
    return false;
  }
  if( QHash<QString,Series>::insert(series.name(), series) == end() ) {
    return false;
  }
  if( !addToScales(series.name()) ) {
    QHash<QString,Series>::remove(series.name());
    return false;
  }
  return true;
}

bool SeriesStore::remove(const QString& seriesName)
{
  if( !contains(seriesName) ) {
    return false;
  }
  removeFromScales(seriesName);
  QHash<QString,Series>::remove(seriesName);
  return true;
}

SimPlotRange SeriesStore::rangeX(const QString& seriesName) const
{
  SimPlotRange result;

  if( !contains(seriesName) ) {
    return result;
  }

  result = _scales[_VAL(seriesName).scale()].rangeX();

  return result;
}

SimPlotRange SeriesStore::rangeY(const QString& seriesName) const
{
  SimPlotRange result;

  if( !contains(seriesName) ) {
    return result;
  }

  result = _scales[_VAL(seriesName).scale()].rangeY();

  return result;
}

QString SeriesStore::titleString(const QString& seriesName) const
{
  QString result;

  if( !contains(seriesName) ) {
    return result;
  }

  const QString unit = _VAL(seriesName).unit();
  result = QString(QStringLiteral("%1 [%2]"))
      .arg(seriesName)
      .arg(unit.isEmpty()
           ? QStringLiteral("-")
           : unit);

  return result;
}

SimPlotRange SeriesStore::totalRangeX() const
{
  SimPlotRange result;

  for(const Scale& scale : _scales) {
    result.update(scale.rangeX());
  }

  return result;
}

Series& SeriesStore::series(const QString& seriesName)
{
  QHash<QString,Series>::iterator it = find(seriesName);
  if( it == end() ) {
    return _nullSeries;
  }
  return it.value();
}

const Series& SeriesStore::constSeries(const QString& seriesName) const
{
  QHash<QString,Series>::const_iterator it = constFind(seriesName);
  if( it == constEnd() ) {
    return _nullSeries;
  }
  return it.value();
}

////// public ////////////////////////////////////////////////////////////////

bool SeriesStore::addToScales(const QString& seriesName)
{
  if( !contains(seriesName) ) {
    return false;
  }
  const QString scaleName = _VAL(seriesName).scale();
  if( !_scales.contains(scaleName)  &&
      _scales.insert(scaleName, Scale(this)) == _scales.end() ) {
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

bool SeriesStore::removeFromScales(const QString& seriesName)
{
  if( !contains(seriesName) ) {
    return false;
  }
  const QString scaleName = _VAL(seriesName).scale();
  if( !_scales.contains(scaleName) ) {
    return false;
  }
  _scales[scaleName].remove(seriesName);
  if( _scales[scaleName].isEmpty() ) {
    _scales.remove(scaleName);
  }
  return true;
}
