/****************************************************************************
** Copyright (c) 2016-2017, Carsten Schmidt. All rights reserved.
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

#ifndef __CSCONVERT_H__
#define __CSCONVERT_H__

#include <cstdint>

#include <type_traits>

#include <emmintrin.h> // SSE2
#include <smmintrin.h> // SSE4.1
#include <xmmintrin.h> // SSE

/*****************************************************************************
 * Implementation ************************************************************
 *****************************************************************************/

namespace cs {

  namespace convert {

    /*************************************************************************
     * Stride Computation ****************************************************
     *************************************************************************/

    template<typename DataT>
    struct Stride {
      enum Values {
        // Elements: Number of Data Elements in SIMD Register
        Elements      = sizeof(__m128) / sizeof(DataT),
        // Blocks: Number of EPI32 Conversions per Data Elements in SIMD Register
        Blocks        = Elements / 4,
        // Prefetch Strategy
        CacheLine     = 64,
        PrefetchSkip  = CacheLine / sizeof(__m128),
        PrefetchWidth = CacheLine / sizeof(DataT)
      };
    };

    /*************************************************************************
     * Converters to EPI32 ***************************************************
     *************************************************************************/

    template<typename DataT>
    struct Converter {
      // SFINAE
    };

    template<>
    struct Converter<int8_t> {
      inline static __m128i run(const __m128i& data)
      {
        return _mm_cvtepi8_epi32(data);
      }
    };

    template<>
    struct Converter<uint8_t> {
      inline static __m128i run(const __m128i& data)
      {
        return _mm_cvtepu8_epi32(data);
      }
    };

    template<>
    struct Converter<int16_t> {
      inline static __m128i run(const __m128i& data)
      {
        return _mm_cvtepi16_epi32(data);
      }
    };

    template<>
    struct Converter<uint16_t> {
      inline static __m128i run(const __m128i& data)
      {
        return _mm_cvtepu16_epi32(data);
      }
    };

    template<>
    struct Converter<int32_t> {
      inline static __m128i run(const __m128i& data)
      {
        return data;
      }
    };

    /*************************************************************************
     * Block Helper **********************************************************
     *************************************************************************/

    template<typename DataT>
    struct BlockHelper {
      inline static __m128d create()
      {
        return _mm_setzero_pd();
      }
    };

    template<>
    struct BlockHelper<uint32_t> {
      inline static __m128d create()
      {
        return _mm_set1_pd(65536.0);
      }
    };

    /*************************************************************************
     * Block Reduction *******************************************************
     *************************************************************************/

    template<typename DataT, int i>
    struct Block {
      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& helper)
      {
        Block<DataT,0>::run(dest, raw, helper);
        Block<DataT,i-1>::run(&dest[4], _mm_srli_si128(raw, 4*sizeof(DataT)), helper);
      }

      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& c1, const __m128d& c0,
                             const __m128d& helper)
      {
        Block<DataT,0>::run(dest, raw, c1, c0, helper);
        Block<DataT,i-1>::run(&dest[4], _mm_srli_si128(raw, 4*sizeof(DataT)), c1, c0, helper);
      }
    };

    template<typename DataT>
    struct Block<DataT,0> {
      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& /*helper*/)
      {
        __m128i data = Converter<DataT>::run(raw);
        _mm_stream_pd(&dest[0], _mm_cvtepi32_pd(data));

        data = _mm_srli_si128(data, 2*sizeof(int32_t));
        _mm_stream_pd(&dest[2], _mm_cvtepi32_pd(data));
      }

      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& c1, const __m128d& c0,
                             const __m128d& /*helper*/)
      {
        __m128i data = Converter<DataT>::run(raw);
        const __m128d result01 = _mm_cvtepi32_pd(data);
        _mm_stream_pd(&dest[0], _mm_add_pd(_mm_mul_pd(c1, result01), c0));

        data = _mm_srli_si128(data, 2*sizeof(int32_t));
        const __m128d result23 = _mm_cvtepi32_pd(data);
        _mm_stream_pd(&dest[2], _mm_add_pd(_mm_mul_pd(c1, result23), c0));
      }
    };

    template<>
    struct Block<uint32_t,0> {
      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& helper)
      {
        // hi = raw >> 16
        __m128i hi = _mm_srli_epi32(raw, 16);
        // lo = (raw << 16) >> 16
        __m128i lo = _mm_srli_epi32(_mm_slli_epi32(raw, 16), 16);

        // dest = hi*helper + lo
        _mm_stream_pd(&dest[0], _mm_add_pd(_mm_mul_pd(_mm_cvtepi32_pd(hi),
                                                      helper),
                                           _mm_cvtepi32_pd(lo)));

        hi = _mm_srli_si128(hi, 2*sizeof(int32_t));
        lo = _mm_srli_si128(lo, 2*sizeof(int32_t));

        // dest = hi*helper + lo
        _mm_stream_pd(&dest[2], _mm_add_pd(_mm_mul_pd(_mm_cvtepi32_pd(hi),
                                                      helper),
                                           _mm_cvtepi32_pd(lo)));
      }

      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& c1, const __m128d& c0,
                             const __m128d& helper)
      {
        // hi = raw >> 16
        __m128i hi = _mm_srli_epi32(raw, 16);
        // lo = (raw << 16) >> 16
        __m128i lo = _mm_srli_epi32(_mm_slli_epi32(raw, 16), 16);

        // result = hi*helper + lo
        const __m128d result01 = _mm_add_pd(_mm_mul_pd(_mm_cvtepi32_pd(hi),
                                                       helper),
                                            _mm_cvtepi32_pd(lo));
        _mm_stream_pd(&dest[0], _mm_add_pd(_mm_mul_pd(c1, result01), c0));

        hi = _mm_srli_si128(hi, 2*sizeof(int32_t));
        lo = _mm_srli_si128(lo, 2*sizeof(int32_t));

        // result = hi*helper + lo
        const __m128d result23 = _mm_add_pd(_mm_mul_pd(_mm_cvtepi32_pd(hi),
                                                       helper),
                                            _mm_cvtepi32_pd(lo));
        _mm_stream_pd(&dest[2], _mm_add_pd(_mm_mul_pd(c1, result23), c0));
      }
    };

    template<>
    struct Block<float,0> {
      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& /*helper*/)
      {
        _mm_stream_pd(&dest[0], _mm_cvtps_pd(_mm_castsi128_ps(raw)));

        _mm_stream_pd(&dest[2], _mm_cvtps_pd(_mm_castsi128_ps(_mm_srli_si128(raw, 2*sizeof(float)))));
      }

      inline static void run(double *dest, const __m128i& raw,
                             const __m128d& c1, const __m128d& c0,
                             const __m128d& /*helper*/)
      {
        const __m128d result01 = _mm_cvtps_pd(_mm_castsi128_ps(raw));
        _mm_stream_pd(&dest[0], _mm_add_pd(_mm_mul_pd(c1, result01), c0));

        const __m128d result23 = _mm_cvtps_pd(_mm_castsi128_ps(_mm_srli_si128(raw, 2*sizeof(float))));
        _mm_stream_pd(&dest[2], _mm_add_pd(_mm_mul_pd(c1, result23), c0));
      }
    };

  } // namespace convert

} // namespace cs

/*****************************************************************************
 * User Interface ************************************************************
 *****************************************************************************/

template<typename DataT>
void csConvert(double *dest, const DataT *src, const int N,
               typename std::enable_if<std::is_arithmetic<DataT>::value>::type * = 0)
{
  using namespace cs::convert;

  const int count = N / Stride<DataT>::Elements;
  if( count > 0 ) {
    const __m128d helper = BlockHelper<DataT>::create();

    for(int i = 0; i < count; i++) {
      if( i % Stride<DataT>::PrefetchSkip == 0 ) {
        _mm_prefetch(reinterpret_cast<const char*>(&dest[Stride<DataT>::PrefetchWidth]), _MM_HINT_NTA);
      }

      const __m128i raw = _mm_load_si128(reinterpret_cast<const __m128i*>(src));

      Block<DataT,Stride<DataT>::Blocks-1>::run(dest, raw, helper);

      dest += Stride<DataT>::Elements;
      src  += Stride<DataT>::Elements;
    }

    _mm_sfence();
  }

  const int remain = N % Stride<DataT>::Elements;
  for(int i = 0; i < remain; i++) {
    *dest++ = *src++;
  }
}

template<typename DataT>
void csConvert(double *dest, const DataT *src, const int N,
               const double c1, const double c0,
               typename std::enable_if<std::is_arithmetic<DataT>::value>::type * = 0)
{
  using namespace cs::convert;

  const int count = N / Stride<DataT>::Elements;
  if( count > 0 ) {
    const __m128d   c1mm = _mm_set1_pd(c1);
    const __m128d   c0mm = _mm_set1_pd(c0);
    const __m128d helper = BlockHelper<DataT>::create();

    for(int i = 0; i < count; i++) {
      if( i % Stride<DataT>::PrefetchSkip == 0 ) {
        _mm_prefetch(reinterpret_cast<const char*>(&dest[Stride<DataT>::PrefetchWidth]), _MM_HINT_NTA);
      }

      const __m128i raw = _mm_load_si128(reinterpret_cast<const __m128i*>(src));

      Block<DataT,Stride<DataT>::Blocks-1>::run(dest, raw, c1mm, c0mm, helper);

      dest += Stride<DataT>::Elements;
      src  += Stride<DataT>::Elements;
    }

    _mm_sfence();
  }

  const int remain = N % Stride<DataT>::Elements;
  for(int i = 0; i < remain; i++) {
    *dest++ = c1 * *src++ + c0;
  }
}

#endif // __CSCONVERT_H__
