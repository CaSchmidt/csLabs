/****************************************************************************
** Copyright (c) 2014-2015, Carsten Schmidt. All rights reserved.
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

#include <csPlot3D/csAxisLabel.h>

////// Public ////////////////////////////////////////////////////////////////

CS_PLOT3D_EXPORT void csSetLabelText(QStaticText& label, const QString& text)
{
  label.setPerformanceHint(QStaticText::AggressiveCaching);
  label.setTextFormat(Qt::PlainText);
  label.setText(text);
}

CS_PLOT3D_EXPORT QPoint csPlaceAxisLabel(const QPoint& p1, const QPoint& p2,
                                         const QStaticText& label,
                                         const bool isTop, const bool isLeft)
{
  const int offsetX(4);
  const int offsetY(4);

  const int        dx = p2.x()-p1.x();
  const int        dy = p2.y()-p1.y();
  const bool    isHor = qAbs(dx) > qAbs(dy);
  const double tanPHI = isHor
      ? double(qAbs(dy)) / double(qAbs(dx))
      : double(qAbs(dx)) / double(qAbs(dy));

  QPoint pos(p1);

  if( isHor ) {
    if( isTop ) {
      pos.ry() -= offsetY + label.size().height();
    } else {
      pos.ry() += offsetY;
    }
    if( dx < 0 ) {
      pos.rx() -= label.size().width();
    }
    const double shiftY = tanPHI * label.size().width();
    if(        dx > 0  &&  dy > 0  &&  !isTop ) {
      pos.ry() += shiftY;
    } else if( dx > 0  &&  dy < 0  &&   isTop ) {
      pos.ry() -= shiftY;
    } else if( dx < 0  &&  dy > 0  &&  !isTop ) {
      pos.ry() += shiftY;
    } else if( dx < 0  &&  dy < 0  &&   isTop ) {
      pos.ry() -= shiftY;
    }
  } else {
    if( isLeft ) {
      pos.rx() -= offsetX + label.size().width();
    } else {
      pos.rx() += offsetX;
    }
    if( dy < 0 ) {
      pos.ry() -= label.size().height();
    }
    const double shiftX = tanPHI * label.size().height();
    if(        dx > 0  &&  dy < 0  &&  !isLeft ) {
      pos.rx() += shiftX;
    } else if( dx < 0  &&  dy < 0  &&   isLeft ) {
      pos.rx() -= shiftX;
    } else if( dx > 0  &&  dy > 0  &&  !isLeft ) {
      pos.rx() += shiftX;
    } else if( dx < 0  &&  dy > 0  &&   isLeft ) {
      pos.rx() -= shiftX;
    }
  }

  return pos;
}
