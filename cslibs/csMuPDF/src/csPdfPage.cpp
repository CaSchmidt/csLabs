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

#include <cstring>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QThread>

#include <csPDF/csPdfPage.h>

#include "internal/config.h"
#include "internal/csPdfPageImpl.h"
#include "internal/fz_pathext.h"
#include "internal/fz_render.h"
#include "internal/fz_util.h"

////// public ////////////////////////////////////////////////////////////////

csPdfPage::csPdfPage()
  : impl()
{
}

csPdfPage::~csPdfPage()
{
}

bool csPdfPage::isEmpty() const
{
  return impl.isNull();
}

void csPdfPage::clear()
{
  impl.clear();
}

int csPdfPage::number() const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDF_PAGEIMPL();

  return impl->no;
}

QImage csPdfPage::renderToImage(const double scale) const
{
  return renderToImage(QRectF(), scale);
}

QImage csPdfPage::renderToImage(const QRectF& area, const double scale) const
{
  if( isEmpty()  ||  scale <= 0.0 ) {
    return QImage();
  }

  CSPDF_PAGEIMPL();

  // (1) Create Transformed Rendering Bounds/Box /////////////////////////////

  fz_matrix pageXForm;
  fz_scale(&pageXForm, scale, scale);

  fz_rect renderRect;
  if( area.isNull() ) {
    fz_bound_page(impl->pdf->document, impl->page, &renderRect);
  } else {
    toRect(&renderRect, area);
  }

  fz_transform_rect(&renderRect, &pageXForm);

  fz_irect iRenderBox;
  fz_round_rect(&iRenderBox, &renderRect);

  // cf. <mupdf/fitz/math.h>
  const int renderWidth  = iRenderBox.x1 - iRenderBox.x0;
  const int renderHeight = iRenderBox.y1 - iRenderBox.y0;

  // (2) Create Target Image /////////////////////////////////////////////////

  QImage image(renderWidth, renderHeight, QImage::Format_RGBA8888_Premultiplied);
  if( image.isNull() ) {
    return QImage();
  }
  image.fill(Qt::white);

  // (3) Parse Page Into Display List ////////////////////////////////////////

  fz_display_list *displayList = NULL;
  fz_var(displayList);

  fz_try(impl->pdf->context) {
    displayList = fz_new_display_list(impl->pdf->context);

    fz_device *device = fz_new_list_device(impl->pdf->context, displayList);
    fz_run_page(impl->pdf->document, impl->page, device, &pageXForm, NULL);
    fz_free_device(device);
  } fz_catch(impl->pdf->context) {
    fz_drop_display_list(impl->pdf->context, displayList);
    displayList = NULL;
  }

  if( displayList == NULL ) {
    return QImage();
  }

  // (4) Slice Rendering Area Along Y-Axis ///////////////////////////////////

  QList<FzRenderData> renderJobs;

  const int noJobs = impl->pdf->renderThreads;

  int y0 = iRenderBox.y0;
  for(int i = 0; i < noJobs; i++) {
    const int sliceHeight = i == 0
        ? renderHeight / noJobs + renderHeight % noJobs
        : renderHeight / noJobs;

    fz_pixmap *pixmap = NULL;
    fz_var(pixmap);

    fz_try(impl->pdf->context) {
      fz_irect iSliceBox;
      iSliceBox.x0 = iRenderBox.x0;
      iSliceBox.y0 = y0;
      iSliceBox.x1 = iRenderBox.x1;
      iSliceBox.y1 = y0+sliceHeight;

      pixmap = fz_new_pixmap_with_bbox_and_data(impl->pdf->context,
                                                fz_device_rgb(impl->pdf->context),
                                                &iSliceBox,
                                                image.scanLine(iSliceBox.y0-iRenderBox.y0));

      fz_rect sliceRect;
      fz_rect_from_irect(&sliceRect, &iSliceBox);

      renderJobs.push_back(FzRenderData(impl->pdf->context, displayList,
                                        pixmap, &sliceRect));
    } fz_catch(impl->pdf->context) {
      fz_drop_pixmap(impl->pdf->context, pixmap);
      pixmap = NULL;
    }

    if( pixmap == NULL ) {
      break;
    }

    y0 += sliceHeight;
  }

  // (5) Free Display List ///////////////////////////////////////////////////

  fz_drop_display_list(impl->pdf->context, displayList);
  displayList = NULL;

  // (6) Execute Jobs ////////////////////////////////////////////////////////

  if( renderJobs.size() == noJobs ) {
    if( noJobs == 1 ) {
      fzRender(renderJobs.front());
    } else {
      QtConcurrent::blockingMap(renderJobs, fzRender);
    }
  }

  return image;
}

csPdfLinks csPdfPage::links() const
{
  if( isEmpty() ) {
    csPdfLinks();
  }

  CSPDF_PAGEIMPL();

  csPdfLinks _links;

  fz_link *link = NULL;
  fz_var(link);

  fz_try(impl->pdf->context) {
    link = fz_load_links(impl->pdf->document, impl->page);
    for(fz_link *l = link; l != NULL; l = l->next) {
      csPdfLink _l = toLink(l, ctmForPage(impl->pdf->document, impl->page));
      if( !_l.isInvalid() ) {
        _links.push_back(_l);
      }
    }
  } fz_always(impl->pdf->context) {
    fz_drop_link(impl->pdf->context, link);
    link = NULL;
  } fz_catch(impl->pdf->context) {
  }

  return _links;
}

csPdfTexts csPdfPage::texts(const QRectF& area) const
{
  if( isEmpty() ) {
    return csPdfTexts();
  }

  CSPDF_PAGEIMPL();

  if( impl->textsCache.isEmpty() ) {
    fz_text_sheet *sheet = NULL;
    fz_var(sheet);
    fz_text_page *page = NULL;
    fz_var(page);
    fz_device *device = NULL;
    fz_var(device);

    fz_try(impl->pdf->context) {
      sheet  = fz_new_text_sheet(impl->pdf->context);
      page   = fz_new_text_page(impl->pdf->context);
      device = fz_new_text_device(impl->pdf->context, sheet, page);

      fz_matrix matrix;
      toMatrix(&matrix, QTransform());
      fz_run_page(impl->pdf->document, impl->page, device, &matrix, NULL);
      impl->textsCache = extractText(page, CSPDF_TEXT_WORDSPACING);
    } fz_always(impl->pdf->context) {
      fz_free_device(device);
      device = NULL;
      fz_free_text_page(impl->pdf->context, page);
      page = NULL;
      fz_free_text_sheet(impl->pdf->context, sheet);
      sheet = NULL;
    } fz_catch(impl->pdf->context) {
    }
  }

  if( area.isNull() ) {
    return impl->textsCache;
  }

  csPdfTexts _texts;
  foreach(const csPdfText t, impl->textsCache) {
    if( area.intersects(t.rect()) ) {
      _texts.push_back(t);
    }
  }

  return _texts;
}

QRectF csPdfPage::rect() const
{
  if( isEmpty() ) {
    return QRectF();
  }

  CSPDF_PAGEIMPL();

  fz_rect rect;
  fz_bound_page(impl->pdf->document, impl->page, &rect);

  return toRect(&rect);
}

QSizeF csPdfPage::size() const
{
  return rect().size();
}

QList<QPainterPath> csPdfPage::extractPaths(const bool closed) const
{
  if( isEmpty() ) {
    return QList<QPainterPath>();
  }

  CSPDF_PAGEIMPL();

  PathExt ext;
  ext.closed = closed;

  fz_device *dev = fzNewPathExtDevice(impl->pdf->context, &ext);
  if( dev == NULL ) {
    return QList<QPainterPath>();
  }

  fz_matrix I;
  setIdentity(&I);

  fz_run_page(impl->pdf->document, impl->page, dev, &I, NULL);

  fz_free_device(dev);

  return ext.paths;
}
