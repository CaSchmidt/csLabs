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

#include <SimPlot/SimPlotSeriesHandle.h>

#include "internal/IPlotImplementation.h"
#include "internal/Series.h"

////// public ////////////////////////////////////////////////////////////////

SimPlotSeriesHandle::SimPlotSeriesHandle(const QString& name,
                                         IPlotImplementation *impl)
  : _impl(impl)
  , _name(name)
{
}

SimPlotSeriesHandle::~SimPlotSeriesHandle()
{
}

bool SimPlotSeriesHandle::isValid() const
{
  return
      !_name.isEmpty()  &&
      _impl != 0        &&
      !_impl->series(_name).isEmpty();
}

bool SimPlotSeriesHandle::activate()
{
  if( !isValid() ) {
    return false;
  }
  return _impl->setActiveSeries(_name);
}

bool SimPlotSeriesHandle::remove()
{
  if( !isValid() ) {
    return false;
  }
  const bool result = _impl->remove(_name);
  _name.clear();
  _impl = 0;
  return result;
}

QString SimPlotSeriesHandle::name() const
{
  if( !isValid() ) {
    return QString();
  }
  return _impl->series(_name).name();
}

QString SimPlotSeriesHandle::unit() const
{
  if( !isValid() ) {
    return QString();
  }
  return _impl->series(_name).unit();
}

QColor SimPlotSeriesHandle::color() const
{
  if( !isValid() ) {
    return QColor();
  }
  return _impl->series(_name).color();
}

void SimPlotSeriesHandle::setColor(const QColor& color, const bool replot)
{
  if( !isValid()  ||  !color.isValid() ) {
    return;
  }
  Series& series = _impl->series(_name);
  const QColor oldColor = series.color();
  series.setColor(color);
  if( replot  &&  oldColor != series.color() ) {
    _impl->replot();
  }
}

void SimPlotSeriesHandle::replot()
{
  if( !isValid() ) {
    return;
  }
  _impl->replot();
}

const ISimPlotSeriesData *SimPlotSeriesHandle::constData() const
{
  if( !isValid() ) {
    return 0;
  }
  return _impl->series(_name).constData();
}

ISimPlotSeriesData *SimPlotSeriesHandle::data()
{
  if( !isValid() ) {
    return 0;
  }
  return _impl->series(_name).data();
}
