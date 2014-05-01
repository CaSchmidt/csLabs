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

#include <QtCore/QFile>

#include <csPDF/csPdfDocument.h>

#include <csPDF/internal/config.h>
#include <csPDF/internal/csPdfDocumentImpl.h>
#include <csPDF/internal/csPdfPageImpl.h>
#include <csPDF/internal/fz_util.h>
#include <csPDF/csPdfContentsModel.h>

////// public ////////////////////////////////////////////////////////////////

csPdfDocument::csPdfDocument()
  : impl()
{
}

csPdfDocument::~csPdfDocument()
{
}

bool csPdfDocument::isEmpty() const
{
  return impl.isNull();
}

void csPdfDocument::clear()
{
  impl.clear();
}

QString csPdfDocument::fileName() const
{
  if( isEmpty() ) {
    return QString();
  }

  CSPDF_DOCIMPL();

  return impl->filename;
}

int csPdfDocument::pageCount() const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDF_DOCIMPL();

  return fz_count_pages(impl->document);
}

csPdfPage csPdfDocument::page(const int no) const
{
  if( isEmpty() ) {
    return csPdfPage();
  }

  CSPDF_DOCIMPL();

  fz_page *page = NULL;
  fz_var(page);

  fz_try(impl->context) {
    page = fz_load_page(impl->document, no);
  } fz_catch(impl->context) {
    fz_free_page(impl->document, page);
    page = NULL;
  }

  if( page == NULL ) {
    return csPdfPage();
  }

  csPdfPage pdfpage;
  pdfpage.impl = QSharedPointer<csPdfPageImpl>(new csPdfPageImpl());
  if( pdfpage.impl.isNull() ) {
    fz_free_page(impl->document, page);

    return csPdfPage();
  }

  pdfpage.impl->page = page;
  pdfpage.impl->pdf  = impl;
  pdfpage.impl->no   = no;

  return pdfpage;
}

csPdfContentsNode *csPdfDocument::tableOfContents() const
{
  if( isEmpty() ) {
    return csPdfContentsModel::newRootNode();
  }

  CSPDF_DOCIMPL();

  csPdfContentsNode *root = csPdfContentsModel::newRootNode();
  if( root == 0 ) {
    return 0;
  }

  fz_outline *outline = NULL;
  fz_var(outline);

  fz_try(impl->context) {
    outline = fz_load_outline(impl->document);
    toContentsNode(outline, root);
  } fz_always(impl->context) {
    fz_free_outline(impl->context, outline);
    outline = NULL;
  } fz_catch(impl->context) {
  }

  return root;
}

csPdfTextPages csPdfDocument::textPages(const int first, const int count) const
{
  if( isEmpty() ) {
    return csPdfTextPages();
  }

  CSPDF_DOCIMPL();

  const int pageCount = fz_count_pages(impl->document);
  if( first < 0  ||  first >= pageCount ) {
    return csPdfTextPages();
  }

  const int last = count <= 0
      ? pageCount-1
      : qBound(0, first+count-1, pageCount-1);

  csPdfTextPages results;
  for(int pageNo = first; pageNo <= last; pageNo++) {
    fz_page *page = NULL;
    fz_var(page);

    fz_try(impl->context) {
      page = fz_load_page(impl->document, pageNo);

      fz_text_sheet *textSheet = NULL;
      fz_var(textSheet);
      fz_text_page *textPage = NULL;
      fz_var(textPage);
      fz_device *device = NULL;
      fz_var(device);

      fz_try(impl->context) {
        textSheet = fz_new_text_sheet(impl->context);
        textPage  = fz_new_text_page(impl->context);
        device    = fz_new_text_device(impl->context, textSheet, textPage);

        fz_matrix matrix;
        toMatrix(&matrix, QTransform());
        fz_run_page(impl->document, page, device, &matrix, NULL);

        results.push_back(csPdfTextPage(pageNo,
                                        extractText(textPage,
                                                    CSPDF_TEXT_WORDSPACING)));
      } fz_always(impl->context) {
        fz_free_device(device);
        device = NULL;
        fz_free_text_page(impl->context, textPage);
        textPage = NULL;
        fz_free_text_sheet(impl->context, textSheet);
        textSheet = NULL;
      } fz_catch(impl->context) {
      }
    } fz_always(impl->context) {
      fz_free_page(impl->document, page);
      page = NULL;
    } fz_catch(impl->context) {
    }
  } // For Each Page

  return results;
}

int csPdfDocument::renderThreads() const
{
  if( isEmpty() ) {
    return 0;
  }

  CSPDF_DOCIMPL();

  return impl->renderThreads;
}

int csPdfDocument::setRenderThreads(const int count)
{
  if( isEmpty() ) {
    return 0;
  }

  CSPDF_DOCIMPL();

  const int old = impl->renderThreads;
  impl->renderThreads = qMax(count, 1);

  return old;
}

csPdfDocument csPdfDocument::load(const QString& filename, const bool memory)
{
  QByteArray data;
  if( memory ) {
    QFile f(filename);
    if( f.open(QIODevice::ReadOnly) ) {
      data = f.readAll();
      f.close();
    } else {
      return csPdfDocument();
    }
  }

  fz_locks_context *locks = newLocksContext();
  if( locks == NULL ) {
    return csPdfDocument();
  }

  fz_context *context = fz_new_context(NULL, locks, FZ_STORE_DEFAULT);

  if( (memory && data.isEmpty()) ||  context == NULL ) {
    fz_free_context(context);
    deleteLocksContext(locks);

    return csPdfDocument();
  }

  fz_stream *stream = NULL;
  fz_var(stream);
  fz_document *document = NULL;
  fz_var(document);

  fz_try(context) {
    if( memory ) {
      stream = fz_open_memory(context,
                              reinterpret_cast<unsigned char*>(data.data()),
                              data.size());
    } else {
      stream = fz_open_file(context, filename.toUtf8().constData());
    }
    document = fz_open_document_with_stream(context,
                                            filename.toUtf8().constData(),
                                            stream);
  } fz_catch(context) {
    fz_close_document(document);
    document = NULL;
    fz_close(stream);
    stream = NULL;
  }

  if( document == NULL ) {
    fz_free_context(context);
    deleteLocksContext(locks);

    return csPdfDocument();
  }

  csPdfDocument pdfdocument;
  pdfdocument.impl = QSharedPointer<csPdfDocumentImpl>(new csPdfDocumentImpl());
  if( pdfdocument.impl.isNull() ) {
    fz_close_document(document);
    fz_close(stream);
    fz_free_context(context);
    deleteLocksContext(locks);

    return csPdfDocument();
  }

  pdfdocument.impl->document = document;
  pdfdocument.impl->stream   = stream;
  pdfdocument.impl->context  = context;
  pdfdocument.impl->data     = data;
  pdfdocument.impl->filename = filename;
  pdfdocument.impl->locks    = locks;

  return pdfdocument;
}
