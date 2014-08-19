/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
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

#include <QtConcurrent/QtConcurrent>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeySequence>

#include "wjobinfo.h"
#include "ui_wjobinfo.h"

////// public ////////////////////////////////////////////////////////////////

WJobInfo::WJobInfo(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui(new Ui::WJobInfo)
  , watcher(0)
{
  ui->setupUi(this);

  // Dialog's Appearance /////////////////////////////////////////////////////

  setModal(true);

  Qt::WindowFlags flags = windowFlags();
  flags &= ~Qt::WindowCloseButtonHint;
  flags &= ~Qt::WindowContextHelpButtonHint;
  setWindowFlags(flags);

  // Close Button ////////////////////////////////////////////////////////////

  ui->closeButton->setEnabled(false);
  connect(ui->closeButton, SIGNAL(clicked()), SLOT(close()));

  // Future Watcher //////////////////////////////////////////////////////////

  watcher = new QFutureWatcher<QByteArray>(this);

  connect(watcher, SIGNAL(finished()), SLOT(enableClose()));
  connect(watcher, SIGNAL(resultReadyAt(int)), SLOT(readResult(int)));
  connect(watcher, SIGNAL(progressRangeChanged(int,int)),
          SLOT(setProgressRange(int,int)));
  connect(watcher, SIGNAL(progressValueChanged(int)),
          SLOT(setProgressValue(int)));
}

WJobInfo::~WJobInfo()
{
  delete ui;
}

void WJobInfo::executeJobs(const Jobs& jobs)
{
  watcher->setFuture(QtConcurrent::mapped(jobs, executeJob));

  ui->progressBar->setMinimum(watcher->progressMinimum());
  ui->progressBar->setMaximum(watcher->progressMaximum());
  ui->progressBar->setValue(watcher->progressValue());

  exec();
}

////// protected /////////////////////////////////////////////////////////////

void WJobInfo::keyPressEvent(QKeyEvent *event)
{
  if( watcher->isRunning() ) {
    event->ignore();
    return;
  }
  QDialog::keyPressEvent(event);
}

////// private slots /////////////////////////////////////////////////////////

void WJobInfo::accept()
{
  QDialog::accept();
}

void WJobInfo::done(int r)
{
  QDialog::done(r);
}

int WJobInfo::exec()
{
  return QDialog::exec();
}

void WJobInfo::open()
{
  QDialog::open();
}

void WJobInfo::reject()
{
  QDialog::reject();
}

void WJobInfo::enableClose()
{
#if 0
  Qt::WindowFlags flags = windowFlags();
  flags |= Qt::WindowCloseButtonHint;
  setWindowFlags(flags);
#endif

  ui->closeButton->setEnabled(true);
}

void WJobInfo::readResult(int index)
{
  const QString result = QString::fromLocal8Bit(watcher->resultAt(index));
  ui->outputBrowser->append(result);
}

void WJobInfo::setProgressRange(int min, int max)
{
  ui->progressBar->setMinimum(min);
  ui->progressBar->setMaximum(max);
}

void WJobInfo::setProgressValue(int val)
{
  ui->progressBar->setValue(val);
}
