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

#include <QtWidgets/QFileDialog>

#include <csQt/csQtUtil.h>

#include "wmoduleeditor.h"
#include "ui_wmoduleeditor.h"

#include "global.h"

////// public ////////////////////////////////////////////////////////////////

WModuleEditor::WModuleEditor(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WModuleEditor)
{
  ui->setupUi(this);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->browseButton, &QPushButton::clicked, this, &WModuleEditor::browseModule);

  connect(ui->filenameEdit, &QLineEdit::textChanged, this, &WModuleEditor::validateModule);
  connect(ui->argumentsEdit, &QLineEdit::textEdited, this, &WModuleEditor::validateModule);

  // Default State ///////////////////////////////////////////////////////////

  validateModule(QString());
}

WModuleEditor::~WModuleEditor()
{
  delete ui;
}

SimModule WModuleEditor::get() const
{
  SimModule mod;
  mod.setFileName(ui->filenameEdit->text());
  mod.setArguments(ui->argumentsEdit->text());
  mod.setActive(ui->activeCheck->isChecked());
  return mod;
}

bool WModuleEditor::set(const SimModule& mod)
{
  if( !SimModule::isValidModule(mod.fileName()) ) {
    return false;
  }
  ui->filenameEdit->setText(mod.fileName());
  ui->argumentsEdit->setText(mod.arguments());
  ui->activeCheck->setChecked(mod.isActive());
  validateModule(QString());
  return true;
}

////// private slots /////////////////////////////////////////////////////////

void WModuleEditor::browseModule()
{
  const QString filename =
        QFileDialog::getOpenFileName(this,
                                     tr("Browse"),
                                     QString(),
                                     tr("Simulation modules (*.dll *.so)"));
  if( filename.isEmpty() ) {
    return;
  }
  ui->filenameEdit->setText(filename);
}

void WModuleEditor::validateModule(const QString&)
{
  bool valid(true);

  if( !SimModule::isValidModule(ui->filenameEdit->text())  ||
      !global::simctx->env.isValidModuleFileName(ui->filenameEdit->text()) ) {
    valid = false;
    ui->nameEdit->clear();
    csSetBackgroundColor(ui->nameEdit, Qt::yellow);
    csSetBackgroundColor(ui->filenameEdit, Qt::yellow);
  } else {
    ui->nameEdit->setText(SimModule::moduleName(ui->filenameEdit->text()));
    csSetBackgroundColor(ui->nameEdit, Qt::white);
    csSetBackgroundColor(ui->filenameEdit, Qt::white);
  }

  if( !SimModule::isValidArguments(ui->argumentsEdit->text()) ) {
    valid = false;
    csSetBackgroundColor(ui->argumentsEdit, Qt::yellow);
  } else {
    csSetBackgroundColor(ui->argumentsEdit, Qt::white);
  }

  ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(valid);
}
