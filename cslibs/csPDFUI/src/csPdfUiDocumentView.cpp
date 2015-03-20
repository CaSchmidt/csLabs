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
#define DATA_LINKPAGE       1
#define DATA_LINKDEST       2
#define DATA_SELECTIONTEXT  3

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
    item->setData(DATA_LINKPAGE, link.destPage());
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

    return item;
  }

}; // namespace priv

////// public ////////////////////////////////////////////////////////////////

csPdfUiDocumentView::csPdfUiDocumentView(QWidget *parent)
  : QGraphicsView(parent)
  , _scene(0)
  , _doc()
  , _page()
  , _multiView(false)
  , _zoom(ZOOM_INIT)
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
  connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
          SLOT(watchVScroll(int)));
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
                                item->data(DATA_SELECTIONTEXT).toString()));
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

void csPdfUiDocumentView::setMultiView(const bool on)
{
  _multiView = on;
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

void csPdfUiDocumentView::gotoLinkSource()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  if( !_linkHistory.isEmpty() ) {
    const ReverseLink revLink = _linkHistory.pop();
    showPage(revLink.page);
    centerOn(revLink.center);
  }
}

void csPdfUiDocumentView::highlightText(const QString& text)
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

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
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  removeItems(HighlightId);
  removeItems(SelectionId);
}

void csPdfUiDocumentView::showFirstPage()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  showPage(1);
}

void csPdfUiDocumentView::showLastPage()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  showPage(_doc.pageCount());
}

void csPdfUiDocumentView::showNextPage()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

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
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

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
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

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
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  const double oldZoom = _zoom;
  _zoom = qMax(ZOOM_MIN, level);

  if( oldZoom != _zoom ) {
    renderPage();
    setTransform(QTransform::fromScale(_SCALE, _SCALE));
  }

  emit zoomChanged(_zoom);
}

void csPdfUiDocumentView::zoomIn()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  zoom(_zoom+ZOOM_STEP);
}

void csPdfUiDocumentView::zoomOut()
{
  if( _multiView  &&  (!isVisible() || !hasFocus()) ) {
    return;
  }

  zoom(_zoom-ZOOM_STEP);
}

////// protected /////////////////////////////////////////////////////////////

bool csPdfUiDocumentView::eventFilter(QObject *watched, QEvent *event)
{
  if(        event->type() == QEvent::GraphicsSceneMousePress ) {
    QGraphicsSceneMouseEvent *mev =
        dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if( mev->modifiers() == Qt::NoModifier  &&
        mev->buttons() ==  Qt::LeftButton ) {
      return followLink(mev->scenePos());
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

void csPdfUiDocumentView::wheelEvent(QWheelEvent *event)
{
  // dy > 0: Away From User; dy < 0: Toward User
  const int dy = event->angleDelta().y() / 8; // [Degree]

  if( (event->modifiers() == Qt::NoModifier  &&
       !verticalScrollBar()->isVisible())
      ||  event->modifiers() == Qt::ShiftModifier) {
    if(        dy < 0 ) {
      showNextPage();
      event->accept();
      return;
    } else if( dy > 0 ) {
      showPreviousPage();
      event->accept();
      return;
    }
  } else if( event->modifiers() == Qt::ControlModifier ) {
    if(        dy < 0 ) {
      zoomOut();
      event->accept();
      return;
    } else if( dy > 0 ) {
      zoomIn();
      event->accept();
      return;
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

void csPdfUiDocumentView::watchVScroll(int value)
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

bool csPdfUiDocumentView::followLink(const QPointF& scenePos)
{
  foreach(QGraphicsItem *item, _scene->items(scenePos)) {
    if( item != 0  &&  itemId(item) == LinkId ) {
      const int     page = item->data(DATA_LINKPAGE).toInt();
      const QPointF dest = item->data(DATA_LINKDEST).toPointF();

      _linkHistory.push(ReverseLink(_page.number()+1, item->boundingRect().center()));
      showPage(page+1);
      if( dest.isNull() ) {
        centerOn(_page.rect().center().x(), _page.rect().top());
      } else {
        centerOn(dest);
      }

      return true;
    }
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

#if 0
  // DEBUG
  QList<QPainterPath> ext = _page.extractPaths();
  foreach(const QPainterPath& path, ext) {
    QGraphicsItem *item =
        _scene->addPath(path,
                        QPen(QBrush(Qt::red, Qt::SolidPattern),
                             2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
                        QBrush(Qt::NoBrush)/*QBrush(Qt::red, Qt::SolidPattern)*/);
    item->setZValue(100.0);
    item->setCursor(Qt::CrossCursor);
  }
#endif
}
