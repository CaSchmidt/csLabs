/****************************************************************************
** Copyright (c) 2015, Carsten Schmidt. All rights reserved.
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

#include <QtGui/QPainter>
#include <QtWidgets/QProxyStyle>
#include <QtWidgets/QStyleOptionFrame>

#include "csQt/csProgressLineEdit.h"

////// Style /////////////////////////////////////////////////////////////////

class csProgressLineEditStyle : public QProxyStyle {
  Q_OBJECT
public:
  csProgressLineEditStyle(QStyle *style = 0)
    : QProxyStyle(style)
    , _color(64, 192, 255)
    , _progress(0)
  {
  }

  ~csProgressLineEditStyle()
  {
  }

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget) const
  {
    QProxyStyle::drawPrimitive(element, option, painter, widget);
    if( element == QStyle::PE_PanelLineEdit  &&  _progress != 0 ) {
      const qreal ratio = qreal(_progress) / qreal(100.0);
      const QRect rect0 = subElementRect(QStyle::SE_LineEditContents, option, widget);
      QRect rect(rect0);
      rect.setWidth(qBound(0, (int)(ratio*rect0.width()), rect0.width()));
      if( rect.width() > 0 ) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(_color, Qt::SolidPattern));
        painter->drawRect(rect);
        painter->restore();
      }
    }
  }

  const QColor& color() const
  {
    return _color;
  }

  void setColor(const QColor& c)
  {
    _color = c;
  }

  int progress() const
  {
    return _progress;
  }

  int setProgress(const int p)
  {
    const int old = _progress;
    _progress = qBound(0, p, 100);
    return old;
  }

private:
  QColor _color;
  int    _progress;
};

////// public ////////////////////////////////////////////////////////////////

csProgressLineEdit::csProgressLineEdit(QWidget *parent)
  : QLineEdit(parent)
  , _style()
{
  _style = new csProgressLineEditStyle();
  setStyle(_style);
}

csProgressLineEdit::~csProgressLineEdit()
{
  delete _style;
}

////// public slots //////////////////////////////////////////////////////////

void csProgressLineEdit::setProgress(int value)
{
  const int old = _style->setProgress(value);
  if( old != value ) {
    update();
  }
}

#include "csProgressLineEdit.moc"
