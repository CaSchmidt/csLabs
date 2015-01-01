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

#ifndef __CSMATH_H__
#define __CSMATH_H__

#include <QtCore/QVector>

template<typename T>
bool csIsIncreasing(const QVector<T>& v, const bool strictly = false)
{
  for(int i = 1; i < v.size(); i++) {
    if( strictly ) {
      if( v[i] <= v[i-1] ) {
        return false;
      }
    } else {
      if( v[i] <  v[i-1] ) {
        return false;
      }
    }
  }

  return true;
}

template<typename T>
inline T csInter1D(const T& x,
                   const T& x0, const T& x1,
                   const T& y0, const T& y1)
{
  const T y = y0 + (x-x0) * (y1-y0) / (x1-x0);

  return y;
}

template<typename T>
inline T csSign(const T& v)
{
  if(        v > 0 ) {
    return T(1);
  } else if( v < 0 ) {
    return T(-1);
  }

  return T(0);
}

#endif // __CSMATH_H__
