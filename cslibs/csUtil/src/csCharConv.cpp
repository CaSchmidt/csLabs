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

#include <limits>
#include <type_traits>

#include "csUtil/csCharConv.h"

namespace cs {

  namespace charconv {

    // from_chars() Implementation ///////////////////////////////////////////

    template<typename CharT>
    struct from_chars_result {
      const CharT *ptr;
      std::errc ec;
    };

    template<typename CharT>
    inline int to_digit(const CharT ch, const int base)
    {
      const int v_0('0');   const int v_9('9');
      const int v_a('a');   const int v_z('z');
#ifdef CS_FROM_CHARS_HAVE_CAPS
      const int v_A('A');   const int v_Z('Z');
#endif
      const int chval(ch);

      int dig = -1;
      if(        v_0 <= chval  &&  chval <= v_9 ) {
        dig = chval - v_0;
      } else if( v_a <= chval  &&  chval <= v_z ) {
        dig = chval - v_a + 10;
#ifdef CS_FROM_CHARS_HAVE_CAPS
      } else if( v_A <= chval  &&  chval <= v_Z ) {
        dig = chval - v_A + 10;
#endif
      }
      if( dig >= base ) {
        dig = -1;
      }
      return dig;
    }

    template<typename ValueT, typename CharT>
    inline from_chars_result<CharT> to_unsigned(const CharT *first, const CharT *last,
                                                ValueT& value, const int base,
                                                const ValueT max,
                                                typename std::enable_if<
                                                std::is_integral<ValueT>::value  &&
                                                std::is_unsigned<ValueT>::value
                                                >::type * = nullptr)
    {
      if( first == nullptr  ||  last == nullptr  ||  last <= first  ||
          base < 2  ||  base > 36 ) {
        return from_chars_result<CharT>{first, std::errc::invalid_argument};
      }

      const ValueT   mulBase = static_cast<ValueT>(base);
      const ValueT  mulGuard = max / mulBase;
      const ValueT mulRemain = max % mulBase;

      int   digit = -1;
      ValueT conv =  0;
      const CharT *ptr = nullptr;
      for(ptr = first;
          ptr < last  &&  (digit = to_digit(*ptr, base)) != -1;
          ptr++) {
        if( conv > mulGuard  ||
            (conv == mulGuard  &&  static_cast<ValueT>(digit) > mulRemain) ) {
          return from_chars_result<CharT>{ptr, std::errc::result_out_of_range};
        }

        conv *= mulBase;
        conv += static_cast<ValueT>(digit);
      } // For Each Character

      if( ptr == first ) { // No Conversion Occured!
        return from_chars_result<CharT>{first, std::errc::invalid_argument};
      }

      value = conv;

      return from_chars_result<CharT>{ptr, std::errc()};
    }

    template<typename ValueT, typename CharT>
    inline from_chars_result<CharT> to_signed(const CharT *first, const CharT *last,
                                              ValueT& value, const int base,
                                              typename std::enable_if<
                                              std::is_integral<ValueT>::value  &&
                                              std::is_signed<ValueT>::value
                                              >::type * = nullptr)
    {
      typedef typename std::make_unsigned<ValueT>::type UnsignedT;

      if( first == nullptr  ||  last == nullptr  ||  last <= first  ||
          base < 2  ||  base > 36 ) {
        return from_chars_result<CharT>{first, std::errc::invalid_argument};
      }

      bool is_negative = false;
      if(        *first == CharT('-') ) {
        is_negative = true;
        first++;
#ifdef CS_FROM_CHARS_HAVE_PLUS
      } else if( *first == CharT('+') ) {
        first++;
#endif
      }

      UnsignedT max = std::numeric_limits<ValueT>::max();
      if( is_negative ) {
        max++;
      }

      UnsignedT conv = 0;
      const from_chars_result<CharT> result =
          to_unsigned(first, last, conv, base, max);

      if( result.ec == std::errc() ) {
        if( is_negative ) {
          conv ^= std::numeric_limits<UnsignedT>::max();
          conv++;
        }
        value = static_cast<ValueT>(conv);
      }

      return result;
    }

    // to_chars() Implementation /////////////////////////////////////////////

    template<typename CharT>
    struct to_chars_result {
      CharT *ptr;
      std::errc ec;
    };

    template<typename CharT>
    inline CharT to_char(const int digit)
    {
      if(         0 <= digit  &&  digit <=  9 ) {
        return CharT('0') + CharT(digit);
      } else if( 10 <= digit  &&  digit <  36 ) {
        return CharT('a') + CharT(digit - 10);
      }
      return CharT(0);
    }

    template<typename ValueT>
    inline int required_size(ValueT value, const int base,
                             typename std::enable_if<
                             std::is_integral<ValueT>::value  &&
                             std::is_unsigned<ValueT>::value
                             >::type * = nullptr)
    {
      if( base < 2  ||  base > 36 ) {
        return 0;
      }

      int result = 0;
      do {
        result++;
        value /= static_cast<ValueT>(base);
      } while( value != 0 );
      return result;
    }

    template<typename ValueT, typename CharT>
    inline to_chars_result<CharT> from_unsigned(CharT *first, CharT *last,
                                                const ValueT value, const int base,
                                                typename std::enable_if<
                                                std::is_integral<ValueT>::value  &&
                                                std::is_unsigned<ValueT>::value
                                                >::type * = nullptr)
    {
      if( first == nullptr  ||  last == nullptr  ||  last <= first  ||
          base < 2  ||  base > 36 ) {
        return to_chars_result<CharT>{last, std::errc::invalid_argument};
      }

      const int reqSize = required_size(value, base);
      if( last - first < static_cast<std::ptrdiff_t>(reqSize) ) {
        return to_chars_result<CharT>{last, std::errc::value_too_large};
      }

      ValueT conv = value;
      CharT  *ptr = first + reqSize - 1;
      do {
        *ptr-- = to_char<CharT>(static_cast<int>(conv % base));
        conv  /= static_cast<ValueT>(base);
      } while( conv != 0 );

      return to_chars_result<CharT>{first + reqSize, std::errc()};
    }

    template<typename ValueT, typename CharT>
    inline to_chars_result<CharT> from_signed(CharT *first, CharT *last,
                                              const ValueT value, const int base,
                                              typename std::enable_if<
                                              std::is_integral<ValueT>::value  &&
                                              std::is_signed<ValueT>::value
                                              >::type * = nullptr)
    {
      typedef typename std::make_unsigned<ValueT>::type UnsignedT;

      if( first == nullptr  ||  last == nullptr  ||  last <= first  ||
          base < 2  ||  base > 36 ) {
        return to_chars_result<CharT>{last, std::errc::invalid_argument};
      }

      const bool is_negative = value < 0;

      UnsignedT conv = static_cast<UnsignedT>(value);
      if( is_negative ) {
        conv ^= std::numeric_limits<UnsignedT>::max();
        conv++;
      }

      int reqSize = required_size(conv, base);
      if( is_negative ) {
        reqSize++;
      }

      if( last - first < static_cast<std::ptrdiff_t>(reqSize) ) {
        return to_chars_result<CharT>{last, std::errc::value_too_large};
      }

      if( is_negative ) {
        *first++ = CharT('-');
      }

      return from_unsigned(first, last, conv, base);
    }

  } // namespace charconv

  ////// Public //////////////////////////////////////////////////////////////

  // from_chars() ////////////////////////////////////////////////////////////

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int8_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_signed(first, last, value, base);
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int16_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_signed(first, last, value, base);
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int32_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_signed(first, last, value, base);
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int64_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_signed(first, last, value, base);
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint8_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_unsigned(first, last, value, base,
                              std::numeric_limits<uint8_t>::max());
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint16_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_unsigned(first, last, value, base,
                              std::numeric_limits<uint16_t>::max());
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint32_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_unsigned(first, last, value, base,
                              std::numeric_limits<uint32_t>::max());
    return from_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint64_t& value, int base)
  {
    const charconv::from_chars_result<char> result =
        charconv::to_unsigned(first, last, value, base,
                              std::numeric_limits<uint64_t>::max());
    return from_chars_result{result.ptr, result.ec};
  }

  // to_chars() //////////////////////////////////////////////////////////////

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int8_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_signed(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int16_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_signed(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int32_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_signed(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int64_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_signed(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint8_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_unsigned(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint16_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_unsigned(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint32_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_unsigned(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint64_t value, int base)
  {
    const charconv::to_chars_result<char> result =
        charconv::from_unsigned(first, last, value, base);
    return to_chars_result{result.ptr, result.ec};
  }

} // namespace cs
