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

#ifndef __UTIL_ALPHANUM_H__
#define __UTIL_ALPHANUM_H__

#include <csCore/cscore_config.h>

namespace priv_a {

  template<typename T>
  inline int toDigit(const T ch, const int base)
  {
    int dig = -1;
    if(        (int)'0' <= (int)ch  &&  (int)ch <= (int)'9' ) {
      dig = (int)ch - (int)'0';
    } else if( (int)'a' <= (int)ch  &&  (int)ch <= (int)'z' ) {
      dig = (int)ch - (int)'a' + 10;
    } else if( (int)'A' <= (int)ch  &&  (int)ch <= (int)'Z' ) {
      dig = (int)ch - (int)'A' + 10;
    }
    if( dig > base ) {
      return -1;
    }
    return dig;
  }

#define BASE  ((uint32_t)base)
#define DIG   ((uint32_t)dig)

  template<typename T>
  inline uint32_t toUInt(const T *s, bool *ok, const int base, const uint32_t max)
  {
    if( ok != 0 ) {
      *ok = false;
    }

    if( s == 0  ||  base < 2  ||  base > 36 ) {
      return 0;
    }

    const uint32_t mulGuard = max / BASE;
    const uint32_t remGuard = max % BASE;

    uint32_t num = 0;

    int dig;
    int i = 0;
    while( (dig = toDigit(s[i++], base)) >= 0 ) {
      if( num > mulGuard ) {
        return 0;
      }

      if( num == mulGuard  &&  DIG > remGuard ) {
        return 0;
      }

      num *= BASE;
      num += DIG;
    }

    if( ok != 0 ) {
      *ok = true;
    }

    return num;
  }

#undef BASE
#undef DIG

}; // namespace priv_a

#endif // __UTIL_ALPHANUM_H__
