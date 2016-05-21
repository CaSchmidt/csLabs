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

#include <SimCore/SimModulesModel.h>

#include "wmodules.h"
#include "ui_wmodules.h"

#include "global.h"
#include "wmoduleeditor.h"

////// public ////////////////////////////////////////////////////////////////

WModules::WModules(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WModules)
{
  ui->setupUi(this);

  // Modules Model ///////////////////////////////////////////////////////////

  ui->modulesView->setModel(new SimModulesModel(global::simctx, ui->modulesView));

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addModuleButton, &QPushButton::clicked, this, &WModules::addModule);
  connect(ui->modulesView, &QListView::activated, this, &WModules::editModule);
  connect(ui->removeModuleButton, &QPushButton::clicked, this, &WModules::removeModule);
}

WModules::~WModules()
{
  delete ui;
}

////// protected /////////////////////////////////////////////////////////////

void WModules::keyPressEvent(QKeyEvent *event)
{
  if(        event->key() == Qt::Key_Insert ) {
    addModule();
    event->accept();
  } else if( event->key() == Qt::Key_Delete ) {
    removeModule();
    event->accept();
  } else {
    QWidget::keyPressEvent(event);
  }
}

////// private slots /////////////////////////////////////////////////////////

void WModules::addModule()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  WModuleEditor editor(this);
  if( editor.exec() == QDialog::Accepted  &&
      !global::simctx->env.insertModule(editor.get()) ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Module insertion failed!"));
  }
}

void WModules::editModule(const QModelIndex& index)
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  const QString name = ui->modulesView->model()->data(index).toString();
  const SimModule oldMod = global::simctx->env.removeModule(name);
  WModuleEditor editor(this);
  if( !editor.set(oldMod) ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Invalid module!"));
    global::simctx->env.insertModule(oldMod);
    return;
  }
  const bool ok = editor.exec() == QDialog::Accepted
      ? global::simctx->env.insertModule(editor.get())
      : global::simctx->env.insertModule(oldMod);
  if( !ok ) {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Module insertion failed!"));
  }
}

void WModules::removeModule()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }
  const QModelIndexList indexes = ui->modulesView->selectionModel()->selectedRows();
  if( indexes.size() > 1  &&
      QMessageBox::question(this,
                            tr("Warning"),
                            tr("Remove multiple modules?"),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::No) != QMessageBox::Yes ) {
    return;
  }
  QStringList names;
  foreach(const QModelIndex& index, indexes) {
    names.push_back(ui->modulesView->model()->data(index).toString());
  }
  foreach(const QString& name, names) {
    global::simctx->env.removeModule(name);
  }
}
