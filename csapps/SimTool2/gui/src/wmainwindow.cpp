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

#include <QtCore/QThread>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "wmainwindow.h"
#include "ui_wmainwindow.h"

#include "config.h"
#include "global.h"
#include "wplotwindow.h"
#include "wsimconfigbar.h"
#include "wvalueswindow.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui(new Ui::WMainWindow)
  , _configBar(0)
  , _currentFilename()
  , _simThread(0)
{
  // Create SimContext so it is Available During UI Initialization ///////////

  _simThread = new QThread(this);

  global::simctx = new SimContext(0);
  global::simctx->moveToThread(_simThread);

  // Initialize UI ///////////////////////////////////////////////////////////

  ui->setupUi(this);

  setAttribute(Qt::WA_DeleteOnClose, true);

  // Configuration ///////////////////////////////////////////////////////////

  _configBar = new WSimConfigBar(this);
  addToolBar(_configBar);

  // Context - Signals & Slots ///////////////////////////////////////////////

  // Logging

  connect(global::simctx, QOverload<const QString&>::of(&SimContext::errorLogged),
          ui->logBrowser, QOverload<const QString&>::of(&WLogger::logError));
  connect(global::simctx, QOverload<int,const QString&>::of(&SimContext::errorLogged),
          ui->logBrowser, QOverload<int,const QString&>::of(&WLogger::logError));
  connect(global::simctx, QOverload<const QString&>::of(&SimContext::warningLogged),
          ui->logBrowser, QOverload<const QString&>::of(&WLogger::logWarning));
  connect(global::simctx, QOverload<int,const QString&>::of(&SimContext::warningLogged),
          ui->logBrowser, QOverload<int,const QString&>::of(&WLogger::logWarning));
  connect(global::simctx, QOverload<const QString&>::of(&SimContext::textLogged),
          ui->logBrowser, QOverload<const QString&>::of(&WLogger::logText));

  // Config

  global::simctx->setConfig(_configBar->get());
  connect(global::simctx, &SimContext::configChanged,
          _configBar, &WSimConfigBar::set);
  connect(_configBar, &WSimConfigBar::configChanged,
          global::simctx, &SimContext::setConfig);

  // Control - State Tracking

  connect(&global::simctx->ctrl, &SimControl::stateEntered,
          this, &WMainWindow::enterSimState);
  connect(&global::simctx->ctrl, &SimControl::stateExited,
          this, &WMainWindow::exitState);

  // Control - Changing States

  connect(ui->simStartAction, &QAction::triggered,
          &global::simctx->ctrl, &SimControl::startRequested);
  connect(ui->simStopAction, &QAction::triggered,
          &global::simctx->ctrl, &SimControl::stopRequested);
  connect(ui->simPauseAction, &QAction::triggered,
          &global::simctx->ctrl, &SimControl::pauseRequested);

  _simThread->start();

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->openEnvAction, &QAction::triggered, this, &WMainWindow::open);
  connect(ui->saveEnvAction, &QAction::triggered, this, &WMainWindow::save);
  connect(ui->saveAsEnvAction, &QAction::triggered, this, &WMainWindow::saveAs);
  connect(ui->quitAction, &QAction::triggered, this, &WMainWindow::close);

  connect(ui->newPlotWindowAction, &QAction::triggered,
          this, &WMainWindow::newPlotWindow);
  connect(ui->newValuesWindowAction, &QAction::triggered,
          this, &WMainWindow::newValuesWindow);

  // Default State ///////////////////////////////////////////////////////////

  enterSimState(global::simctx->sim.activeState());
}

WMainWindow::~WMainWindow()
{
  WPlotWindow::closeAll();
  WValuesWindow::closeAll();

  global::simctx->deleteLater();

  _simThread->quit();
  _simThread->wait();
  delete _simThread;

  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::enterSimState(int state)
{
  // Control Actions /////////////////////////////////////////////////////////

  ui->simStartAction->setEnabled(false);
  ui->simStopAction->setEnabled(false);
  ui->simPauseAction->setEnabled(false);

  if(        state == IdleState ) {
    ui->simStartAction->setEnabled(true);
  } else if( state == StepState ) {
    ui->simStopAction->setEnabled(true);
    ui->simPauseAction->setEnabled(true);
  } else if( state == PauseState ) {
    ui->simStartAction->setEnabled(true);
    ui->simStopAction->setEnabled(true);
  }

  // Simulation Configuration ////////////////////////////////////////////////

  if( state == IdleState ) {
    _configBar->setEnabled(true);
  } else {
    _configBar->setEnabled(false);
  }
}

void WMainWindow::exitState(int state)
{
  if( state == IdleState ) {
    ui->logBrowser->clear();
  }
}

void WMainWindow::open()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }

  const QString filename =
      QFileDialog::getOpenFileName(this,
                                   tr("Open"),
                                   QFileInfo(_currentFilename).canonicalPath(),
                                   tr("Simulation environments (*.json)"));
  if( filename.isEmpty() ) {
    return;
  }

  openEnvironment(filename);
}

void WMainWindow::save()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }

  if( _currentFilename.isEmpty() ) {
    saveAs();
  } else {
    saveEnvironment(_currentFilename);
  }
}

void WMainWindow::saveAs()
{
  if( global::simctx->sim.activeState() != IdleState ) {
    return;
  }

  const QString filename =
      QFileDialog::getSaveFileName(this,
                                   tr("Save as"),
                                   QFileInfo(_currentFilename).canonicalPath(),
                                   tr("Simulation environments (*.json)"));
  if( filename.isEmpty() ) {
    return;
  }

  saveEnvironment(filename);
}

void WMainWindow::newPlotWindow()
{
  WPlotWindow *window = new WPlotWindow(0);
  window->show();
}

void WMainWindow::newValuesWindow()
{
  WValuesWindow *window = new WValuesWindow(0);
  window->show();
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::openEnvironment(const QString& filename)
{
  WPlotWindow::closeAll();
  WValuesWindow::closeAll();

  if( !global::simctx->open(filename)  ||  !loadConfig(filename, this) ) {
    _currentFilename.clear();
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to open environment!"));
  } else {
    _currentFilename = filename;
  }
}

void WMainWindow::saveEnvironment(const QString& filename)
{
  if( !global::simctx->save(filename)  ||  !storeConfig(filename, this) ) {
    _currentFilename.clear();
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to save environment!"));
  } else {
    _currentFilename = filename;
  }
}
