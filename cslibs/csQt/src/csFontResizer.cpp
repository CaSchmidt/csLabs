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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QWidget>

#include "csQt/csFontResizer.h"

////// public ////////////////////////////////////////////////////////////////

csFontResizer::csFontResizer(QObject *parent)
  : QObject(parent)
  , minSize(6)
  , maxSize(14)
  , incSize(2)
  , defSize()
{
}

csFontResizer::~csFontResizer()
{
}

bool csFontResizer::eventFilter(QObject *watched, QEvent *event)
{
  if( event->type() == QEvent::KeyPress ) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    if( keyEvent->matches(QKeySequence::ZoomIn)  ||
        (keyEvent->modifiers() == Qt::ControlModifier &&
         keyEvent->key() == Qt::Key_BracketRight) ) {
      resizeFont(watched, 0, incSize);
      return true;
    } else if( keyEvent->matches(QKeySequence::ZoomOut) ) {
      resizeFont(watched, 0, -incSize);
      return true;
    } else if( keyEvent->modifiers() == Qt::ControlModifier  &&
               keyEvent->key() == Qt::Key_0 ) {
      resizeFont(watched, defSize.value(dynamic_cast<QWidget*>(watched), 0));
      return true;
    }
  } else if( event->type() == QEvent::Wheel ) {
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
    if( wheelEvent->modifiers() == Qt::ControlModifier  &&
        wheelEvent->delta() > 0 ) {
      resizeFont(watched, 0, incSize);
      return true;
    } else if( wheelEvent->modifiers() == Qt::ControlModifier  &&
               wheelEvent->delta() < 0 ) {
      resizeFont(watched, 0, -incSize);
      return true;
    }
  }

  return QObject::eventFilter(watched, event);
}

////// private ///////////////////////////////////////////////////////////////

void csFontResizer::resizeFont(QObject *watched, const int size, const int increment)
{
  if( (size == 0  &&  increment == 0)  ||
      (size != 0  &&  increment != 0) ) {
    return;
  }

  QWidget *widget = dynamic_cast<QWidget*>(watched);
  if( widget == 0 ) {
    return;
  }

  QFont font = widget->font();

  if( !defSize.contains(widget) ) {
    defSize.insert(widget, font.pointSize());
  }

  int newSize(0);
  if( size != 0 ) {
    newSize = qBound(minSize, size, maxSize);
  } else if( increment != 0 ) {
    newSize = qBound(minSize, font.pointSize() + increment, maxSize);
  }

  if( newSize > 0 ) {
    font.setPointSize(newSize);
  }

  widget->setFont(font);
}
