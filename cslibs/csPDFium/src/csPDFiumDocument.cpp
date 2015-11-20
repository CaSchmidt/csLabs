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

#include <QtCore/QFile>

#include <csPDFium/csPDFiumDocument.h>

#include <csPDFium/csPDFiumContentsModel.h>

#include "internal/csPDFiumDocumentImpl.h"
#include "internal/csPDFiumPageImpl.h"
#include "internal/fpdf_util.h"

csPDFiumDocument::csPDFiumDocument()
  : impl()
{
}

csPDFiumDocument::~csPDFiumDocument()
{
}

bool csPDFiumDocument::isEmpty() const
{
  return impl.isNull();
}

bool csPDFiumDocument::isMemory() const
{
  if( isEmpty() ) {
    return false;
  }

  CSPDFIUM_DOCIMPL();

  return !impl->data.isEmpty();
}

void csPDFiumDocument::clear()
{
  if( isEmpty() ) {
    return;
  }

  // CAUTION: Upon on a successful lock we may delete 'mutex'!
  impl->mutex.lock();
  impl->mutex.unlock();

  impl.clear();
}

QString csPDFiumDocument::fileName() const
{
  if( isEmpty() ) {
    return QString();
  }

  CSPDFIUM_DOCIMPL();

  return impl->fileName;
}

int csPDFiumDocument::pageCount() const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDFIUM_DOCIMPL();

  return FPDF_GetPageCount(impl->document);
}

csPDFiumPage csPDFiumDocument::page(const int no) const
{
  if( isEmpty() ) {
    return csPDFiumPage();
  }

  CSPDFIUM_DOCIMPL();

  if( no < 0  ||  no >= FPDF_GetPageCount(impl->document) ) {
    return csPDFiumPage();
  }

  csPDFiumPageImpl *pimpl = new csPDFiumPageImpl();
  if( pimpl == 0 ) {
    return csPDFiumPage();
  }

  pimpl->page = FPDF_LoadPage(impl->document, no);
  if( pimpl->page == NULL ) {
    delete pimpl;
    return csPDFiumPage();
  }

  const qreal h = FPDF_GetPageHeight(pimpl->page);
  pimpl->ctm = QMatrix(1, 0, 0, -1, 0, h);
  pimpl->doc = impl;
  pimpl->no  = no;

  csPDFiumPage page;
  page.impl = QSharedPointer<csPDFiumPageImpl>(pimpl);

  return page;
}

csPDFiumContentsNode *csPDFiumDocument::tableOfContents() const
{
  if( isEmpty() ) {
    return 0;
  }

  CSPDFIUM_DOCIMPL();

  csPDFiumContentsNode *root = csPDFiumContentsModel::newRootNode();
  if( root != 0 ) {
    util::parseContents(FPDFBookmark_GetFirstChild(impl->document, NULL),
                        impl->document, root);
  }

  return root;
}

csPDFiumTextPage csPDFiumDocument::texPage(const int no) const
{
  if( isEmpty() ) {
    return csPDFiumTextPage();
  }

  CSPDFIUM_DOCIMPL();

  if( no < 0  ||  no >= FPDF_GetPageCount(impl->document) ) {
    return csPDFiumTextPage();
  }

  const FPDF_PAGE page = FPDF_LoadPage(impl->document, no);
  if( page == NULL ) {
    return csPDFiumTextPage();
  }

  const qreal     h = FPDF_GetPageHeight(page);
  const QMatrix ctm = QMatrix(1, 0, 0, -1, 0, h);

  const csPDFiumTextPage result(no, util::extractTexts(page, ctm));

  FPDF_ClosePage(page);

  return result;
}

csPDFiumTextPages csPDFiumDocument::textPages(const int first, const int count) const
{
  if( isEmpty() ) {
    return csPDFiumTextPages();
  }

  CSPDFIUM_DOCIMPL();

  const int pageCount = FPDF_GetPageCount(impl->document);
  if( first < 0  ||  first >= pageCount ) {
    return csPDFiumTextPages();
  }

  const int last = count <= 0
      ? pageCount-1
      : qBound(0, first+count-1, pageCount-1);

  csPDFiumTextPages results;
  for(int pageNo = first; pageNo <= last; pageNo++) {
    const FPDF_PAGE page = FPDF_LoadPage(impl->document, pageNo);
    if( page == NULL ) {
      continue;
    }

    const qreal     h = FPDF_GetPageHeight(page);
    const QMatrix ctm = QMatrix(1, 0, 0, -1, 0, h);

    results.push_back(csPDFiumTextPage(pageNo,
                                       util::extractTexts(page, ctm)));

    FPDF_ClosePage(page);
  }

  return results;
}

int csPDFiumDocument::resolveBookmark(const void *pointer) const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDFIUM_DOCIMPL();

  if( pointer == 0 ) {
    return -1;
  }

  const FPDF_BOOKMARK bookmark = (const FPDF_BOOKMARK)pointer;

  FPDF_DEST dest = FPDFBookmark_GetDest(impl->document, bookmark);
  if( dest == NULL ) {
    const FPDF_ACTION action = FPDFBookmark_GetAction(bookmark);
    if( action != NULL  &&  FPDFAction_GetType(action) == PDFACTION_GOTO ) {
      dest = FPDFAction_GetDest(impl->document, action);
    }
  }

  if( dest == NULL ) {
    return -1;
  }

  return FPDFDest_GetPageIndex(impl->document, dest);
}

int csPDFiumDocument::resolveLink(const void *pointer) const
{
  if( isEmpty() ) {
    return -1;
  }

  CSPDFIUM_DOCIMPL();

  if( pointer == 0 ) {
    return -1;
  }

  const FPDF_LINK link = (const FPDF_LINK)pointer;

  FPDF_DEST dest = FPDFLink_GetDest(impl->document, link);
  if( dest == NULL ) {
    const FPDF_ACTION action = FPDFLink_GetAction(link);
    if( action != NULL  &&  FPDFAction_GetType(action) == PDFACTION_GOTO ) {
      dest = FPDFAction_GetDest(impl->document, action);
    }
  }

  if( dest == NULL ) {
    return -1;
  }

  return FPDFDest_GetPageIndex(impl->document, dest);
}

csPDFiumDocument csPDFiumDocument::load(const QString& filename,
                                        const bool memory)
{
  csPDFiumDocumentImpl *impl = new csPDFiumDocumentImpl();
  if( impl == 0 ) {
    return csPDFiumDocument();
  }

  impl->fileName = filename;
  if( memory ) {
    QFile file(filename);
    if( !file.open(QIODevice::ReadOnly) ) {
      delete impl;
      return csPDFiumDocument();
    }

    impl->data = file.readAll();
    if( impl->data.size() != file.size() ) {
      file.close();
      delete impl;
      return csPDFiumDocument();
    }
    file.close();

    impl->document = FPDF_LoadMemDocument(impl->data.constData(),
                                          impl->data.size(), NULL);

  } else {
#ifndef Q_OS_WIN // ASSUMPTION: All other OSes treat paths as UTF-8...
    impl->document = FPDF_LoadDocument(filename.toUtf8().constData(), NULL);
#else
    // NOTE: PDFium uses UTF-16LE encoding!
    impl->document = FPDF_LoadDocumentW(filename.utf16(), NULL);
#endif
  }

  if( impl->document == NULL ) {
    delete impl;
    return csPDFiumDocument();
  }

  csPDFiumDocument doc;
  doc.impl = QSharedPointer<csPDFiumDocumentImpl>(impl);

  return doc;
}
