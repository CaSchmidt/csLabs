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

#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>

#include <csQt/csMultiToolButton.h>

#include <SimCore/Sim.h>

#include "wsimconfigbar.h"

#include "global.h"

////// Macros ////////////////////////////////////////////////////////////////

#define DEFAULT_STEP      10 // [ms]
#define DEFAULT_DURATION  10 // [s]

////// public ////////////////////////////////////////////////////////////////

WSimConfigBar::WSimConfigBar(QWidget *parent)
  : QToolBar(parent)
  , _durationSpin(0)
  , _modeButton(0)
  , _stepSpin(0)
{
  setWindowTitle(tr("Configuration"));
  setAllowedAreas(Qt::TopToolBarArea);

  // Operation Mode //////////////////////////////////////////////////////////

  csMultiItems modeItems;
  modeItems.push_back(csMultiItem(tr("Real-Time"), RealTimeMode));
  modeItems.push_back(csMultiItem(tr("Offline"), OfflineMode));

  _modeButton = new csMultiToolButton(modeItems, InvalidDataType, this);
  addWidget(_modeButton);

  // Step Size ///////////////////////////////////////////////////////////////

  addSeparator();
  addWidget(new QLabel(tr("Step:"), this));

  _stepSpin = new QSpinBox(this);
  _stepSpin->setSizePolicy(_stepSpin->sizePolicy().horizontalPolicy(),
                           QSizePolicy::Expanding);
  _stepSpin->setMinimum(1);
  _stepSpin->setMaximum(1000);
  _stepSpin->setValue(DEFAULT_STEP);
  _stepSpin->setSuffix(tr("ms"));
  addWidget(_stepSpin);

  // Duration ////////////////////////////////////////////////////////////////

  addSeparator();
  addWidget(new QLabel(tr("Duration:"), this));

  _durationSpin = new QSpinBox(this);
  _durationSpin->setSizePolicy(_durationSpin->sizePolicy().horizontalPolicy(),
                               QSizePolicy::Expanding);
  _durationSpin->setMinimum(1);
  _durationSpin->setMaximum(1000000);
  _durationSpin->setValue(DEFAULT_DURATION);
  _durationSpin->setSuffix(tr("s"));
  addWidget(_durationSpin);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(_modeButton, &csMultiToolButton::selectionChanged, this, &WSimConfigBar::setMode);
  connect(_stepSpin, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged(int)));
  connect(_durationSpin, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged(int)));

  // Default State ///////////////////////////////////////////////////////////

  setMode(_modeButton->currentSelection()); // NOTE: Calls writeConfig()!
}

WSimConfigBar::~WSimConfigBar()
{
}

SimConfig WSimConfigBar::get() const
{
  return SimConfig((SimOperationMode)_modeButton->currentSelection(),
                   double(_stepSpin->value())/1000.0,
                   double(_durationSpin->value()));
}

////// public slots //////////////////////////////////////////////////////////

void WSimConfigBar::set(const SimConfig& config)
{
  blockSignals(true);
  _modeButton->setCurrentSelection(config.mode);
  setMode(config.mode);
  _stepSpin->setValue(config.step*1000.0);
  _durationSpin->setValue(config.duration);
  blockSignals(false);
}

////// private slots /////////////////////////////////////////////////////////

void WSimConfigBar::setMode(int id)
{
  if( id == OfflineMode ) {
    _durationSpin->setEnabled(true);
  } else {
    _durationSpin->setEnabled(false);
  }
  emit configChanged(get());
}

void WSimConfigBar::valuesChanged(int)
{
  emit configChanged(get());
}
