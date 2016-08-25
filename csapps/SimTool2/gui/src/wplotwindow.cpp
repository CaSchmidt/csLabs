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

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>

#include "wplotwindow.h"
#include "ui_wplotwindow.h"

#include "datalogsmodel.h"
#include "global.h"

////// static private ////////////////////////////////////////////////////////

int WPlotWindow::_count(0);
QList<class WPlotWindow*> WPlotWindow::_windows;

////// public ////////////////////////////////////////////////////////////////

WPlotWindow::WPlotWindow(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WPlotWindow)
  , _id(++_count)
  , _model(0)
{
  ui->setupUi(this);

  setAcceptDrops(true);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowTitle(tr("Plot [%1]").arg(_id));

  _windows.push_back(this);

  ui->chartView->setFrameStyle(QFrame::StyledPanel);
  ui->chartView->setFrameShadow(QFrame::Sunken);

  QtCharts::QChart *chart = ui->chartView->chart();
  chart->setBackgroundRoundness(0);
  chart->setMargins(QMargins(0, 0, 0, 0));
  chart->legend()->setVisible(false);

  _model = new DataLogsModel(chart, global::simctx, this);
  ui->dataLogsView->setModel(_model);
}

WPlotWindow::~WPlotWindow()
{
  _windows.removeAll(this);
  delete ui;
}

void WPlotWindow::closeAll()
{
  while( !_windows.isEmpty() ) {
    delete _windows.takeFirst();
  }
  _count = 0;
}

////// protected /////////////////////////////////////////////////////////////

void WPlotWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if( event->proposedAction() == Qt::CopyAction  &&
      event->mimeData()->hasFormat(QStringLiteral(SIM_MIME_VARIABLES)) ) {
    event->acceptProposedAction();
  }
}

void WPlotWindow::dropEvent(QDropEvent *event)
{
  if( event->proposedAction() != Qt::CopyAction  ||
      !event->mimeData()->hasFormat(QStringLiteral(SIM_MIME_VARIABLES)) ) {
    return;
  }

  const QStringList names = QString::fromUtf8(event->mimeData()->data(QStringLiteral(SIM_MIME_VARIABLES)))
      .split(QChar::fromLatin1('\n'), QString::SkipEmptyParts);
  foreach(const QString& name, names) {
    _model->addVariable(name);
  }
  ui->dataLogsView->resizeColumnsToContents();
}
