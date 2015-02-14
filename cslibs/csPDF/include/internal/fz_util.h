/****************************************************************************
** Copyright (c) 2013-2015, Carsten Schmidt. All rights reserved.
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

#ifndef __FZ_UTIL_H__
#define __FZ_UTIL_H__

#include <QtCore/QPointF>
#include <QtCore/QRect>
#include <QtCore/QRectF>
#include <QtGui/QTransform>

extern "C" {
#include <mupdf/fitz.h>
};

#include <csPDF/csPdfContentsNode.h>
#include <csPDF/csPdfLink.h>
#include <csPDF/csPdfText.h>

inline fz_matrix *setIdentity(fz_matrix *m)
{
  m->a = m->d = 1.0;
  m->b = m->c = m->e = m->f = 0.0;

  return m;
}

inline QPointF toPoint(const fz_point *point)
{
  return QPointF(point->x, point->y);
}

inline void toPoint(fz_point *point, const QPointF& qpoint)
{
  point->x = qpoint.x();
  point->y = qpoint.y();
}

inline QRect toRect(const fz_irect *rect)
{
  return QRect(rect->x0,
               rect->y0,
               rect->x1 - rect->x0,
               rect->y1 - rect->y0);
}

inline void toRect(fz_irect *rect, const QRect& qrect)
{
  rect->x0 = qrect.x();
  rect->y0 = qrect.y();
  rect->x1 = qrect.x() + qrect.width();
  rect->y1 = qrect.y() + qrect.height();
}

inline QRectF toRect(const fz_rect *rect)
{
  return QRectF(rect->x0,
                rect->y0,
                rect->x1 - rect->x0,
                rect->y1 - rect->y0);
}

inline void toRect(fz_rect *rect, const QRectF& qrect)
{
  rect->x0 = qrect.x();
  rect->y0 = qrect.y();
  rect->x1 = qrect.x() + qrect.width();
  rect->y1 = qrect.y() + qrect.height();
}

inline QTransform toTransform(const fz_matrix *matrix)
{
  return QTransform(matrix->a, matrix->b, 0.0,
                    matrix->c, matrix->d, 0.0,
                    matrix->e, matrix->f, 1.0);
}

inline void toMatrix(fz_matrix *matrix, const QTransform& qtransform)
{
  matrix->a = qtransform.m11(); // sx
  matrix->b = qtransform.m12(); // shear y (vertical)
  matrix->c = qtransform.m21(); // shear x (horizontal)
  matrix->d = qtransform.m22(); // sy
  matrix->e = qtransform.m31(); // dx
  matrix->f = qtransform.m32(); // dy
}

void toContentsNode(fz_outline *outline, csPdfContentsNode *parent);

csPdfLink toLink(fz_link *link, const QTransform& ctm);
csPdfLink toLink(const QRectF& sourceRect, void *data, const QTransform& ctm);

csPdfTexts extractText(void *data, const double wordSpacing);

fz_locks_context *newLocksContext();
void deleteLocksContext(fz_locks_context* &ctx);

QTransform ctmForPage(fz_document *doc, fz_page *page);

#endif // __FZ_UTIL_H__
