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
#include <QtGui/QPaintEvent>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

#include "csQt/csCollapser.h"

////// public ////////////////////////////////////////////////////////////////

csCollapser::csCollapser(QSplitter *splitter, QWidget *widget,
                         const QColor& color, const int ratio)
  : QToolButton(0)
  , _lastSize(1)
  , _splitter(splitter)
  , _widget(widget)
{
  // QToolButton /////////////////////////////////////////////////////////////

  setArrowType(Qt::NoArrow);
  // setAutoRaise(true);
  setCursor(Qt::ArrowCursor);
  setIcon(QIcon());
  setToolButtonStyle(Qt::ToolButtonIconOnly);

  QPalette p = palette();
  p.setColor(QPalette::Active, QPalette::Button, color);
  setPalette(p);

  // Place button on splitter's handle ///////////////////////////////////////

  if( splitter->count() != 2 ) {
    return;
  }

  const int widgetIndex = splitter->indexOf(widget);
  if( widgetIndex < 0 ) {
    return;
  }

  for(int i = 0; i < splitter->count(); i++) {
    splitter->setCollapsible(i, i == widgetIndex);
  }

  const int handleIndex = widgetIndex == 0
      ? widgetIndex+1
      : widgetIndex;
  QSplitterHandle *handle = splitter->handle(handleIndex);
  setParent(handle);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  int w(0), h(0);
  if( splitter->orientation() == Qt::Horizontal ) {
    w = handle->width();
    h = ratio*w;
  } else {
    h = handle->width();
    w = ratio*h;
  }
  setMinimumSize(w, h);
  setMaximumSize(w, h);

  QLayout *layout = splitter->orientation() == Qt::Horizontal
      ? dynamic_cast<QLayout*>(new QVBoxLayout(handle))
      : dynamic_cast<QLayout*>(new QHBoxLayout(handle));
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(this);

  handle->setLayout(layout);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(this, &csCollapser::clicked, this, &csCollapser::collapse);
}

csCollapser::~csCollapser()
{
}

////// public slots //////////////////////////////////////////////////////////

void csCollapser::collapse()
{
  const int widgetIndex = _splitter->indexOf(_widget);

  QList<int> sizes = _splitter->sizes();
  if( sizes[widgetIndex] > 0 ) {
    _lastSize = sizes[widgetIndex];
    sizes[widgetIndex] = 0;
  } else {
    for(int i = 0; i < _splitter->count(); i++) {
      if( i == widgetIndex ) {
        continue;
      }
      sizes[i] -= _lastSize / (_splitter->count()-1);
    }
    sizes[widgetIndex] = _lastSize;
  }
  _splitter->setSizes(sizes);
}
