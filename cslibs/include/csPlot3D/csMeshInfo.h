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

#ifndef __CSMESHINFO_H__
#define __CSMESHINFO_H__

#include <csPlot3D/csMath.h>

class csMeshInfo {
public:
  csMeshInfo()
    : _numColumns()
    , _numRows()
    , _numVertices()
    , _xMin()
    , _xMax()
    , _yMin()
    , _yMax()
    , _zMin()
    , _zMax()
  {
  }

  ~csMeshInfo()
  {
  }

  inline bool isEmpty() const
  {
    return _numColumns == 0  ||  _numRows == 0  ||  _numVertices == 0;
  }

  inline int columnCount() const
  {
    return _numColumns;
  }

  inline int rowCount() const
  {
    return _numRows;
  }

  inline int vertexCount() const
  {
    return _numVertices;
  }

  inline int column(const int index) const
  {
    return index % _numColumns;
  }

  inline int row(const int index) const
  {
    return index / _numColumns;
  }

  inline int index(const int x, const int y) const
  {
    return y*_numColumns+x;
  }

  inline float xCenter() const
  {
    return (_xMax+_xMin) / 2.0f;
  }

  inline float yCenter() const
  {
    return (_yMax+_yMin) / 2.0f;
  }

  inline float zCenter() const
  {
    return (_zMax+_zMin) / 2.0f;
  }

  inline float xInterval() const
  {
    return _xMax-_xMin;
  }

  inline float yInterval() const
  {
    return _yMax-_yMin;
  }

  inline float zInterval() const
  {
    return _zMax-_zMin;
  }

  inline float xMin() const
  {
    return _xMin;
  }

  inline float xMax() const
  {
    return _xMax;
  }

  inline float yMin() const
  {
    return _yMin;
  }

  inline float yMax() const
  {
    return _yMax;
  }

  inline float zMin() const
  {
    return _zMin;
  }

  inline float zMax() const
  {
    return _zMax;
  }

  bool initialize(const QVector<float>& x, const QVector<float>& y,
                  const QVector<float>& z)
  {
    _numColumns = _numRows = _numVertices = 0;
    _xMin = _xMax = _yMin = _yMax = _zMin = _zMax = 0.0f;

    if( x.size() < 2  ||  y.size() < 2  ||  x.size()*y.size() != z.size() ) {
      return false;
    }

    const bool strictly(false);
    if( !csIsIncreasing(x, strictly)  ||  !csIsIncreasing(y, strictly) ) {
      return false;
    }

    _numColumns  = x.size();
    _numRows     = y.size();
    _numVertices = z.size();

    computeRange(_xMin, _xMax, x);
    computeRange(_yMin, _yMax, y);
    computeRange(_zMin, _zMax, z);

    return true;
  }

private:
  void computeRange(float& min, float& max, const QVector<float>& v) const
  {
    min = max = v[0];
    for(int i = 1; i < v.size(); i++) {
      if( v[i] < min ) {
        min = v[i];
      }
      if( v[i] > max ) {
        max = v[i];
      }
    }
  }

  int _numColumns;
  int _numRows;
  int _numVertices;
  float _xMin;
  float _xMax;
  float _yMin;
  float _yMax;
  float _zMin;
  float _zMax;
};

#endif // __CSMESHINFO_H__
