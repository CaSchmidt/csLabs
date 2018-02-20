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

#include <cmath>

#include <limits>

#include "3rdparty/double-conversion/double-conversion.h"

#include "csUtil/csCharConv.h"

namespace cs {

  namespace charconv {

    constexpr int length(const char *first, const char *last)
    {
      return static_cast<int>(last - first);
    }

    // from_chars() Implementation ///////////////////////////////////////////

    template<typename IeeeT>
    struct to_ieee {
      static constexpr IeeeT run(const double_conversion::StringToDoubleConverter& c,
                                 const char *buffer,
                                 const int length,
                                 int *processed)
      {
        // SFINAE
      }
    };

    template<>
    struct to_ieee<double> {
      static constexpr double run(const double_conversion::StringToDoubleConverter& c,
                                  const char *buffer,
                                  const int length,
                                  int *processed)
      {
        return c.StringToDouble(buffer, length, processed);
      }
    };

    template<>
    struct to_ieee<float> {
      static constexpr float run(const double_conversion::StringToDoubleConverter& c,
                                 const char *buffer,
                                 const int length,
                                 int *processed)
      {
        return c.StringToFloat(buffer, length, processed);
      }
    };

    inline int from_flags(const chars_format /*fmt*/)
    {
      return double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK;
    }

    template<typename T>
    inline from_chars_result to_fp(const char *first, const char *last,
                                   T& value,
                                   chars_format fmt)
    {
      if( first == nullptr  ||  last == nullptr  ||  last <= first ) {
        return from_chars_result{first, std::errc::invalid_argument};
      }

      const int flags = from_flags(fmt);
      const double_conversion::StringToDoubleConverter
          conv(flags,
               std::numeric_limits<double>::quiet_NaN(),
               std::numeric_limits<double>::quiet_NaN(),
               "inf",
               "nan");

      int processed = 0;
      const T v = to_ieee<T>::run(conv, first, length(first, last), &processed);

      if( std::isnan(v)  &&  processed < 3 ) { // Empty or Junk String
        return from_chars_result{first, std::errc::invalid_argument};
      }

      value = v;

      return from_chars_result{first+processed, std::errc()};
    }

  } // namespace charconv

  ////// Public //////////////////////////////////////////////////////////////

  // from_chars() ////////////////////////////////////////////////////////////

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              float& value,
                                              chars_format fmt)
  {
    return charconv::to_fp(first, last, value, fmt);
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              double& value,
                                              chars_format fmt)
  {
    return charconv::to_fp(first, last, value, fmt);
  }

  // to_chars() //////////////////////////////////////////////////////////////

} // namespace cs
