/****************************************************************************
** Copyright (c) 2013-2014, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QMimeData>
#include <QtCore/QThread>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QActionGroup>
#include <QtWidgets/QFileDialog>

#include <csPDF/csPdfDocument.h>
#include <csPDF/csPdfContentsModel.h>

#include "wmainwindow.h"
#include "ui_wmainwindow.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags),
    ui(new Ui::WMainWindow)
{
  ui->setupUi(this);

  // Drag & Drop /////////////////////////////////////////////////////////////

  setAcceptDrops(true);
  ui->pdfView->setAcceptDrops(true);
  ui->pdfView->installEventFilter(this);

  // Document View ///////////////////////////////////////////////////////////

  connect(ui->pdfView, SIGNAL(pageChanged(int)),
          ui->searchWidget, SLOT(setStartPage(int)));

  // Quick Search ////////////////////////////////////////////////////////////

  ui->pdfView->installEventFilter(ui->quickSearchWidget);
  ui->quickSearchWidget->hide();

  connect(ui->quickSearchWidget, SIGNAL(searchTextEdited(const QString&)),
          ui->pdfView, SLOT(highlightText(const QString&)));

  // Table of Contents ///////////////////////////////////////////////////////

  connect(ui->contentsWidget, SIGNAL(pageRequested(int)),
          ui->pdfView, SLOT(showPage(int)));

  connect(ui->showTocAction, SIGNAL(triggered()),
          ui->contentsDock, SLOT(show()));

  // Document Search /////////////////////////////////////////////////////////

  connect(ui->showSearchAction, SIGNAL(triggered()),
          ui->searchDock, SLOT(show()));
  connect(ui->showSearchAction, SIGNAL(triggered()),
          ui->searchWidget, SLOT(selectSearchText()));
  connect(ui->searchWidget, SIGNAL(pageRequested(int)),
          ui->pdfView, SLOT(showPage(int)));
  connect(ui->searchWidget, SIGNAL(highlightRequested(const QString&)),
          ui->pdfView, SLOT(highlightText(const QString&)));

  // Edit Mode ///////////////////////////////////////////////////////////////

  QActionGroup *modeGroup = new QActionGroup(this);
  modeGroup->addAction(ui->handToolAction);
  modeGroup->addAction(ui->rectangleToolAction);
  ui->handToolAction->setChecked(true);
  setEditMode(false);

  connect(ui->handToolAction, SIGNAL(triggered(bool)),
          SLOT(setEditMode(bool)));
  connect(ui->rectangleToolAction, SIGNAL(triggered(bool)),
          SLOT(setEditMode(bool)));


  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->openAction, SIGNAL(triggered()), SLOT(openFile()));
  connect(ui->quitAction, SIGNAL(triggered()), SLOT(close()));

  connect(ui->copyAction, SIGNAL(triggered()), SLOT(copySelection()));
}

WMainWindow::~WMainWindow()
{
  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::copySelection()
{
  const QString s = ui->pdfView->selectedText();
  if( s.isEmpty() ) {
    QApplication::clipboard()->clear();
    return;
  }

  QApplication::clipboard()->setText(s);
}

void WMainWindow::openFile()
{
  const QString filename =
      QFileDialog::getOpenFileName(this,
                                   tr("Open"),
                                   QString(),
                                   tr("Adobe PDF files (*.pdf)"));
  if( filename.isEmpty() ) {
    return;
  }

  openFile(filename);
}

void WMainWindow::setEditMode(bool)
{
  if( ui->handToolAction->isChecked() ) {
    ui->pdfView->setDragMode(QGraphicsView::ScrollHandDrag);
  } else if( ui->rectangleToolAction->isChecked() ) {
    ui->pdfView->setDragMode(QGraphicsView::RubberBandDrag);
  }
}

////// protected /////////////////////////////////////////////////////////////

void WMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if( event->mimeData()->hasUrls()  &&
      event->mimeData()->urls().size() == 1  &&
      event->mimeData()->urls().front().toLocalFile().endsWith(".pdf", Qt::CaseInsensitive) ) {
    event->setDropAction(Qt::CopyAction);
    event->acceptProposedAction();
  }
}

void WMainWindow::dropEvent(QDropEvent *event)
{
  openFile(event->mimeData()->urls().front().toLocalFile());
  event->acceptProposedAction();
}

bool WMainWindow::eventFilter(QObject *watched, QEvent *event)
{
  if( watched == ui->pdfView ) {
    if(        event->type() == QEvent::DragEnter ) {
      dragEnterEvent(dynamic_cast<QDragEnterEvent*>(event));
      return event->isAccepted();
    } else if( event->type() == QEvent::Drop ) {
      dropEvent(dynamic_cast<QDropEvent*>(event));
      return event->isAccepted();
    }
  }

  return QMainWindow::eventFilter(watched, event);
}

void WMainWindow::keyPressEvent(QKeyEvent *event)
{
  if( event->key() == Qt::Key_Escape ) {
    if( ui->quickSearchWidget->isVisible() ) {
      ui->quickSearchWidget->hide();
    } else {
      ui->pdfView->removeMarks();
    }
    // setFocus(Qt::OtherFocusReason);
    event->accept();
  }
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::openFile(const QString& filename)
{
  csPdfDocument doc = csPdfDocument::load(filename);
  doc.setRenderThreads(QThread::idealThreadCount());
  ui->pdfView->setDocument(doc);
  ui->contentsWidget->setDocument(doc);
  ui->searchWidget->setDocument(doc);
}
