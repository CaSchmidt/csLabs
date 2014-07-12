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

#ifndef __CSPDFLINK_H__
#define __CSPDFLINK_H__

#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

class csPdfLink {
public:
  enum Type {
    Invalid = 0,
    Goto
  };

  inline csPdfLink()
    : _type(Invalid)
    , _srcRect()
    , _page(-1)
    , _destTL()
    , _destBR()
    , _is_fit_h()
    , _is_fit_v()
    , _is_zoom()
    , _is_new_window()
  {
  }

  inline ~csPdfLink()
  {
  }

  inline Type type() const
  {
    return _type;
  }

  inline bool isInvalid() const
  {
    return _type == Invalid;
  }

  inline bool isGoto() const
  {
    return _type == Goto;
  }

  inline QRectF sourceRect() const
  {
    return _srcRect;
  }

  inline int page() const
  {
    return _page;
  }

  inline QPointF destTopLeft() const
  {
    return _destTL;
  }

  inline QPointF destBottomRight() const
  {
    return _destBR;
  }

  inline bool isFitHorizontally() const
  {
    return _is_fit_h;
  }

  inline bool isFitVertically() const
  {
    return _is_fit_v;
  }

  inline bool isZoom() const
  {
    return _is_zoom;
  }

  inline double zoomRatio() const
  {
    if( isZoom() ) {
      return _destBR.x();
    }
    return 0.0;
  }

  inline bool isNewWindow() const
  {
    return _is_new_window;
  }

private:
  Type _type;
  QRectF _srcRect;
  int _page;
  QPointF _destTL;
  QPointF _destBR;
  bool _is_fit_h;
  bool _is_fit_v;
  bool _is_zoom;
  bool _is_new_window;

  friend csPdfLink toLink(const QRectF&, void *);
};

typedef QList<csPdfLink>                      csPdfLinks;
typedef QList<csPdfLink>::iterator            csPdfLinkIter;
typedef QList<csPdfLink>::const_iterator csConstPdfLinkIter;

#endif // __CSPDFLINK_H__
