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

#include "internal/IPlotImplementation.h"

////// public ////////////////////////////////////////////////////////////////

IPlotImplementation::IPlotImplementation(const SimPlot::DrawFlags drawFlags, const SimPlotTheme& theme)
  : _drawFlags(drawFlags)
  , _theme(theme)
{
}

IPlotImplementation::~IPlotImplementation()
{
}

SimPlot::DrawFlags IPlotImplementation::drawFlags() const
{
  return _drawFlags;
}

void IPlotImplementation::setDrawFlags(const SimPlot::DrawFlags flags)
{
  const SimPlot::DrawFlags oldFlags = _drawFlags;
  _drawFlags = flags & ~SimPlot::IsActive;
  if( oldFlags != _drawFlags ) {
    replot();
  }
}

void IPlotImplementation::setDrawFlag(const SimPlot::DrawFlag flag, const bool on)
{
  if( flag == SimPlot::IsActive ) {
    return;
  }
  const SimPlot::DrawFlags oldFlags = _drawFlags;
  _drawFlags.setFlag(flag, on);
  if( oldFlags != _drawFlags ) {
    replot();
  }
}

SimPlotTheme& IPlotImplementation::theme()
{
  return _theme;
}

const SimPlotTheme& IPlotImplementation::theme() const
{
  return _theme;
}
