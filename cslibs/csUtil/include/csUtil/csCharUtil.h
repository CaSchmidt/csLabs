/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
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

#ifndef CSCHARUTIL_H
#define CSCHARUTIL_H

#include <csUtil/csTypeTraits.h>

namespace cs {

  template<typename T>
  using if_char_t = typename std::enable_if<is_char<T>::value,T>::type;

  template<typename T>
  using if_char_bool = typename std::enable_if<is_char<T>::value,bool>::type;

  template<typename T>
  constexpr if_char_bool<T> isDigit(const T& c)
  {
    return static_cast<T>('0') <= c  &&  c <=  static_cast<T>('9');
  }

  template<typename T>
  constexpr if_char_bool<T> isSpace(const T& c)
  {
    return
        c == static_cast<T>(' ')   || // space
        c == static_cast<T>('\f')  || // form feed
        c == static_cast<T>('\n')  || // line feed
        c == static_cast<T>('\r')  || // carriage return
        c == static_cast<T>('\t')  || // horizontal tab
        c == static_cast<T>('\v');    // vertical tab
  }

} // namespace cs

#endif // CSCHARUTIL_H
