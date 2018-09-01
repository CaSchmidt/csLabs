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

#ifndef CSFLAGS_H
#define CSFLAGS_H

#include <csUtil/csTypeTraits.h>

/*
 * NOTE:
 * This implementation is heavily inspired by Anthony Williams' article
 * "Using Enum Classes as Bitfields".
 * cf. https://accu.org/var/uploads/journals/Overload132.pdf#page=23
 */

namespace cs {

  template<typename T>
  struct is_flags {
    enum : bool {
      value = false
    };
  };

  template<typename T>
  using if_flags_t = typename std::enable_if<
  std::is_enum<T>::value  &&
  std::is_unsigned<safe_underlying_type_t<T>>::value  &&
  is_flags<T>::value,
  T>::type;

  template<typename T>
  using if_flags_bool = typename std::enable_if<
  std::is_enum<T>::value  &&
  std::is_unsigned<safe_underlying_type_t<T>>::value  &&
  is_flags<T>::value,
  bool>::type;

  template<typename T>
  constexpr if_flags_t<T>& setFlags(T& result, const T& flags, const bool on = true)
  {
    if( on ) {
      result |=  flags;
    } else {
      result &= ~flags;
    }
    return result;
  }

  template<typename T>
  constexpr if_flags_bool<T> testFlag(const T& x, const T& flag)
  {
    using data_t = safe_underlying_type_t<T>;
    return (static_cast<data_t>(x) & static_cast<data_t>(flag)) != 0;
  }

  template<typename T>
  constexpr if_flags_bool<T> testMask(const T& x, const T& mask)
  {
    using data_t = safe_underlying_type_t<T>;
    return (static_cast<data_t>(x) & static_cast<data_t>(mask)) == static_cast<data_t>(mask);
  }

} // namespace cs

// AND ///////////////////////////////////////////////////////////////////////

template<typename T>
constexpr cs::if_flags_t<T> operator&(const T& a, const T& b)
{
  using data_t = cs::safe_underlying_type_t<T>;
  return static_cast<T>(static_cast<data_t>(a) & static_cast<data_t>(b));
}

template<typename T>
constexpr cs::if_flags_t<T>& operator&=(T& result, const T& a)
{
  using data_t = cs::safe_underlying_type_t<T>;
  result = static_cast<T>(static_cast<data_t>(result) & static_cast<data_t>(a));
  return result;
}

// NOT ///////////////////////////////////////////////////////////////////////

template<typename T>
constexpr cs::if_flags_t<T> operator~(const T& a)
{
  using data_t = cs::safe_underlying_type_t<T>;
  return static_cast<T>(~static_cast<data_t>(a));
}

// OR ////////////////////////////////////////////////////////////////////////

template<typename T>
constexpr cs::if_flags_t<T> operator|(const T& a, const T& b)
{
  using data_t = cs::safe_underlying_type_t<T>;
  return static_cast<T>(static_cast<data_t>(a) | static_cast<data_t>(b));
}

template<typename T>
constexpr cs::if_flags_t<T>& operator|=(T& result, const T& a)
{
  using data_t = cs::safe_underlying_type_t<T>;
  result = static_cast<T>(static_cast<data_t>(result) | static_cast<data_t>(a));
  return result;
}

// XOR ///////////////////////////////////////////////////////////////////////

template<typename T>
constexpr cs::if_flags_t<T> operator^(const T& a, const T& b)
{
  using data_t = cs::safe_underlying_type_t<T>;
  return static_cast<T>(static_cast<data_t>(a) ^ static_cast<data_t>(b));
}

template<typename T>
constexpr cs::if_flags_t<T>& operator^=(T& result, const T& a)
{
  using data_t = cs::safe_underlying_type_t<T>;
  result = static_cast<T>(static_cast<data_t>(result) ^ static_cast<data_t>(a));
  return result;
}

// Macros ////////////////////////////////////////////////////////////////////

#define CS_ENABLE_FLAGS(T) \
  namespace cs {           \
    template<>             \
    struct is_flags<T> {   \
      enum : bool {        \
        value = true       \
      };                   \
    };                     \
  }

#endif // CSFLAGS_H
