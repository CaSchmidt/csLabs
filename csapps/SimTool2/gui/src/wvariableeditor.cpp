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

#include <limits>

#include <QtGui/QDoubleValidator>
#include <QtWidgets/QPushButton>

#include <csQt/csQtUtil.h>

#include <SimCore/Sim.h>
#include <SimCore/SimUtil.h>

#include "wvariableeditor.h"
#include "ui_wvariableeditor.h"

#include "global.h"

////// public ////////////////////////////////////////////////////////////////

WVariableEditor::WVariableEditor(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WVariableEditor)
  , _validator(0)
{
  ui->setupUi(this);

  // Type ////////////////////////////////////////////////////////////////////

  ui->typeCombo->clear();
  ui->typeCombo->addItem(tr("Double"), (int)DoubleType);
  ui->typeCombo->addItem(tr("Single"), (int)SingleType);
  ui->typeCombo->addItem(tr("Int"), (int)IntType);
  ui->typeCombo->addItem(tr("UInt"), (int)UIntType);

  // Init ////////////////////////////////////////////////////////////////////

  _validator = new QDoubleValidator(this);
  _validator->setLocale(sim::locale());
  _validator->setBottom(std::numeric_limits<double>::lowest());
  _validator->setTop(std::numeric_limits<double>::max());
  _validator->setDecimals(3);
  ui->initEdit->setValidator(_validator);
  ui->initEdit->setText(_validator->locale().toString(0.0,
                                                      'f',
                                                      _validator->decimals()));

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->nameEdit, &QLineEdit::textEdited, this, &WVariableEditor::validateName);

  // Default State ///////////////////////////////////////////////////////////

  validateName();
}

WVariableEditor::~WVariableEditor()
{
  delete ui;
}

SimVariable WVariableEditor::get() const
{
  SimVariable var;
  var.setName(ui->nameEdit->text());
  var.setType(type());
  var.setUnit(ui->unitEdit->text());
  var.setInitialValue(initialValue());
  return var;
}

bool WVariableEditor::set(const SimVariable& var)
{
  const int index = ui->typeCombo->findData((int)var.type());
  if( index < 0 ) {
    return false;
  }
  ui->nameEdit->setText(var.name());
  ui->typeCombo->setCurrentIndex(index);
  ui->unitEdit->setText(var.unit());
  ui->initEdit->setText(_validator->locale().toString(var.initialValue(),
                                                      'f',
                                                      _validator->decimals()));
  validateName();
  return true;
}

////// private slots /////////////////////////////////////////////////////////

void WVariableEditor::validateName(const QString&)
{
  if( global::simctx->env.isValidVariableName(ui->nameEdit->text()) ) {
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
    csSetBackgroundColor(ui->nameEdit, Qt::white);
  } else {
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    csSetBackgroundColor(ui->nameEdit, Qt::yellow);
  }
}

////// private ///////////////////////////////////////////////////////////////

double WVariableEditor::initialValue() const
{
  const double init = _validator->locale().toDouble(ui->initEdit->text());
  return sim::bound(init, type());
}

SimDataType WVariableEditor::type() const
{
  const int index = ui->typeCombo->currentIndex();
  return (SimDataType)ui->typeCombo->itemData(index).toInt();
}
