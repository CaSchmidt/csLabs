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

#ifndef CSSTRINGUTIL_H
#define CSSTRINGUTIL_H

#include <string>

#include <csUtil/csCharUtil.h>

namespace cs {

  template<typename T>
  using if_string_type = typename std::enable_if<is_char<T>::value,std::basic_string<T>>::type;

  template<typename T>
  inline if_string_type<T> simplified(const std::basic_string<T>& s)
  {
    using SizeT = typename std::basic_string<T>::size_type;
    constexpr T ch_space = static_cast<T>(' ');

    // (1) Get size //////////////////////////////////////////////////////////

    SizeT numWords = 0;
    SizeT  numSize = 0;
    T prev = ch_space;
    for(const T& curr : s) {
      if( !isSpace(curr) ) { // count characters in WORD only
        numSize++;

        if( isSpace(prev) ) { // transition WHITESPACE -> WORD
          numWords++;
        }
      }

      prev = curr;
    }

    // (2) Create result /////////////////////////////////////////////////////

    std::basic_string<T> result;
    if( numSize < 1 ) {
      return result;
    }

    const SizeT numSpaces = numWords - 1;
    try {
      result.reserve(numSize + numSpaces);
    } catch(...) {
      result.clear();
      return result;
    }

    // (3) Copy //////////////////////////////////////////////////////////////

    SizeT cntSpacesWritten = 0;
    prev = ch_space;
    for(const T& curr : s) {
      if( !isSpace(curr) ) {
        result += curr;
      } else {
        // transition WORD -> WHITESPACE; insert SPACE if necessary
        if( !isSpace(prev)  &&  cntSpacesWritten < numSpaces ) {
          result += ch_space;
          cntSpacesWritten++;
        }
      }

      prev = curr;
    }

    return result;
  }

} // namespace cs

#endif // CSSTRINGUTIL_H
