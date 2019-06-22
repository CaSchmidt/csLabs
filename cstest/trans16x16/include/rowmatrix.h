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

#ifndef ROWMATRIX_H
#define ROWMATRIX_H

#include <cstdint>

#include <vector>

class RowMatrix {
public:
  using Buffer = std::vector<uint8_t>;
  using size_type = Buffer::size_type;

  RowMatrix(const size_type rows, const size_type cols);
  ~RowMatrix() noexcept = default;

  RowMatrix(const RowMatrix&) = default;
  RowMatrix& operator=(const RowMatrix&) = default;

  RowMatrix(RowMatrix&&) noexcept = default;
  RowMatrix& operator=(RowMatrix&&) noexcept = default;

  size_type columns() const;
  size_type rows() const;
  size_type stride() const;

  uint8_t *row(const size_type i);
  const uint8_t *row(const size_type i) const;

  uint8_t& operator()(const size_type i, const size_type j);
  const uint8_t& operator()(const size_type i, const size_type j) const;

  void print() const;

private:
  RowMatrix() = delete;

  Buffer _buffer{};
  size_type _stride{};
};

#endif // ROWMATRIX_H
