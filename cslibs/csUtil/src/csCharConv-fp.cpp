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
#include <cstring>

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

    template<typename ValueT>
    inline ValueT to_ieee(const double_conversion::StringToDoubleConverter& conv,
                          const char *buffer, const int length, int *processed,
                          typename std::enable_if<
                          std::is_same<ValueT,double>::value
                          >::type * = nullptr)
    {
      return conv.StringToDouble(buffer, length, processed);
    }

    template<typename ValueT>
    inline ValueT to_ieee(const double_conversion::StringToDoubleConverter& conv,
                          const char *buffer, const int length, int *processed,
                          typename std::enable_if<
                          std::is_same<ValueT,float>::value
                          >::type * = nullptr)
    {
      return conv.StringToFloat(buffer, length, processed);
    }

    inline int from_flags(const chars_format /*fmt*/)
    {
      return double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK;
    }

    template<typename ValueT>
    inline from_chars_result to_fp(const char *first, const char *last,
                                   ValueT& value,
                                   chars_format fmt,
                                   typename std::enable_if<
                                   std::is_floating_point<ValueT>::value
                                   >::type * = nullptr)
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
      const ValueT v = to_ieee<ValueT>(conv, first, length(first, last), &processed);

      if( std::isnan(v)  &&  processed < 3 ) { // Empty or Junk String
        return from_chars_result{first, std::errc::invalid_argument};
      }

      value = v;

      return from_chars_result{first+processed, std::errc()};
    }

    // to_chars() Implementation /////////////////////////////////////////////

    // NOTE: Operates on [begin,end)
    char *find_dot(char *begin, char *end, const char ch_dot)
    {
      for(char *ptr = begin; ptr < end; ptr++) {
        if( *ptr == ch_dot ) {
          return ptr;
        }
      }
      return nullptr;
    }

    // NOTE: Operates on [begin,end)
    char *find_exp(char *begin, char *end, const char ch_exp)
    {
      if( begin != nullptr ) {
        for(char *ptr = end - 1; ptr >= begin; ptr--) {
          if( *ptr == ch_exp ) {
            return ptr;
          }
        }
      }
      return nullptr;
    }

    /*
     * NOTE:
     * begin -> the position of the decimal dot
     *   end -> the position of the exponent OR '\0'
     */
    void remove_trailing_zeros(char *begin, char *end, const char ch_dot)
    {
      if( begin == nullptr ) {
        return;
      }

      char *ptr = end - 1;
      for(; ptr >= begin; ptr--) {
        if( *ptr != '0' ) {
          break;
        }
      }

      if( ptr == end - 1 ) {
        return;
      }

      if( *ptr != ch_dot ) {
        ptr++;
      }

      while( *end != '\0' ) {
        *ptr++ = *end++;
      }

      *ptr = '\0';
    }

    template<typename ValueT>
    inline void from_ieee_shortest(const double_conversion::DoubleToStringConverter& conv,
                                   const ValueT value,
                                   double_conversion::StringBuilder *builder,
                                   typename std::enable_if<
                                   std::is_same<ValueT,double>::value
                                   >::type * = nullptr)
    {
      conv.ToShortest(value, builder);
    }

    template<typename ValueT>
    inline void from_ieee_shortest(const double_conversion::DoubleToStringConverter& conv,
                                   const ValueT value,
                                   double_conversion::StringBuilder *builder,
                                   typename std::enable_if<
                                   std::is_same<ValueT,float>::value
                                   >::type * = nullptr)
    {
      conv.ToShortestSingle(value, builder);
    }

    template<typename ValueT>
    inline to_chars_result from_fp(char *first, char *last,
                                   const ValueT value,
                                   const chars_format fmt,
                                   int precision,
                                   typename std::enable_if<
                                   std::is_floating_point<ValueT>::value
                                   >::type * = nullptr)
    {
      // Frequently Used Constants ///////////////////////////////////////////

      const char ch_dot = '.';
      const char ch_exp = 'e';

      const int max_ldg_zeros = 4;
      const int max_trl_zeros = 0;

      // sign + decimal point
      // e.g. -d.d
      const int min_fixed = 2;
      // sign + decimal point + exponent + exponent's sign + exponent's 3 digits
      // e.g. -d.dE-ddd
      const int min_expon = 7;

      // (1) Sanity Check ////////////////////////////////////////////////////

      if( first == nullptr  ||  last == nullptr  ||  last <= first ) {
        return to_chars_result{first, std::errc::invalid_argument};
      }

      // (2) Setup ///////////////////////////////////////////////////////////

      // cf. http://en.cppreference.com/w/cpp/io/c/fprintf
      if(        precision <  0 ) {
        precision = 6;
      } else if( precision == 0 ) {
        precision = 1;
      }

      double_conversion::StringBuilder builder(first, length(first, last));

      const int flags =
          double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN |
          double_conversion::DoubleToStringConverter::UNIQUE_ZERO;

      const double_conversion::DoubleToStringConverter conv(flags,
                                                            "inf", "nan", ch_exp,
                                                            -4, 6,
                                                            max_ldg_zeros,
                                                            max_trl_zeros);

      // (3) Conversion //////////////////////////////////////////////////////

      bool ok = false;
      if(        fmt == chars_format::general ) {
        if( length(first, last) <
            min_expon + max_ldg_zeros + max_trl_zeros + precision + 1 ) {
          return to_chars_result{first, std::errc::invalid_argument};
        }

        ok = conv.ToPrecision(value, precision, &builder);

      } else if( fmt == chars_format::scientific ) {
        if( length(first, last) <
            min_expon + (1 + precision) + 1 ) {
          return to_chars_result{first, std::errc::invalid_argument};
        }

        ok = conv.ToExponential(value, precision, &builder);

      } else {
        if( length(first, last) <
            min_fixed + double_conversion::DoubleToStringConverter::kMaxFixedDigitsBeforePoint + precision + 1 ) {
          return to_chars_result{first, std::errc::invalid_argument};
        }

        ok = conv.ToFixed(value, precision, &builder);

      }
      builder.Finalize();

      if( !ok ) {
        return to_chars_result{first, std::errc::invalid_argument};
      }

      // (4) Remove Trailing Zeros ///////////////////////////////////////////

#ifdef CS_TO_CHARS_HAVE_REMOVE_TRAILING_ZEROS
      if( fmt == chars_format::general ) {
        const std::size_t len0 = strlen(first);
        char *pos_dot = find_dot(  first, first + len0, ch_dot);
        char *pos_exp = find_exp(pos_dot, first + len0, ch_exp);
        remove_trailing_zeros(pos_dot, pos_exp == nullptr
                              ? first + len0
                              : pos_exp, ch_dot);
      }
#endif

      return to_chars_result{first+strlen(first)+1, std::errc()};
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

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, float value)
  {
    return charconv::from_fp(first, last, value, chars_format::general, -1);
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, double value)
  {
    return charconv::from_fp(first, last, value, chars_format::general, -1);
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, float value,
                                          chars_format fmt)
  {
    return charconv::from_fp(first, last, value, fmt, -1);
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, double value,
                                          chars_format fmt)
  {
    return charconv::from_fp(first, last, value, fmt, -1);
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, float value,
                                          chars_format fmt, int precision)
  {
    return charconv::from_fp(first, last, value, fmt, precision);
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last, double value,
                                          chars_format fmt, int precision)
  {
    return charconv::from_fp(first, last, value, fmt, precision);
  }

} // namespace cs
