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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>

#include <SimCore/SimValuesModel.h>

#include "wvalueswindow.h"
#include "ui_wvalueswindow.h"

#include "config.h"
#include "global.h"
#include "valuedelegate.h"

////// static private ////////////////////////////////////////////////////////

int WValuesWindow::_count(0);
QList<class WValuesWindow*> WValuesWindow::_windows;

////// public ////////////////////////////////////////////////////////////////

WValuesWindow::WValuesWindow(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WValuesWindow)
  , _id(++_count)
  , _model(0)
{
  ui->setupUi(this);

  setAcceptDrops(true);
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowTitle(tr("Values [%1]").arg(_id));

  _windows.push_back(this);

  _model = new SimValuesModel(global::simctx, ui->valuesView);
  ui->valuesView->setModel(_model);

  ui->valuesView->setItemDelegateForColumn(1, new ValueDelegate(this));
}

WValuesWindow::~WValuesWindow()
{
  _windows.removeAll(this);
  delete ui;
}

void WValuesWindow::closeAll()
{
  while( !_windows.isEmpty() ) {
    delete _windows.takeFirst();
  }
  _count = 0;
}

void WValuesWindow::loadConfig(const QJsonObject& cfgObj)
{
  const QJsonArray valsArr = cfgObj[QStringLiteral("valuesWindows")].toArray();
  foreach(const QJsonValue& value, valsArr) {
    const QJsonObject winObj = value.toObject();
    if( winObj.isEmpty() ) {
      continue;
    }

    WValuesWindow *w = new WValuesWindow(0);
    loadGeometry(w, winObj);

    const QJsonArray nameArr = winObj[QStringLiteral("variables")].toArray();
    foreach(const QJsonValue& value, nameArr) {
      w->_model->addValue(value.toString());
    }

    w->ui->valuesView->resizeColumnsToContents();
    w->show();
  } // For Each Window
}

void WValuesWindow::storeConfig(QJsonObject& cfgObj)
{
  QJsonArray valsArr;
  foreach(WValuesWindow *w, _windows) {
    QJsonObject winObj;
    storeGeometry(winObj, w);

    QJsonArray nameArr;
    for(int i = 0; i < w->_model->rowCount(); i++) {
      const QModelIndex index = w->_model->index(i, 0);
      const QString name = w->_model->data(index, Qt::DisplayRole).toString();
      nameArr.append(name);
    }
    winObj[QStringLiteral("variables")] = nameArr;

    valsArr.append(winObj);
  } // For Each Window
  cfgObj[QStringLiteral("valuesWindows")] = valsArr;
}

////// protected /////////////////////////////////////////////////////////////

void WValuesWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if( event->proposedAction() == Qt::CopyAction  &&
      event->mimeData()->hasFormat(QStringLiteral(SIM_MIME_VARIABLES)) ) {
    event->acceptProposedAction();
  }
}

void WValuesWindow::dropEvent(QDropEvent *event)
{
  if( event->proposedAction() != Qt::CopyAction  ||
      !event->mimeData()->hasFormat(QStringLiteral(SIM_MIME_VARIABLES)) ) {
    return;
  }

  const QStringList names = QString::fromUtf8(event->mimeData()->data(QStringLiteral(SIM_MIME_VARIABLES)))
      .split(QChar::fromLatin1('\n'), QString::SkipEmptyParts);
  foreach(const QString& name, names) {
    _model->addValue(name);
  }
  ui->valuesView->resizeColumnsToContents();
}
