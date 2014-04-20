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

#ifndef __WPDFVIEW_H__
#define __WPDFVIEW_H__

#include <QtWidgets/QGraphicsView>

#include <csPDF/csPdfDocument.h>

class WPdfView : public QGraphicsView {
  Q_OBJECT
public:
  WPdfView(QWidget *parent);
  ~WPdfView();

  QString selectedText() const;
  void setDocument(const csPdfDocument& doc);

public slots:
  void highlightText(const QString& text);
  void removeMarks();
  void showFirstPage();
  void showLastPage();
  void showNextPage();
  void showPage(int no); // [1, _doc.pageCount()]
  void showPreviousPage();
  void zoom(double level); // [%]
  void zoomIn();
  void zoomOut();

protected:
  bool eventFilter(QObject *watched, QEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent *event);

private slots:
  void selectArea(QRect rect, QPointF fromScene, QPointF toScene);
  void watchVScroll(int value);

private:
  bool followLink(const QPointF& scenePos);
  void removeItems(const int id);
  void renderPage();

  QGraphicsScene *_scene;
  csPdfDocument _doc;
  csPdfPage _page;
  double _zoom; // [%]
  int _pageBounces;

signals:
  void pageChanged(int no); // [1, _doc.pageCount()]
  void zoomChanged(double zoom); // [%]
};

#endif // __WPDFVIEW_H__
