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

#include <QtCore/QCoreApplication>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QScrollBar>

#include <csPDF/csPdfUtil.h>

#include "wpdfview.h"

////// Macros ////////////////////////////////////////////////////////////////

// Item Data
#define DATA_ID             0
#define DATA_LINKPAGE       1
#define DATA_LINKDEST       2
#define DATA_SELECTIONTEXT  3

// Item IDs
#define ID_PAGE       1
#define ID_LINK       2
#define ID_HIGHLIGHT  3
#define ID_SELECTION  4

// Item Layers
#define Z_PAGE        0.0
#define Z_LINK       10.0
#define Z_HIGHLIGHT  20.0
#define Z_SELECTION  30.0

// Zoom

#define ZOOM_INIT  100.0
#define ZOOM_MIN    10.0
#define ZOOM_STEP   20.0

#define _SCALE  (_zoom/100.0)

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  static QGraphicsItem *addHighlight(QGraphicsScene *scene, const QRectF& rect)
  {
    QColor hlColor(Qt::yellow);
    hlColor.setAlphaF(0.4);
    QGraphicsItem *item = scene->addRect(rect,
                                         QPen(Qt::NoPen),
                                         QBrush(hlColor, Qt::SolidPattern));
    item->setZValue(Z_HIGHLIGHT);
    item->setData(DATA_ID, ID_HIGHLIGHT);

    return item;
  }

  static QGraphicsItem *addLink(QGraphicsScene *scene, const csPdfLink& link)
  {
    QGraphicsItem *item = scene->addRect(link.sourceRect());
    item->setFlag(QGraphicsItem::ItemHasNoContents, true);
    item->setCursor(Qt::PointingHandCursor);
    item->setZValue(Z_LINK);
    item->setData(DATA_ID, ID_LINK);
    item->setData(DATA_LINKPAGE, link.page());
    item->setData(DATA_LINKDEST, link.destTopLeft());

    return item;
  }

  static QGraphicsItem *addSelection(QGraphicsScene *scene, const csPdfText& pdfText)
  {
    QColor selColor(Qt::blue);
    selColor.setAlphaF(0.4);
    QGraphicsItem *item = scene->addRect(pdfText.rect(),
                                         QPen(Qt::NoPen),
                                         QBrush(selColor, Qt::SolidPattern));
    item->setZValue(Z_SELECTION);
    item->setData(DATA_ID, ID_SELECTION);
    item->setData(DATA_SELECTIONTEXT, pdfText.text());

    return item;
  }

}; // namespace priv

////// public ////////////////////////////////////////////////////////////////

WPdfView::WPdfView(QWidget *parent)
  : QGraphicsView(parent),
    _scene(0),
    _doc(),
    _page(),
    _zoom(ZOOM_INIT),
    _pageBounces(0)
{
  // Graphics Scene //////////////////////////////////////////////////////////

  _scene = new QGraphicsScene(this);
  _scene->setBackgroundBrush(QBrush(Qt::gray, Qt::SolidPattern));
  setScene(_scene);

  // Event Filter ////////////////////////////////////////////////////////////

  _scene->installEventFilter(this);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(this, SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)),
          SLOT(selectArea(QRect,QPointF,QPointF)));
  connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
          SLOT(watchVScroll(int)));
}

WPdfView::~WPdfView()
{
}

QString WPdfView::selectedText() const
{
  csPdfTexts texts;
  foreach(QGraphicsItem *item, _scene->items()) {
    if( item->data(DATA_ID).toInt() == ID_SELECTION ) {
      QGraphicsRectItem *ri = dynamic_cast<QGraphicsRectItem*>(item);
      texts.push_back(csPdfText(ri->rect(),
                                item->data(DATA_SELECTIONTEXT).toString()));
    }
  }

  if( texts.isEmpty() ) {
    return QString();
  }

  qSort(texts);

  QString text(texts[0].text());
  QRectF  lastRect(texts[0].rect());
  for(int i = 1; i < texts.size(); i++) {
    const QRectF curRect = texts[i].rect();

    if( lastRect.top() <= curRect.center().y()  &&
        curRect.center().y() <= lastRect.bottom() ) {
      text += " ";
    } else {
      text += "\n";
    }
    text += texts[i].text();

    lastRect = curRect;
  }

  return text;
}

void WPdfView::setDocument(const csPdfDocument& doc)
{
  _doc.clear();
  _page.clear();

  _doc = doc;

  resetTransform();
  _zoom = ZOOM_INIT;
  emit zoomChanged(_zoom);

  showFirstPage();
}

////// public slots //////////////////////////////////////////////////////////

void WPdfView::highlightText(const QString& text)
{
  removeItems(ID_HIGHLIGHT);

  if( _page.isEmpty() ) {
    return;
  }

  const csPdfTexts texts = _page.texts();
  const QStringList needles = csPdfPrepareSearch(text);
  if( needles.size() == 1 ) {
    foreach(const int pos,
            csPdfFindAll(texts, needles.front(), Qt::CaseInsensitive)) {
      priv::addHighlight(_scene, texts[pos].rect());
    }
  } else if( needles.size() > 1 ) {
    QList<QRectF> hlBoxes;
    foreach(const int pos,
            csPdfFindAll(texts, needles, Qt::CaseInsensitive)) {
      for(int i = 0; i < needles.size(); i++) {
        if( !hlBoxes.contains(texts[pos+i].rect()) ) {
          hlBoxes.push_back(texts[pos+i].rect());
        }
      }
    }
    foreach(const QRectF hlBox, hlBoxes) {
      priv::addHighlight(_scene, hlBox);
    }
  }
}

void WPdfView::removeMarks()
{
  removeItems(ID_HIGHLIGHT);
  removeItems(ID_SELECTION);
}

void WPdfView::showFirstPage()
{
  showPage(1);
}

void WPdfView::showLastPage()
{
  showPage(_doc.pageCount());
}

void WPdfView::showNextPage()
{
  if( _doc.isEmpty()  ||  _page.isEmpty() ) {
    emit pageChanged(1);
    return;
  }

  const int oldNo = _page.number();
  const int newNo = qBound(0, oldNo+1, _doc.pageCount()-1);
  showPage(newNo+1);

  if( oldNo != newNo ) {
    centerOn(_scene->sceneRect().center().x(), _scene->sceneRect().top());
  }
}

void WPdfView::showPage(int no)
{
  if( _doc.isEmpty() ) {
    emit pageChanged(1);
    return;
  }

  if( !_page.isEmpty()  &&  _page.number() == no-1 ) {
    return;
  }

  _scene->clear();

  const int pageNo = qBound(0, no-1, _doc.pageCount()-1); // 0-based
  _page = _doc.page(pageNo);
  renderPage();

  foreach(const csPdfLink link, _page.links()) {
    if( !link.isGoto() ) {
      continue;
    }

    priv::addLink(_scene, link);
  }

  emit pageChanged(pageNo+1);
}

void WPdfView::showPreviousPage()
{
  if( _doc.isEmpty()  ||  _page.isEmpty() ) {
    emit pageChanged(1);
    return;
  }

  const int oldNo = _page.number();
  const int newNo = qBound(0, oldNo-1, _doc.pageCount()-1);
  showPage(newNo+1);

  if( oldNo != newNo ) {
    centerOn(_scene->sceneRect().center().x(), _scene->sceneRect().bottom());
  }
}

void WPdfView::zoom(double level)
{
  const double oldZoom = _zoom;
  _zoom = qMax(ZOOM_MIN, level);

  if( oldZoom != _zoom ) {
    renderPage();
    setTransform(QTransform::fromScale(_SCALE, _SCALE));
  }

  emit zoomChanged(_zoom);
}

void WPdfView::zoomIn()
{
  zoom(_zoom+ZOOM_STEP);
}

void WPdfView::zoomOut()
{
  zoom(_zoom-ZOOM_STEP);
}

////// protected /////////////////////////////////////////////////////////////

bool WPdfView::eventFilter(QObject *watched, QEvent *event)
{
  if( event->type() == QEvent::GraphicsSceneMousePress ) {
    QGraphicsSceneMouseEvent *mev =
        dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    return followLink(mev->scenePos());
  }

  return QGraphicsView::eventFilter(watched, event);
}

void WPdfView::keyPressEvent(QKeyEvent *event)
{
  if(        event->matches(QKeySequence::MoveToStartOfDocument) ) {
    showFirstPage();
    event->accept();
  } else if( event->matches(QKeySequence::MoveToEndOfDocument) ) {
    showLastPage();
    event->accept();
  } else if( event->matches(QKeySequence::MoveToNextPage) ) {
    showNextPage();
    event->accept();
  } else if( event->matches(QKeySequence::MoveToPreviousPage) ) {
    showPreviousPage();
    event->accept();
  } else if( event->matches(QKeySequence::ZoomIn)  ||
             (event->modifiers() == Qt::ControlModifier  &&
              event->key() == Qt::Key_BracketRight) ) {
    zoomIn();
    event->accept();
  } else if( event->matches(QKeySequence::ZoomOut) ) {
    zoomOut();
    event->accept();
  }
}

void WPdfView::wheelEvent(QWheelEvent *event)
{
  // dy > 0: Away From User; dy < 0: Toward User
  const int dy = event->angleDelta().y() / 8; // [Degree]

  if( (event->modifiers() == Qt::NoModifier  &&
       !verticalScrollBar()->isVisible())
      ||  event->modifiers() == Qt::ControlModifier) {
    if(        dy < 0 ) {
      showNextPage();
      event->accept();
    } else if( dy > 0 ) {
      showPreviousPage();
      event->accept();
    }
  } else if( event->modifiers() == Qt::ShiftModifier ) {
    if(        dy < 0 ) {
      zoomIn();
      event->accept();
    } else if( dy > 0 ) {
      zoomOut();
      event->accept();
    }
  } else {
    QCoreApplication::sendEvent(verticalScrollBar(), event);
  }
}

////// private slots /////////////////////////////////////////////////////////

void WPdfView::selectArea(QRect rect, QPointF fromScene, QPointF toScene)
{
  if( _page.isEmpty()  ||  rect.isEmpty() ) {
    return;
  }

  const double x = qMin(fromScene.x(), toScene.x());
  const double y = qMin(fromScene.y(), toScene.y());
  const double w = qAbs(fromScene.x() - toScene.x());
  const double h = qAbs(fromScene.y() - toScene.y());

  removeItems(ID_SELECTION);
  foreach(const csPdfText t, _page.texts(QRectF(x, y, w, h))) {
    priv::addSelection(_scene, t);
  }
}

void WPdfView::watchVScroll(int value)
{
  const int cfg_maxPageBounces(1);

  if( !verticalScrollBar()->isVisible() ) {
    _pageBounces = 0;
    return;
  }

  if(        value == verticalScrollBar()->minimum() ) {
    if( _pageBounces > 0 ) {
      _pageBounces = -1;
    } else {
      _pageBounces--;
    }
    if( qAbs(_pageBounces) > cfg_maxPageBounces ) {
      showPreviousPage();
      _pageBounces = 0;
    }
  } else if( value == verticalScrollBar()->maximum() ) {
    if( _pageBounces < 0 ) {
      _pageBounces = 1;
    } else {
      _pageBounces++;
    }
    if( qAbs(_pageBounces) > cfg_maxPageBounces ) {
      showNextPage();
      _pageBounces = 0;
    }
  } else {
    _pageBounces = 0;
  }
}

////// private ///////////////////////////////////////////////////////////////

bool WPdfView::followLink(const QPointF& scenePos)
{
  QGraphicsItem *hitItem = 0;
  foreach(QGraphicsItem *item, _scene->items(scenePos)) {
    if( item->data(DATA_ID).toInt() != ID_LINK ) {
      continue;
    }

    if( hitItem == 0 ) {
      hitItem = item;
    } else {
      const QPointF dHit =
          hitItem->mapToScene(hitItem->boundingRect().center()) - scenePos;
      const QPointF dTest =
          item->mapToScene(item->boundingRect().center()) - scenePos;
      if( QPointF::dotProduct(dTest, dTest) < QPointF::dotProduct(dHit, dHit) ) {
        hitItem = item;
      }
    }
  }

  if( hitItem != 0 ) {
    const int     page = hitItem->data(DATA_LINKPAGE).toInt();
    const QPointF dest = hitItem->data(DATA_LINKDEST).toPointF();

    showPage(page+1);
    // centerOn(dest); // TODO: Transform 'dest'; cf. SumatraPDF

    return true;
  }

  return false;
}

void WPdfView::removeItems(const int id)
{
  foreach(QGraphicsItem *item, _scene->items()) {
    if( item->data(DATA_ID).toInt() == id ) {
      _scene->removeItem(item);
      delete item;
    }
  }
}

void WPdfView::renderPage()
{
  removeItems(ID_PAGE);

  if( _page.isEmpty() ) {
    return;
  }

  const QImage image = _page.renderToImage(_SCALE);
  QGraphicsItem *item = _scene->addPixmap(QPixmap::fromImage(image));
  item->setTransform(QTransform::fromScale(1.0/_SCALE, 1.0/_SCALE));
  item->setZValue(Z_PAGE);
  item->setData(DATA_ID, ID_PAGE);

  setSceneRect(_page.rect());
}
