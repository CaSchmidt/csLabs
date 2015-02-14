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

#ifndef __CSPDFTEXT_H__
#define __CSPDFTEXT_H__

#include <QtCore/QList>
#include <QtCore/QRectF>
#include <QtCore/QString>

class csPdfText {
public:
  inline csPdfText(const QRectF& r = QRectF(), const QString& t = QString())
    : _rect(r)
    , _text(t)
  {
  }

  inline ~csPdfText()
  {
  }

  inline const QRectF rect() const
  {
    return _rect;
  }

  inline const QString text() const
  {
    return _text;
  }

  inline bool operator<(const csPdfText& other) const
  {
    const QRectF& a = _rect;
    const QRectF& b = other.rect();

    if( a.center().y() < b.top() ) {
      return true;
    } else if( a.top() <= b.center().y()  &&  b.center().y() <= a.bottom()  &&
               a.center().x() < b.center().x() ) {
      return true;
    }

    return false;
  }

private:
  QRectF  _rect;
  QString _text;
};

typedef QList<csPdfText>                       csPdfTexts;
typedef QList<csPdfText>::iterator             csPdfTextIter;
typedef QList<csPdfText>::const_iterator  csConstPdfTextIter;

#endif // __CSPDFTEXT_H__
