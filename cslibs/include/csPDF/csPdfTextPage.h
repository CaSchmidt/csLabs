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

#ifndef __CSPDFTEXTPAGE_H__
#define __CSPDFTEXTPAGE_H__

#include <csPDF/csPdfText.h>

class csPdfTextPage {
public:
  inline csPdfTextPage(const int pageNo = -1,
                       const csPdfTexts& txts = csPdfTexts())
    : _no(pageNo)
    , _texts(txts)
  {
  }

  inline ~csPdfTextPage()
  {
  }

  const bool isEmpty() const
  {
    return _no < 0  ||  _texts.isEmpty();
  }

  inline int no() const
  {
    return _no;
  }

  inline csPdfTexts texts() const
  {
    return _texts;
  }

private:
  int _no;
  csPdfTexts _texts;
};

typedef QList<csPdfTextPage>                       csPdfTextPages;
typedef QList<csPdfTextPage>::iterator             csPdfTextPageIter;
typedef QList<csPdfTextPage>::const_iterator  csConstPdfTextPageIter;

#endif // __CSPDFTEXTPAGE_H__
