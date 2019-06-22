/****************************************************************************
** Copyright (c) 2019, Carsten Schmidt. All rights reserved.
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

#include <cstdio>

#include "rowmatrix.h"

////// public ////////////////////////////////////////////////////////////////

RowMatrix::RowMatrix(const size_type rows, const size_type cols)
  : _buffer(rows*cols, 0)
  , _stride(cols)
{
}

RowMatrix::size_type RowMatrix::columns() const
{
  return _stride;
}

RowMatrix::size_type RowMatrix::rows() const
{
  return _buffer.size()/_stride;
}

RowMatrix::size_type RowMatrix::stride() const
{
  return _stride;
}

uint8_t *RowMatrix::row(const size_type i)
{
  return _buffer.data() + i*_stride;
}

const uint8_t *RowMatrix::row(const size_type i) const
{
  return _buffer.data() + i*_stride;
}

uint8_t& RowMatrix::operator()(const size_type i, const size_type j)
{
  return _buffer[i*_stride + j];
}

const uint8_t& RowMatrix::operator()(const size_type i, const size_type j) const
{
  return _buffer[i*_stride + j];
}

void RowMatrix::print() const
{
  for(size_type i = 0; i < rows(); i++) {
    for(size_type j = 0; j < columns(); j++) {
      printf(" %3d", operator()(i, j));
    }
    printf("\n");
  }
}
