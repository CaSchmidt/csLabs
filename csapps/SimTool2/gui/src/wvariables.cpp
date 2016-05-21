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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QMessageBox>

#include <SimCore/SimVariablesModel.h>

#include "wvariables.h"
#include "ui_wvariables.h"

#include "global.h"
#include "wvariableeditor.h"

////// public ////////////////////////////////////////////////////////////////

WVariables::WVariables(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WVariables)
{
  ui->setupUi(this);

  // Variables Model /////////////////////////////////////////////////////////

  _variablesModel = new SimVariablesModel(global::simctx, this);
  ui->variablesView->setModel(_variablesModel);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addVariableButton, &QPushButton::clicked, this, &WVariables::addVariable);
  connect(ui->removeVariableButton, &QPushButton::clicked, this, &WVariables::removeVariables);
  connect(ui->variablesView, &QListView::activated, this, &WVariables::editVariable);
}

WVariables::~WVariables()
{
  delete ui;
}

////// protected /////////////////////////////////////////////////////////////

void WVariables::keyPressEvent(QKeyEvent *event)
{
  if(        event->key() == Qt::Key_Insert ) {
    addVariable();
    event->accept();
  } else if( event->key() == Qt::Key_Delete ) {
    removeVariables();
    event->accept();
  } else {
    QWidget::keyPressEvent(event);
  }
}

////// private slots /////////////////////////////////////////////////////////

void WVariables::addVariable()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  WVariableEditor editor(this);
  if( editor.exec() == QDialog::Accepted  &&
      !global::simctx->env.insertVariable(editor.get()) ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Variable insertion failed!"));
  }
}

void WVariables::editVariable(const QModelIndex& index)
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  const QString name = ui->variablesView->model()->data(index).toString();
  const SimVariable oldVar = global::simctx->env.removeVariable(name);
  WVariableEditor editor(this);
  if( !editor.set(oldVar) ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unsupported variable type!"));
    global::simctx->env.insertVariable(oldVar);
    return;
  }
  const bool ok = editor.exec() == QDialog::Accepted
      ? global::simctx->env.insertVariable(editor.get())
      : global::simctx->env.insertVariable(oldVar);
  if( !ok ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Variable insertion failed!"));
  }
}

void WVariables::removeVariables()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  const QModelIndexList indexes = ui->variablesView->selectionModel()->selectedRows();
  if( indexes.size() > 1  &&
      QMessageBox::question(this,
                            tr("Warning"),
                            tr("Remove multiple variables?"),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::No) != QMessageBox::Yes ) {
    return;
  }
  QStringList names;
  foreach(const QModelIndex& index, indexes) {
    names.push_back(ui->variablesView->model()->data(index).toString());
  }
  foreach(const QString& name, names) {
    global::simctx->env.removeVariable(name);
  }
}
