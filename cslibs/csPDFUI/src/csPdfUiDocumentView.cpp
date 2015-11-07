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

#include <cmath>

#include <QtCore/QCoreApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QScrollBar>

#include <csPDFUI/csPdfUiDocumentView.h>

#include <csPDFium/csPDFiumUtil.h>
#include <csPDFSearch/csPdfSearchUtil.h>

////// Macros ////////////////////////////////////////////////////////////////

// Item Data
#define DATA_ID             0
#define DATA_LINKPOINTER    1
#define DATA_LINKDEST       2
#define DATA_SELECTIONTEXT  3
#define DATA_SELECTIONPOS   4

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
    item->setZValue(csPdfUiDocumentView::HighlightLayer);
    csPdfUiDocumentView::setItemId(item, csPdfUiDocumentView::HighlightId);

    return item;
  }

  static QGraphicsItem *addLink(QGraphicsScene *scene, const csPDFiumLink& link)
  {
    QGraphicsItem *item = scene->addRect(link.srcRect());
    item->setFlag(QGraphicsItem::ItemHasNoContents, true);
    item->setCursor(Qt::PointingHandCursor);
    item->setZValue(csPdfUiDocumentView::LinkLayer);
    csPdfUiDocumentView::setItemId(item, csPdfUiDocumentView::LinkId);
    item->setData(DATA_LINKPOINTER, QVariant::fromValue(const_cast<void*>(link.pointer())));
    item->setData(DATA_LINKDEST, QPointF()); // TODO: Link destination

    return item;
  }

  static QGraphicsItem *addSelection(QGraphicsScene *scene, const csPDFiumText& pdfText)
  {
    QColor selColor(Qt::blue);
    selColor.setAlphaF(0.4);
    QGraphicsItem *item = scene->addRect(pdfText.rect(),
                                         QPen(Qt::NoPen),
                                         QBrush(selColor, Qt::SolidPattern));
    item->setZValue(csPdfUiDocumentView::SelectionLayer);
    csPdfUiDocumentView::setItemId(item, csPdfUiDocumentView::SelectionId);
    item->setData(DATA_SELECTIONTEXT, pdfText.text());
    item->setData(DATA_SELECTIONPOS,  pdfText.pos());

    return item;
  }

}; // namespace priv

////// public ////////////////////////////////////////////////////////////////

csPdfUiDocumentView::csPdfUiDocumentView(QWidget *parent)
  : QGraphicsView(parent)
  , _scene(0)
  , _doc()
  , _page()
  , _zoom(ZOOM_INIT)
  , _zoomMode(ZoomUser)
  , _pageBounces(0)
  , _linkHistory()
{
  // Graphics Scene //////////////////////////////////////////////////////////

  _scene = new QGraphicsScene(this);
  _scene->setBackgroundBrush(QBrush(Qt::gray, Qt::SolidPattern));
  setScene(_scene);

  // Default Drag Mode ///////////////////////////////////////////////////////

  setDragMode(ScrollHandDrag);

  // Event Filter ////////////////////////////////////////////////////////////

  _scene->installEventFilter(this);
  verticalScrollBar()->installEventFilter(this);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(this, SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)),
          SLOT(selectArea(QRect,QPointF,QPointF)));
}

csPdfUiDocumentView::~csPdfUiDocumentView()
{
}

QString csPdfUiDocumentView::selectedText() const
{
  csPDFiumTexts texts;
  foreach(QGraphicsItem *item, _scene->items()) {
    if( itemId(item) == SelectionId ) {
      QGraphicsRectItem *ri = dynamic_cast<QGraphicsRectItem*>(item);
      texts.push_back(csPDFiumText(ri->rect(),
                                   item->data(DATA_SELECTIONTEXT).toString(),
                                   item->data(DATA_SELECTIONPOS).toInt()));
    }
  }

  if( texts.isEmpty() ) {
    return QString();
  }

  qSort(texts);

  QString text(texts[0].text());
  for(int i = 1; i < texts.size(); i++) {
    if( csPDFium::overlapsV(texts[i-1].rect(), texts[i].rect()) ) {
      text += _L1C(' ');
    } else {
      text += _L1C('\n');
    }
    text += texts[i].text();
  }

  return text;
}

const csPDFiumDocument& csPdfUiDocumentView::document() const
{
  return _doc;
}

void csPdfUiDocumentView::setDocument(const csPDFiumDocument& doc)
{
  _scene->clear();
  _doc.clear();
  _page.clear();
  _linkHistory.clear();

  _doc = doc;

  resetTransform();
  _zoom = ZOOM_INIT;
  emit zoomChanged(_zoom);

  showFirstPage();
}

void csPdfUiDocumentView::setItemId(QGraphicsItem *item, const int id)
{
  item->setData(DATA_ID, id);
}

int csPdfUiDocumentView::itemId(const QGraphicsItem *item)
{
  return item->data(DATA_ID).toInt();
}

////// public slots //////////////////////////////////////////////////////////

void csPdfUiDocumentView::fitToPageWidth()
{
  if( zoom(_zoom, ZoomFitToPageWidth) ) {
    renderPage();
  }
}

void csPdfUiDocumentView::gotoLinkSource()
{
  if( !_linkHistory.isEmpty() ) {
    const ReverseLink revLink = _linkHistory.pop();
    showPage(revLink.page);
    centerOn(revLink.center);
  }
}

void csPdfUiDocumentView::highlightText(const QString& text)
{
  removeItems(HighlightId);

  if( _page.isEmpty() ) {
    return;
  }

  const csPDFiumTexts texts = _page.texts();
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

void csPdfUiDocumentView::removeMarks()
{
  removeItems(HighlightId);
  removeItems(SelectionId);
}

void csPdfUiDocumentView::showFirstPage()
{
  showPage(1);
}

void csPdfUiDocumentView::showLastPage()
{
  showPage(_doc.pageCount());
}

void csPdfUiDocumentView::showNextPage()
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

void csPdfUiDocumentView::showPage(int no)
{
  _pageBounces = 0;

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
  zoom(_zoom, _zoomMode);
  renderPage();

  foreach(const csPDFiumLink link, _page.links()) {
    if( !link.isEmpty() ) {
      priv::addLink(_scene, link);
    }
  }

  emit pageChanged(pageNo+1);
}

void csPdfUiDocumentView::showPreviousPage()
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

void csPdfUiDocumentView::zoom(double level)
{
  if( zoom(level, ZoomUser) ) {
    renderPage();
  }
}

void csPdfUiDocumentView::zoomIn()
{
  zoom(_zoom+ZOOM_STEP);
}

void csPdfUiDocumentView::zoomOut()
{
  zoom(_zoom-ZOOM_STEP);
}

////// protected /////////////////////////////////////////////////////////////

bool csPdfUiDocumentView::eventFilter(QObject *watched, QEvent *event)
{
  if(        event->type() == QEvent::GraphicsSceneMousePress ) {
    QGraphicsSceneMouseEvent *mev =
        dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if( mev->modifiers() == Qt::NoModifier  &&
        mev->buttons()   ==  Qt::LeftButton &&
        followLink(mev->scenePos()) ) {
      return true;
    }
  } else if( event->type() == QEvent::MouseButtonDblClick ) {
    QMouseEvent *mev = dynamic_cast<QMouseEvent*>(event);
    if( mev->button() == Qt::LeftButton ) {
      const int btnRng = verticalScrollBar()->size().width()-1;
      const int max    = verticalScrollBar()->maximum();
      const int maxY   = verticalScrollBar()->height()-1;
      const int min    = verticalScrollBar()->minimum();
      const int val    = verticalScrollBar()->value();
      const int y      = mev->pos().y();

      if(        val == min  &&  0 <= y  &&  y <= btnRng ) {
        showPreviousPage();
        return true;
      } else if( val == max  &&  maxY-btnRng <= y  &&  y <= maxY) {
        showNextPage();
        return true;
      }
    }
  }

  return QGraphicsView::eventFilter(watched, event);
}

void csPdfUiDocumentView::keyPressEvent(QKeyEvent *event)
{
  if(        event->matches(QKeySequence::MoveToStartOfDocument) ) {
    showFirstPage();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::MoveToEndOfDocument) ) {
    showLastPage();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::MoveToNextPage) ) {
    showNextPage();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::MoveToPreviousPage) ) {
    showPreviousPage();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::ZoomIn)  ||
             (event->modifiers() == Qt::ControlModifier  &&
              event->key() == Qt::Key_BracketRight) ) {
    zoomIn();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::ZoomOut) ) {
    zoomOut();
    event->accept();
    return;
  } else if( event->matches(QKeySequence::Back) ) {
    gotoLinkSource();
    event->accept();
    return;
  }

  QGraphicsView::keyPressEvent(event);
}

QList<QGraphicsItem*> csPdfUiDocumentView::listItems(const int id) const
{
  QList<QGraphicsItem*> found;
  foreach(QGraphicsItem *item, _scene->items()) {
    if( itemId(item) == id ) {
      found.push_back(item);
    }
  }
  return found;
}

void csPdfUiDocumentView::removeItems(const int id)
{
  foreach(QGraphicsItem *item, _scene->items()) {
    if( itemId(item) == id ) {
      _scene->removeItem(item);
      delete item;
    }
  }
}

void csPdfUiDocumentView::resizeEvent(QResizeEvent *)
{
  if( _zoomMode == ZoomFitToPageWidth ) {
    if( zoom(_zoom, _zoomMode) ) {
      renderPage();
    }
  }
}

void csPdfUiDocumentView::wheelEvent(QWheelEvent *event)
{
  // Configuration
  const int cfg_maxPageBounces(1);

  // Conditions
  const bool condVScrollVisible
      = verticalScrollBar()->isVisible();
  const bool condPageChange
      = (event->modifiers() == Qt::NoModifier  &&  !condVScrollVisible)
      || event->modifiers() == Qt::ShiftModifier;
  const bool condZoom
      = event->modifiers() == Qt::ControlModifier;
  const bool condPageBounce
      = event->modifiers() == Qt::NoModifier  &&  condVScrollVisible;

  // dy > 0: Away From User; dy < 0: Toward User
  const int dy = event->angleDelta().y() / 8; // [Degree]

  if( !condVScrollVisible ) {
    _pageBounces = 0;
  }

  if(        condPageChange ) {
    if(        dy < 0 ) {
      showNextPage();
      event->accept();
      return;
    } else if( dy > 0 ) {
      showPreviousPage();
      event->accept();
      return;
    }

  } else if( condZoom ) {
    if(        dy < 0 ) {
      zoomOut();
      event->accept();
      return;
    } else if( dy > 0 ) {
      zoomIn();
      event->accept();
      return;
    }

  } else if( condPageBounce ) {
    const int value = verticalScrollBar()->value();

    if(        value <= verticalScrollBar()->minimum() ) {
      if( _pageBounces > 0 ) {
        _pageBounces = -1;
      } else {
        _pageBounces--;
      }
      if( qAbs(_pageBounces) > cfg_maxPageBounces ) {
        showPreviousPage();
        _pageBounces = 0;
        return;
      }

    } else if( value >= verticalScrollBar()->maximum() ) {
      if( _pageBounces < 0 ) {
        _pageBounces = 1;
      } else {
        _pageBounces++;
      }
      if( qAbs(_pageBounces) > cfg_maxPageBounces ) {
        showNextPage();
        _pageBounces = 0;
        return;
      }

    } else {
      _pageBounces = 0;
    }
  }

  QGraphicsView::wheelEvent(event);
}

////// private slots /////////////////////////////////////////////////////////

void csPdfUiDocumentView::selectArea(QRect rect, QPointF fromScene, QPointF toScene)
{
  if( _page.isEmpty()  ||  rect.isEmpty() ) {
    return;
  }

  const double x = qMin(fromScene.x(), toScene.x());
  const double y = qMin(fromScene.y(), toScene.y());
  const double w = qAbs(fromScene.x() - toScene.x());
  const double h = qAbs(fromScene.y() - toScene.y());

  removeItems(SelectionId);
  foreach(const csPDFiumText t, _page.texts(QRectF(x, y, w, h))) {
    priv::addSelection(_scene, t);
  }
}

////// private ///////////////////////////////////////////////////////////////

bool csPdfUiDocumentView::followLink(const QPointF& scenePos)
{
  const QGraphicsItem *item = _scene->itemAt(scenePos, QTransform());
  if( item != 0  &&  itemId(item) == LinkId ) {
    const void *pointer = item->data(DATA_LINKPOINTER).value<void*>();
    const QPointF  dest = item->data(DATA_LINKDEST).toPointF();
    const int      page = _doc.resolveLink(pointer);
    if( page < 0 ) {
      return false;
    }

    _linkHistory.push(ReverseLink(_page.number()+1, item->boundingRect().center()));
    showPage(page+1);
    if( dest.isNull() ) {
      centerOn(_page.rect().center().x(), _page.rect().top());
    } else {
      centerOn(dest);
    }

    return true;
  }

  return false;
}

void csPdfUiDocumentView::renderPage()
{
  removeItems(PageId);

  if( _page.isEmpty() ) {
    return;
  }

  const QImage image = _page.renderToImage(_SCALE);
  QGraphicsItem *item = _scene->addPixmap(QPixmap::fromImage(image));
  item->setTransform(QTransform::fromScale(1.0/_SCALE, 1.0/_SCALE));
  item->setZValue(PageLayer);
  setItemId(item, PageId);

  setSceneRect(_page.rect());
}

bool csPdfUiDocumentView::zoom(const double level, const int newMode)
{
  const double oldZoom = _zoom;

  _zoomMode = newMode;
  if( _zoomMode == ZoomFitToPageWidth  &&  !_page.isEmpty() ) {
    const double   pageWidth = _page.size().width();
    const double screenWidth = viewport()->width();
    _zoom = qMax(ZOOM_MIN, std::floor(screenWidth / pageWidth * 100.0));
  } else { // Default: User defined
    _zoom = qMax(ZOOM_MIN, level);
  }
  setTransform(QTransform::fromScale(_SCALE, _SCALE));

  if( oldZoom != _zoom ) {
    emit zoomChanged(_zoom);
  }

  return oldZoom != _zoom;
}
