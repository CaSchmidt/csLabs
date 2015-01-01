/****************************************************************************
** Copyright (c) 2014-2015, Carsten Schmidt. All rights reserved.
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

#ifndef __CSEDGE_H__
#define __CSEDGE_H__

#include <QtCore/QList>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

#include <csPlot3D/csMath.h>

class csEdge {
public:
  enum Axis {
    Invalid = 0,
    XAxis,
    YAxis,
    ZAxis
  };

  csEdge(const Axis axis = Invalid,
         const float *data1 = 0, const float *data2 = 0,
         const QVector3D& n1 = QVector3D(),
         const QVector3D& n2 = QVector3D())
    : _axis(axis)
    , _p1()
    , _p2()
    , _mid()
    , _n1(n1)
    , _n2(n2)
  {
    if( data1 != 0 ) {
      _p1 = QVector4D(data1[0], data1[1], data1[2], 1.0f);
    }
    if( data2 != 0 ) {
      _p2 = QVector4D(data2[0], data2[1], data2[2], 1.0f);
    }
    if( !_p1.isNull()  ||  !_p2.isNull() ) {
      const QVector4D p12(_p2-_p1);
      const float     len = p12.length();
      const QVector3D dir(p12.toVector3D().normalized());
      _mid = _p1 + 0.5f*len*QVector4D(dir, 0.0f);
    }
  }

  ~csEdge()
  {
  }

  inline bool isEmpty() const
  {
    return
        _axis == Invalid
        ||  _p1.isNull()  ||  _p2.isNull()
        ||  _n1.isNull()  ||  _n2.isNull();
  }

  bool isSilhouette(const QMatrix4x4& view, const QVector4D& origin) const
  {
    const QVector3D n1 = view.mapVector(_n1).normalized();
    const QVector3D n2 = view.mapVector(_n2).normalized();
    const QVector3D o  = (view.map(_mid)-origin).toVector3D().normalized();

    return
        csSign(QVector3D::dotProduct(n1, o)) != csSign(QVector3D::dotProduct(n2, o));
  }

  inline Axis axis() const
  {
    return _axis;
  }

  inline const QVector4D& p1() const
  {
    return _p1;
  }

  inline const QVector4D& p2() const
  {
    return _p2;
  }

  inline const QVector4D& mid(const QMatrix4x4& M = QMatrix4x4()) const
  {
    return M.map(_mid);
  }

private:
  Axis _axis;
  QVector4D _p1, _p2, _mid;
  QVector3D _n1, _n2;
};

typedef QList<csEdge> csEdges;

#endif // __CSEDGE_H__
