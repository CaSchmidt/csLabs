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

#include <fpdfdoc.h>

#include <csPDFium/csPDFiumPage.h>

#include "internal/csPDFiumPageImpl.h"
#include "internal/fpdf_util.h"

csPDFiumPage::csPDFiumPage()
  : impl()
{
}

csPDFiumPage::~csPDFiumPage()
{
}

bool csPDFiumPage::isEmpty() const
{
  return impl.isNull();
}

void csPDFiumPage::clear()
{
  if( isEmpty() ) {
    return;
  }

  CSPDFIUM_PAGEIMPL();

  impl.clear();
}

int csPDFiumPage::number() const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDFIUM_PAGEIMPL();

  return impl->no;
}

QImage csPDFiumPage::renderToImage(const double scale) const
{
  if( isEmpty() ) {
    return QImage();
  }

  CSPDFIUM_PAGEIMPL();

  const double w = FPDF_GetPageWidth(impl->page);
  const double h = FPDF_GetPageHeight(impl->page);

  QImage image(w*scale, h*scale, QImage::Format_RGBA8888);
  if( image.isNull() ) {
    return QImage();
  }
  image.fill(0xFFFFFFFF);

  FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(),
                                           FPDFBitmap_BGRA,
                                           image.scanLine(0), image.bytesPerLine());
  if( bitmap == NULL ) {
    return QImage();
  }

  FPDF_RenderPageBitmap(bitmap, impl->page,
                        0, 0, image.width(), image.height(),
                        0, 0); // no rotation, no flags
  FPDFBitmap_Destroy(bitmap);
  bitmap = NULL;

  for(int i = 0; i < image.height(); i++) {
    uchar *pixels = image.scanLine(i);
    for(int j = 0; j < image.width(); j++) {
      qSwap(pixels[0], pixels[2]);
      pixels += 4;
    }
  }

  return image;
}

csPDFiumLinks csPDFiumPage::links() const
{
  if( isEmpty() ) {
    return csPDFiumLinks();
  }

  CSPDFIUM_PAGEIMPL();

  csPDFiumLinks links;

  int pos(0);
  FPDF_LINK link;
  while( FPDFLink_Enumerate(impl->page, &pos, &link) ) {
    FPDF_DEST dest = FPDFLink_GetDest(impl->doc->document, link);
    if( dest == NULL ) {
      const FPDF_ACTION action = FPDFLink_GetAction(link);
      if( action == NULL  ||  FPDFAction_GetType(action) != PDFACTION_GOTO ) {
        continue;
      }
      dest = FPDFAction_GetDest(impl->doc->document, action);
      if( dest == NULL ) {
        continue;
      }
    }

    const unsigned long destPage = FPDFDest_GetPageIndex(impl->doc->document, dest);

    FS_RECTF linkRect;
    if( !FPDFLink_GetAnnotRect(link, &linkRect) ) {
      continue;
    }

    const QPointF topLeft     = QPointF(linkRect.left,  linkRect.top)   *impl->ctm;
    const QPointF bottomRight = QPointF(linkRect.right, linkRect.bottom)*impl->ctm;

    links.push_back(csPDFiumLink(QRectF(topLeft, bottomRight), destPage));
  }

  return links;
}

csPDFiumTexts csPDFiumPage::texts(const QRectF& area) const
{
  if( isEmpty() ) {
    return csPDFiumTexts();
  }

  CSPDFIUM_PAGEIMPL();

  if( impl->textCache.isEmpty() ) {
    impl->textCache = util::extractTexts(impl->page, impl->ctm);
  }

  if( area.isNull() ) {
    return impl->textCache;
  }

  csPDFiumTexts texts;
  foreach(const csPDFiumText& t, impl->textCache) {
    if( area.intersects(t.rect()) ) {
      texts.push_back(t);
    }
  }

  return texts;
}

QRectF csPDFiumPage::rect() const
{
  if( isEmpty() ) {
    return QRectF();
  }

  CSPDFIUM_PAGEIMPL();

  const double w = FPDF_GetPageWidth(impl->page);
  const double h = FPDF_GetPageHeight(impl->page);

  return QRectF(QPointF(0, 0), QSizeF(w, h));
}

QSizeF csPDFiumPage::size() const
{
  if( isEmpty() ) {
    return QSizeF();
  }

  CSPDFIUM_PAGEIMPL();

  const double w = FPDF_GetPageWidth(impl->page);
  const double h = FPDF_GetPageHeight(impl->page);

  return QSizeF(w, h);
}