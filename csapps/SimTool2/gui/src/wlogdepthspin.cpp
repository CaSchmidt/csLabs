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

#include <QtWidgets/QLineEdit>

#include "wlogdepthspin.h"

#include <SimCore/SimConfig.h>
#include <SimCore/SimDataSeries.h>
#include <SimCore/SimUtil.h>

////// public ////////////////////////////////////////////////////////////////

WLogDepthSpin::WLogDepthSpin(QWidget *parent)
  : QSpinBox(parent)
  , _simStep_ms(SimConfig().stepMS())
{
  setSingleStep(1);
  setMinimum(SimDataSeries::minDepth());
  setMaximum(SimDataSeries::maxDepth());
  lineEdit()->setReadOnly(true);
  setSuffix(tr("s"));
}

WLogDepthSpin::~WLogDepthSpin()
{
}

void WLogDepthSpin::setSimStep(int simStep_ms)
{
  _simStep_ms = simStep_ms;

  blockSignals(true);
  setValue(value());
  blockSignals(false);
}

////// protected /////////////////////////////////////////////////////////////

QString WLogDepthSpin::textFromValue(int val) const
{
  const int size = SimDataSeries::calculateSize(val);
  return sim::locale().toString(double(size*_simStep_ms)/1000.0, 'f', 3);
}
