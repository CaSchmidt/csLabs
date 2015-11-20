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

#ifndef __CSPDFIUMDOCUMENT_H__
#define __CSPDFIUMDOCUMENT_H__

#include <QtCore/QSharedPointer>
#include <QtCore/QString>

#include <csPDFium/cspdfium_config.h>
#include <csPDFium/csPDFiumContentsNode.h>
#include <csPDFium/csPDFiumPage.h>
#include <csPDFium/csPDFiumTextPage.h>

class csPDFiumDocumentImpl;

class CS_PDFIUM_EXPORT csPDFiumDocument {
public:
  csPDFiumDocument();
  ~csPDFiumDocument();

  bool isEmpty() const;
  bool isMemory() const;
  void clear();
  QString fileName() const;
  int pageCount() const;
  csPDFiumPage page(const int no) const; // no == [0, pageCount()-1]
  csPDFiumContentsNode *tableOfContents() const;
  csPDFiumTextPage textPage(const int no) const; // no == [0, pageCount()-1]
  csPDFiumTextPages textPages(const int first, const int count = -1) const;
  int resolveBookmark(const void *pointer) const;
  int resolveLink(const void *pointer) const;

  static csPDFiumDocument load(const QString& filename,
                               const bool memory = false);

private:
  QSharedPointer<csPDFiumDocumentImpl> impl;
};

#endif // __CSPDFIUMDOCUMENT_H__
