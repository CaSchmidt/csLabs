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

#ifndef __CSPDFUTIL_H__
#define __CSPDFUTIL_H__

#include <QtCore/QRectF>

#ifndef _L1
# define _L1(s)  QString::fromLatin1(s)
#endif

#ifndef _L1C
# define _L1C(c)  QChar::fromLatin1(c)
#endif

#ifndef _U8
# define _U8(s)  QString::fromUtf8(s)
#endif

namespace csPDFium {

  inline bool overlapsH(const QRectF& a, const QRectF& b)
  {
    const bool left  = a.left() <= b.left()   &&  b.left()  <= a.right();
    const bool right = a.left() <= b.right()  &&  b.right() <= a.right();

    return left  ||  right;
  }

  inline bool overlapsV(const QRectF& a, const QRectF& b)
  {
    const bool top    = a.top() <= b.top()     &&  b.top()    <= a.bottom();
    const bool bottom = a.top() <= b.bottom()  &&  b.bottom() <= a.bottom();

    return top  ||  bottom;
  }

}; // namespace csPDFium

#endif // __CSPDFUTIL_H__
