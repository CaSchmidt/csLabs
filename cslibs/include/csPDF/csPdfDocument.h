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

#ifndef __CSPDFDOCUMENT_H__
#define __CSPDFDOCUMENT_H__

#include <QtCore/QSharedPointer>
#include <QtCore/QString>

#include <csPDF/cspdf_config.h>
#include <csPDF/csPdfContentsNode.h>
#include <csPDF/csPdfPage.h>
#include <csPDF/csPdfTextPage.h>

class csPdfDocumentImpl;

class CS_PDF_EXPORT csPdfDocument {
public:
  csPdfDocument();
  ~csPdfDocument();

  bool isEmpty() const;
  void clear();
  QString fileName() const;
  int pageCount() const;
  csPdfPage page(const int no) const; // no == [0, pageCount()-1]
  csPdfContentsNode *tableOfContents() const;
  csPdfTextPages textPages(const int first, const int count = -1) const;
  int renderThreads() const;
  int setRenderThreads(const int count);

  static csPdfDocument load(const QString& filename, const bool memory = false);

private:
  QSharedPointer<csPdfDocumentImpl> impl;
};

#endif // __CSPDFDOCUMENT_H__
