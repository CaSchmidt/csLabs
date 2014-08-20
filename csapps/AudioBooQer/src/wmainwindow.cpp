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

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>

#include <csQt/csQtUtil.h>

#include "wmainwindow.h"
#include "ui_wmainwindow.h"

#include "chapter.h"
#include "chaptermodel.h"
#include "wjobinfo.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui(new Ui::WMainWindow)
{
  ui->setupUi(this);

  // Chapters Model //////////////////////////////////////////////////////////

  ChapterModel *model = new ChapterModel(ui->chaptersView);
  ui->chaptersView->setModel(model);

  ui->chapterNoCheck->setChecked(model->showChapterNo());
  ui->firstChapterSpin->setValue(model->firstChaopterNo());
  ui->numberWidthSpin->setValue(model->widthChapterNo());

  ui->dissolveChapterAction->setShortcut(Qt::CTRL+Qt::Key_D);
  connect(ui->dissolveChapterAction, SIGNAL(triggered()),
          model, SLOT(dissolveLastChapter()));

  connect(ui->playerWidget, SIGNAL(fileNameChanged(const QString&)),
          model, SLOT(setPlayingFileName(const QString&)));

  connect(ui->chapterNoCheck, SIGNAL(toggled(bool)),
          model, SLOT(setShowChapterNo(bool)));
  connect(ui->firstChapterSpin, SIGNAL(valueChanged(int)),
          model, SLOT(setFirstChapterNo(int)));
  connect(ui->numberWidthSpin, SIGNAL(valueChanged(int)),
          model, SLOT(setWidthChapterNo(int)));

  // Buttons /////////////////////////////////////////////////////////////////

  connect(ui->goButton, SIGNAL(clicked()), SLOT(processJobs()));
  connect(ui->toolSoxButton,  SIGNAL(clicked()), SLOT(selectTool()));
  connect(ui->toolLameButton, SIGNAL(clicked()), SLOT(selectTool()));

  // File Menu ///////////////////////////////////////////////////////////////

  ui->openDirAction->setShortcut(QKeySequence::Open);
  connect(ui->openDirAction, SIGNAL(triggered()), SLOT(openDirectory()));

  ui->quitAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  connect(ui->quitAction, SIGNAL(triggered()), SLOT(close()));

  // Edit Menu ///////////////////////////////////////////////////////////////

  ui->newChapterAction->setShortcut(QKeySequence::New);
  connect(ui->newChapterAction, SIGNAL(triggered()), SLOT(createNewChapter()));

  // Playback Menu ///////////////////////////////////////////////////////////

  ui->previousAction->setShortcut(Qt::CTRL+Qt::Key_1);
  connect(ui->previousAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(previous()));

  ui->stopAction->setShortcut(Qt::CTRL+Qt::Key_2);
  connect(ui->stopAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(stop()));

  ui->playAction->setShortcut(Qt::CTRL+Qt::Key_3);
  connect(ui->playAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(play()));

  ui->nextAction->setShortcut(Qt::CTRL+Qt::Key_4);
  connect(ui->nextAction, SIGNAL(triggered()),
          ui->playerWidget, SLOT(next()));

  // Load Settings ///////////////////////////////////////////////////////////

  loadSettings();
}

WMainWindow::~WMainWindow()
{
  saveSettings();

  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::createNewChapter()
{
  QItemSelectionModel *selection = ui->chaptersView->selectionModel();
  const QModelIndexList indexes = selection->selectedIndexes();
  if( indexes.size() != 1 ) {
    return;
  }
  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model != 0 ) {
    const QModelIndex index = model->createNewChapter(indexes.front());
    if( index.isValid() ) {
      ui->chaptersView->expand(index);
    }
  }
}

void WMainWindow::openDirectory()
{
  const QString dirPath =
      QFileDialog::getExistingDirectory(this,
                                        tr("Open directory"),
                                        QDir::currentPath());
  if( dirPath.isEmpty() ) {
    return;
  }

  ui->playerWidget->reset();

  const QDir dir(dirPath);
  const QList<QFileInfo> wavFiles =
      dir.entryInfoList(QStringList() << _L1("*.wav"),
                        QDir::Files, QDir::Name);

  QStringList fileNames;
  foreach(const QFileInfo& fileInfo, wavFiles) {
    fileNames.push_back(fileInfo.absoluteFilePath());
  }

  ChapterRoot *root = new ChapterRoot();
  ChapterNode *sources = new ChapterNode(root, true);
  root->insert(sources);
  sources->setTitle(tr("<Files>"));
  sources->setFiles(fileNames);

  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model != 0 ) {
    model->setData(root);
  }

  ui->playerWidget->setFiles(fileNames);
  ui->chaptersView->expandAll();
  QDir::setCurrent(dirPath);
}

void WMainWindow::selectTool()
{
  QString caption;
  if(        sender() == ui->toolSoxButton ) {
    caption = tr("Select tool \"SoX\"");
  } else if( sender() == ui->toolLameButton ) {
    caption = tr("Select tool \"LAME\"");
  }

  const QString fileName =
      QFileDialog::getOpenFileName(this, caption);
  if( fileName.isEmpty() ) {
    return;
  }

  if(        sender() == ui->toolSoxButton ) {
    ui->toolSoxEdit->setText(fileName);
  } else if( sender() == ui->toolLameButton ) {
    ui->toolLameEdit->setText(fileName);
  }
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::loadSettings()
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);

  settings.beginGroup(_L1("tools"));
  ui->toolSoxEdit->setText(settings.value(_L1("sox")).toString());
  ui->toolLameEdit->setText(settings.value(_L1("lame")).toString());
  ui->lameOptionsEdit->setText(settings.value(_L1("lame_options")).toString());
  settings.endGroup();
}

void WMainWindow::saveSettings() const
{
  QSettings settings(settingsFileName(), QSettings::IniFormat);

  settings.beginGroup(_L1("tools"));
  settings.setValue(_L1("sox"), ui->toolSoxEdit->text());
  settings.setValue(_L1("lame"), ui->toolLameEdit->text());
  settings.setValue(_L1("lame_options"), ui->lameOptionsEdit->text());
  settings.endGroup();

  settings.sync();
}

void WMainWindow::processJobs()
{
  if(     ui->toolSoxEdit->text().isEmpty()
      ||  ui->toolLameEdit->text().isEmpty() ) {
    return;
  }

  ChapterModel *model = dynamic_cast<ChapterModel*>(ui->chaptersView->model());
  if( model == 0 ) {
    return;
  }

  Jobs jobs = model->buildJobs();
  if( jobs.size() < 1 ) {
    return;
  }

  ui->playerWidget->stop();

  for(int i = 0; i < jobs.size(); i++) {
    jobs[i].renameInput = ui->renameCheck->isChecked();

    jobs[i].soxExe      = ui->toolSoxEdit->text();
    jobs[i].lameExe     = ui->toolLameEdit->text();
    jobs[i].lameOptions = ui->lameOptionsEdit->text();
  }

  WJobInfo jobInfo(this);
  jobInfo.executeJobs(jobs);

  model->deleteJobs();
}

QString WMainWindow::settingsFileName()
{
  return QFileInfo(QDir(QApplication::applicationDirPath()),
                   _L1("AudioBooQer.ini")).absoluteFilePath();
}
