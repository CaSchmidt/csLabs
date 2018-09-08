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

#include <algorithm>
#include <string>

#include <csUtil/csCharUtil.h>

namespace cs {

  ////// Types ///////////////////////////////////////////////////////////////

  template<typename T>
  using if_string_t = typename std::enable_if<is_char<T>::value,std::basic_string<T>>::type;

  ////// Implementation //////////////////////////////////////////////////////

  template<typename T>
  inline std::size_t length(const T *s,
                            const if_char_bool<T> * = nullptr) noexcept
  {
    return s != nullptr
        ? std::char_traits<T>::length(s)
        : 0;
  }

  template<typename T>
  inline std::size_t lengthN(const T *s, const std::size_t maxCount,
                            const if_char_bool<T> * = nullptr) noexcept
  {
    std::size_t count = 0;

    if( s == nullptr ) {
      return count;
    }

    const T  *ptr = s;
    const T *last = ptr + maxCount;
    for(; ptr < last; count++, ptr++) {
      if( *ptr == glyph<T>::null ) {
        break;
      }
    }

    return count;
  }

  template<typename T>
  inline std::size_t lengthDiff(const T *first, const T *last,
                            const if_char_bool<T> * = nullptr) noexcept
  {
    return first != nullptr  &&  first < last
        ? lengthN(first, static_cast<std::size_t>(last - first))
        : 0;
  }

  template<typename T>
  inline if_char_bool<T> equals(const T *s1, const char *s2,
                                const std::size_t _count = MAX_SIZE_T) noexcept
  {
    const std::size_t count = _count == MAX_SIZE_T
        ? std::min(length(s1), length(s2))
        : _count;
    return std::char_traits<T>::compare(s1, s2, count) == 0;
  }

  template<typename T>
  inline if_char_bool<T> greater(const T *s1, const char *s2,
                                 const std::size_t _count = MAX_SIZE_T) noexcept
  {
    const std::size_t count = _count == MAX_SIZE_T
        ? std::min(length(s1), length(s2))
        : _count;
    return std::char_traits<T>::compare(s1, s2, count) > 0;
  }

  template<typename T>
  if_char_bool<T> less(const T *s1, const char *s2,
                       const std::size_t _count = MAX_SIZE_T) noexcept
  {
    const std::size_t count = _count == MAX_SIZE_T
        ? std::min(length(s1), length(s2))
        : _count;
    return std::char_traits<T>::compare(s1, s2, count) < 0;
  }

  template<typename T>
  inline if_string_t<T> simplified(const std::basic_string<T>& s)
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

  template<typename T>
  inline if_string_t<T> trimmed(const std::basic_string<T>& s)
  {
    using const_iter = typename std::basic_string<T>::const_iterator;

    std::basic_string<T> result;

    // (1) Scan begin ////////////////////////////////////////////////////////

    const_iter beg = s.cend();
    for(const_iter it = s.cbegin(); it != s.cend(); it++) {
      if( !isSpace(*it) ) {
        beg = it;
        break;
      }
    }

    if( beg == s.cend() ) {
      return result;
    }

    // (2) Scan end //////////////////////////////////////////////////////////

    const_iter end = s.cend();
    for(const_iter it = s.cend(); it != beg; it--) {
      if( !isSpace(*(it - 1)) ) {
        end = it;
        break;
      }
    }

    if( end == beg ) {
      return result;
    }

    // (3) Copy //////////////////////////////////////////////////////////////

    try {
      result.assign(beg, end);
    } catch(...) {
      result.clear();
      return result;
    }

    return result;
  }

} // namespace cs

#endif // CSSTRINGUTIL_H
