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
#include <cstdlib>

#include <emmintrin.h>

// #include <intrin.h>

#include "rowmatrix.h"

using size_type = RowMatrix::size_type;

void transpose16x16(uint8_t *dest, const size_type destStride,
                    const uint8_t *src, const size_type srcStride)
{
  // __debugbreak();

#if 1
  __m128i row0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  0*srcStride));
  __m128i row1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  1*srcStride));
  __m128i row2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  2*srcStride));
  __m128i row3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  3*srcStride));
  __m128i row4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  4*srcStride));
  __m128i row5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  5*srcStride));
  __m128i row6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  6*srcStride));
  __m128i row7 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  7*srcStride));
  __m128i row8 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  8*srcStride));
  __m128i row9 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  9*srcStride));
  __m128i rowA = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 10*srcStride));
  __m128i rowB = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 11*srcStride));
  __m128i rowC = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 12*srcStride));
  __m128i rowD = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 13*srcStride));
  __m128i rowE = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 14*srcStride));
  __m128i rowF = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 15*srcStride));

  __m128i merge10 = _mm_unpacklo_epi8(row0, row1);
  __m128i merge11 = _mm_unpackhi_epi8(row0, row1);
  __m128i merge12 = _mm_unpacklo_epi8(row2, row3);
  __m128i merge13 = _mm_unpackhi_epi8(row2, row3);
  __m128i merge14 = _mm_unpacklo_epi8(row4, row5);
  __m128i merge15 = _mm_unpackhi_epi8(row4, row5);
  __m128i merge16 = _mm_unpacklo_epi8(row6, row7);
  __m128i merge17 = _mm_unpackhi_epi8(row6, row7);
  __m128i merge18 = _mm_unpacklo_epi8(row8, row9);
  __m128i merge19 = _mm_unpackhi_epi8(row8, row9);
  __m128i merge1A = _mm_unpacklo_epi8(rowA, rowB);
  __m128i merge1B = _mm_unpackhi_epi8(rowA, rowB);
  __m128i merge1C = _mm_unpacklo_epi8(rowC, rowD);
  __m128i merge1D = _mm_unpackhi_epi8(rowC, rowD);
  __m128i merge1E = _mm_unpacklo_epi8(rowE, rowF);
  __m128i merge1F = _mm_unpackhi_epi8(rowE, rowF);

  __m128i merge20 = _mm_unpacklo_epi16(merge10, merge12);
  __m128i merge21 = _mm_unpackhi_epi16(merge10, merge12);
  __m128i merge22 = _mm_unpacklo_epi16(merge11, merge13);
  __m128i merge23 = _mm_unpackhi_epi16(merge11, merge13);
  __m128i merge24 = _mm_unpacklo_epi16(merge14, merge16);
  __m128i merge25 = _mm_unpackhi_epi16(merge14, merge16);
  __m128i merge26 = _mm_unpacklo_epi16(merge15, merge17);
  __m128i merge27 = _mm_unpackhi_epi16(merge15, merge17);
  __m128i merge28 = _mm_unpacklo_epi16(merge18, merge1A);
  __m128i merge29 = _mm_unpackhi_epi16(merge18, merge1A);
  __m128i merge2A = _mm_unpacklo_epi16(merge19, merge1B);
  __m128i merge2B = _mm_unpackhi_epi16(merge19, merge1B);
  __m128i merge2C = _mm_unpacklo_epi16(merge1C, merge1E);
  __m128i merge2D = _mm_unpackhi_epi16(merge1C, merge1E);
  __m128i merge2E = _mm_unpacklo_epi16(merge1D, merge1F);
  __m128i merge2F = _mm_unpackhi_epi16(merge1D, merge1F);

  __m128i merge30 = _mm_unpacklo_epi32(merge20, merge24);
  __m128i merge31 = _mm_unpackhi_epi32(merge20, merge24);
  __m128i merge32 = _mm_unpacklo_epi32(merge22, merge26);
  __m128i merge33 = _mm_unpackhi_epi32(merge22, merge26);
  __m128i merge34 = _mm_unpacklo_epi32(merge21, merge25);
  __m128i merge35 = _mm_unpackhi_epi32(merge21, merge25);
  __m128i merge36 = _mm_unpacklo_epi32(merge23, merge27);
  __m128i merge37 = _mm_unpackhi_epi32(merge23, merge27);
  __m128i merge38 = _mm_unpacklo_epi32(merge28, merge2C);
  __m128i merge39 = _mm_unpackhi_epi32(merge28, merge2C);
  __m128i merge3A = _mm_unpacklo_epi32(merge2A, merge2E);
  __m128i merge3B = _mm_unpackhi_epi32(merge2A, merge2E);
  __m128i merge3C = _mm_unpacklo_epi32(merge29, merge2D);
  __m128i merge3D = _mm_unpackhi_epi32(merge29, merge2D);
  __m128i merge3E = _mm_unpacklo_epi32(merge2B, merge2F);
  __m128i merge3F = _mm_unpackhi_epi32(merge2B, merge2F);

  __m128i merge40 = _mm_unpacklo_epi64(merge30, merge38);
  __m128i merge41 = _mm_unpackhi_epi64(merge30, merge38);
  __m128i merge42 = _mm_unpacklo_epi64(merge31, merge39);
  __m128i merge43 = _mm_unpackhi_epi64(merge31, merge39);
  __m128i merge44 = _mm_unpacklo_epi64(merge34, merge3C);
  __m128i merge45 = _mm_unpackhi_epi64(merge34, merge3C);
  __m128i merge46 = _mm_unpacklo_epi64(merge35, merge3D);
  __m128i merge47 = _mm_unpackhi_epi64(merge35, merge3D);
  __m128i merge48 = _mm_unpacklo_epi64(merge32, merge3A);
  __m128i merge49 = _mm_unpackhi_epi64(merge32, merge3A);
  __m128i merge4A = _mm_unpacklo_epi64(merge33, merge3B);
  __m128i merge4B = _mm_unpackhi_epi64(merge33, merge3B);
  __m128i merge4C = _mm_unpacklo_epi64(merge36, merge3E);
  __m128i merge4D = _mm_unpackhi_epi64(merge36, merge3E);
  __m128i merge4E = _mm_unpacklo_epi64(merge37, merge3F);
  __m128i merge4F = _mm_unpackhi_epi64(merge37, merge3F);
#else
  const __m128i row0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  0*srcStride));
  const __m128i row1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  1*srcStride));
  const __m128i row2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  2*srcStride));
  const __m128i row3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  3*srcStride));
  const __m128i row4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  4*srcStride));
  const __m128i row5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  5*srcStride));
  const __m128i row6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  6*srcStride));
  const __m128i row7 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  7*srcStride));
  const __m128i row8 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  8*srcStride));
  const __m128i row9 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src +  9*srcStride));
  const __m128i rowA = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 10*srcStride));
  const __m128i rowB = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 11*srcStride));
  const __m128i rowC = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 12*srcStride));
  const __m128i rowD = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 13*srcStride));
  const __m128i rowE = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 14*srcStride));
  const __m128i rowF = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 15*srcStride));

  const __m128i merge10 = _mm_unpacklo_epi8(row0, row1);
  const __m128i merge11 = _mm_unpackhi_epi8(row0, row1);
  const __m128i merge12 = _mm_unpacklo_epi8(row2, row3);
  const __m128i merge13 = _mm_unpackhi_epi8(row2, row3);
  const __m128i merge14 = _mm_unpacklo_epi8(row4, row5);
  const __m128i merge15 = _mm_unpackhi_epi8(row4, row5);
  const __m128i merge16 = _mm_unpacklo_epi8(row6, row7);
  const __m128i merge17 = _mm_unpackhi_epi8(row6, row7);
  const __m128i merge18 = _mm_unpacklo_epi8(row8, row9);
  const __m128i merge19 = _mm_unpackhi_epi8(row8, row9);
  const __m128i merge1A = _mm_unpacklo_epi8(rowA, rowB);
  const __m128i merge1B = _mm_unpackhi_epi8(rowA, rowB);
  const __m128i merge1C = _mm_unpacklo_epi8(rowC, rowD);
  const __m128i merge1D = _mm_unpackhi_epi8(rowC, rowD);
  const __m128i merge1E = _mm_unpacklo_epi8(rowE, rowF);
  const __m128i merge1F = _mm_unpackhi_epi8(rowE, rowF);

  const __m128i merge20 = _mm_unpacklo_epi16(merge10, merge12);
  const __m128i merge21 = _mm_unpackhi_epi16(merge10, merge12);
  const __m128i merge22 = _mm_unpacklo_epi16(merge11, merge13);
  const __m128i merge23 = _mm_unpackhi_epi16(merge11, merge13);
  const __m128i merge24 = _mm_unpacklo_epi16(merge14, merge16);
  const __m128i merge25 = _mm_unpackhi_epi16(merge14, merge16);
  const __m128i merge26 = _mm_unpacklo_epi16(merge15, merge17);
  const __m128i merge27 = _mm_unpackhi_epi16(merge15, merge17);
  const __m128i merge28 = _mm_unpacklo_epi16(merge18, merge1A);
  const __m128i merge29 = _mm_unpackhi_epi16(merge18, merge1A);
  const __m128i merge2A = _mm_unpacklo_epi16(merge19, merge1B);
  const __m128i merge2B = _mm_unpackhi_epi16(merge19, merge1B);
  const __m128i merge2C = _mm_unpacklo_epi16(merge1C, merge1E);
  const __m128i merge2D = _mm_unpackhi_epi16(merge1C, merge1E);
  const __m128i merge2E = _mm_unpacklo_epi16(merge1D, merge1F);
  const __m128i merge2F = _mm_unpackhi_epi16(merge1D, merge1F);

  const __m128i merge30 = _mm_unpacklo_epi32(merge20, merge24);
  const __m128i merge31 = _mm_unpackhi_epi32(merge20, merge24);
  const __m128i merge32 = _mm_unpacklo_epi32(merge22, merge26);
  const __m128i merge33 = _mm_unpackhi_epi32(merge22, merge26);
  const __m128i merge34 = _mm_unpacklo_epi32(merge21, merge25);
  const __m128i merge35 = _mm_unpackhi_epi32(merge21, merge25);
  const __m128i merge36 = _mm_unpacklo_epi32(merge23, merge27);
  const __m128i merge37 = _mm_unpackhi_epi32(merge23, merge27);
  const __m128i merge38 = _mm_unpacklo_epi32(merge28, merge2C);
  const __m128i merge39 = _mm_unpackhi_epi32(merge28, merge2C);
  const __m128i merge3A = _mm_unpacklo_epi32(merge2A, merge2E);
  const __m128i merge3B = _mm_unpackhi_epi32(merge2A, merge2E);
  const __m128i merge3C = _mm_unpacklo_epi32(merge29, merge2D);
  const __m128i merge3D = _mm_unpackhi_epi32(merge29, merge2D);
  const __m128i merge3E = _mm_unpacklo_epi32(merge2B, merge2F);
  const __m128i merge3F = _mm_unpackhi_epi32(merge2B, merge2F);

  const __m128i merge40 = _mm_unpacklo_epi64(merge30, merge38);
  const __m128i merge41 = _mm_unpackhi_epi64(merge30, merge38);
  const __m128i merge42 = _mm_unpacklo_epi64(merge31, merge39);
  const __m128i merge43 = _mm_unpackhi_epi64(merge31, merge39);
  const __m128i merge44 = _mm_unpacklo_epi64(merge34, merge3C);
  const __m128i merge45 = _mm_unpackhi_epi64(merge34, merge3C);
  const __m128i merge46 = _mm_unpacklo_epi64(merge35, merge3D);
  const __m128i merge47 = _mm_unpackhi_epi64(merge35, merge3D);
  const __m128i merge48 = _mm_unpacklo_epi64(merge32, merge3A);
  const __m128i merge49 = _mm_unpackhi_epi64(merge32, merge3A);
  const __m128i merge4A = _mm_unpacklo_epi64(merge33, merge3B);
  const __m128i merge4B = _mm_unpackhi_epi64(merge33, merge3B);
  const __m128i merge4C = _mm_unpacklo_epi64(merge36, merge3E);
  const __m128i merge4D = _mm_unpackhi_epi64(merge36, merge3E);
  const __m128i merge4E = _mm_unpacklo_epi64(merge37, merge3F);
  const __m128i merge4F = _mm_unpackhi_epi64(merge37, merge3F);
#endif

  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  0*destStride), merge40);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  1*destStride), merge41);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  2*destStride), merge42);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  3*destStride), merge43);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  4*destStride), merge44);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  5*destStride), merge45);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  6*destStride), merge46);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  7*destStride), merge47);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  8*destStride), merge48);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest +  9*destStride), merge49);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 10*destStride), merge4A);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 11*destStride), merge4B);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 12*destStride), merge4C);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 13*destStride), merge4D);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 14*destStride), merge4E);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + 15*destStride), merge4F);
}

int main(int /*argc*/, char ** /*argv*/)
{
  RowMatrix src(16, 16), dest(16, 16);

  for(size_type i = 0; i < src.rows(); i++) {
    for(size_type j = 0; j < src.columns(); j++) {
      src(i, j) = static_cast<uint8_t>(i*src.stride() + j);
    }
  }

  src.print(); printf("\n");
  transpose16x16(dest.row(0), dest.stride(), src.row(0), src.stride());
  dest.print(); printf("\n");

  return EXIT_SUCCESS;
}
