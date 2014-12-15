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

#include <csPDF/csPdfDocument.h>
#include <csPDF/csPdfContentsModel.h>

#include <csPDFUI/csPdfUiTocWidget.h>
#include "ui_csPdfUiTocWidget.h"

////// public ////////////////////////////////////////////////////////////////

csPdfUiTocWidget::csPdfUiTocWidget(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::csPdfUiTocWidget)
  , _contentsModel(0)
{
  ui->setupUi(this);

  // Contents Model //////////////////////////////////////////////////////////

  _contentsModel = new csPdfContentsModel(ui->contentsView);
  ui->contentsView->setModel(_contentsModel);

  connect(ui->contentsView, SIGNAL(activated(const QModelIndex&)),
          SLOT(activateTocItem(const QModelIndex&)));

  // Filter //////////////////////////////////////////////////////////////////

  connect(ui->filterEdit, SIGNAL(textChanged(const QString&)),
          _contentsModel, SLOT(filter(const QString&)));
}

csPdfUiTocWidget::~csPdfUiTocWidget()
{
  delete ui;
}

void csPdfUiTocWidget::setDocument(const class csPdfDocument& doc)
{
  ui->filterEdit->clear();
  _contentsModel->setRootNode(doc.tableOfContents());
}

////// private slots /////////////////////////////////////////////////////////

void csPdfUiTocWidget::activateTocItem(const QModelIndex& index)
{
  csPdfContentsNode *node = static_cast<csPdfContentsNode*>(index.internalPointer());
  if( node != 0  &&  node->link().isGoto() ) {
    emit pageRequested(node->link().page()+1);
  }
}