/****************************************************************************
** Copyright (c) 2017, Carsten Schmidt. All rights reserved.
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

#include <QtGui/QTransform>

#include "internal/PanAndZoom.h"

#include "internal/Mapping.h"

namespace Pan {

  SimPlotRange horizontal(const QPointF& delta, const QSizeF& screen,
                          const SimPlotRange& viewX,
                          const SimPlotRange& viewY,
                          const SimPlotRange& boundsX)
  {
    if( !viewX.isValid()  ||  !viewY.isValid()  ||  !boundsX.isValid()  ||  screen.isEmpty() ) {
      return SimPlotRange();
    }

    const QTransform  xform = Mapping::screenToView(screen, viewX, viewY, true);
    const QPointF deltaView = xform.map(delta);

    // Horizontal

    qreal leftX = viewX.min() - deltaView.x();
    if(        leftX < boundsX.min() ) {
      leftX += boundsX.min() - leftX;
    } else if( leftX + viewX.span() > boundsX.max() ) {
      leftX -= leftX + viewX.span() - boundsX.max();
    }

    return SimPlotRange(leftX, leftX + viewX.span());
  }

  SimPlotRange vertical(const QPointF& delta, const QSizeF& screen,
                        const SimPlotRange& viewX,
                        const SimPlotRange& viewY,
                        const SimPlotRange& boundsY)
  {
    if( !viewX.isValid()  ||  !viewY.isValid()  ||  !boundsY.isValid()  ||  screen.isEmpty() ) {
      return SimPlotRange();
    }

    const QTransform  xform = Mapping::screenToView(screen, viewX, viewY, true);
    const QPointF deltaView = xform.map(delta);

    // Vertical

    qreal bottomY = viewY.min() + deltaView.y();
    if(        bottomY < boundsY.min() ) {
      bottomY += boundsY.min() - bottomY;
    } else if( bottomY + viewY.span() > boundsY.max() ) {
      bottomY -= bottomY + viewY.span() - boundsY.max();
    }

    return SimPlotRange(bottomY, bottomY + viewY.span());
  }

} // namespace Pan

namespace ZoomIn {

  QRectF rectangular(const QRectF& zoomRect, const QRectF& screen,
                     const SimPlotRange& viewX,
                     const SimPlotRange& viewY)
  {
    if( !viewX.isValid()  ||  !viewY.isValid()  ||  screen.isEmpty() ) {
      return QRectF();
    }

    const QRectF inter = screen & zoomRect;
    if( inter.isEmpty() ) {
      return QRectF();
    }

    const QTransform xform = Mapping::screenToView(screen.size(), viewX, viewY);
    const QRectF  zoomView =
        xform.map(inter.translated(-screen.topLeft())).boundingRect();

    const QRectF view(QPointF(viewX.min(), viewY.min()),
                      QPointF(viewX.max(), viewY.max()));

    return view & zoomView;
  }

} // namespace ZoomIn
