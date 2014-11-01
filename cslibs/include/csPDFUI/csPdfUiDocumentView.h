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

#ifndef __CSPDFUIDOCUMENTVIEW_H__
#define __CSPDFUIDOCUMENTVIEW_H__

#include <QtCore/QStack>
#include <QtWidgets/QGraphicsView>

#include <csPDFUI/cspdfui_config.h>
#include <csPDF/csPdfDocument.h>

class  CS_PDFUI_EXPORT csPdfUiDocumentView : public QGraphicsView {
  Q_OBJECT
public:
  enum ItemId {
    PageId = 1,
    HighlightId,
    SelectionId,
    LinkId,
    UserId = 1000
  };

  enum ItemLayer {
    PageLayer      =    0,
    HighlightLayer =   10,
    SelectionLayer =   20,
    LinkLayer      =   30,
    UserLayer      = 1000
  };

  csPdfUiDocumentView(QWidget *parent);
  ~csPdfUiDocumentView();

  QString selectedText() const;
  void setDocument(const csPdfDocument& doc);

  static void setItemId(QGraphicsItem *item, const int id);
  static int itemId(const QGraphicsItem *item);

public slots:
  void highlightText(const QString& text);
  void removeMarks();
  void showFirstPage();
  void showLastPage();
  void showNextPage();
  virtual void showPage(int no); // [1, _doc.pageCount()]
  void showPreviousPage();
  void zoom(double level); // [%]
  void zoomIn();
  void zoomOut();

protected:
  bool eventFilter(QObject *watched, QEvent *event);
  void keyPressEvent(QKeyEvent *event);
  QList<QGraphicsItem*> listItems(const int id) const;
  void removeItems(const int id);
  void wheelEvent(QWheelEvent *event);

private slots:
  void selectArea(QRect rect, QPointF fromScene, QPointF toScene);
  void watchVScroll(int value);

private:
  bool followLink(const QPointF& scenePos);
  void renderPage();

protected:
  QGraphicsScene *_scene;
  csPdfDocument _doc;
  csPdfPage _page;

private:
  double _zoom; // [%]
  int _pageBounces;
  QStack<int> _linkHistory; // [1, _doc.pageCount()]

signals:
  void pageChanged(int no); // [1, _doc.pageCount()]
  void zoomChanged(double zoom); // [%]
};

#endif // __CSPDFUIDOCUMENTVIEW_H__
