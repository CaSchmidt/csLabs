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

#ifndef __CSFLAGS_H__
#define __CSFLAGS_H__

#include <type_traits>

#include <csUtil/csutil_config.h>

template<typename EnumT,
         typename std::enable_if<std::is_enum<EnumT>::value>::type * = nullptr>
class csFlags {
public:
  typedef EnumT enum_type;
  typedef typename std::underlying_type<EnumT>::type value_type;

  csFlags()
    : _value(0)
  {
  }

  explicit csFlags(const value_type value)
    : _value(value)
  {
  }

  csFlags(const EnumT flag)
    : _value(flag)
  {
  }

  csFlags(const csFlags<EnumT>& other)
    : _value(other._value)
  {
  }

  ~csFlags()
  {
  }

  inline csFlags<EnumT>& operator=(const csFlags<EnumT>& other)
  {
    if( this != &other ) {
      _value = other._value;
    }
    return *this;
  }

  inline bool operator!() const
  {
    return _value == 0;
  }

  inline csFlags<EnumT> operator~() const
  {
    return csFlags<EnumT>(~_value);
  }

  // AND /////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator&=(const csFlags<EnumT>& other)
  {
    _value &= other._value;
    return *this;
  }

  inline csFlags<EnumT> operator&(const csFlags<EnumT>& other) const
  {
    return csFlags<EnumT>(_value & other._value);
  }

  // OR //////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator|=(const csFlags<EnumT>& other)
  {
    _value |= other._value;
    return *this;
  }

  inline csFlags<EnumT> operator|(const csFlags<EnumT>& other) const
  {
    return csFlags<EnumT>(_value | other._value);
  }

  // XOR /////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator^=(const csFlags<EnumT>& other)
  {
    _value ^= other._value;
    return *this;
  }

  inline csFlags<EnumT> operator^(const csFlags<EnumT>& other) const
  {
    return csFlags<EnumT>(_value ^ other._value);
  }

  // Set /////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& setFlag(const EnumT flag, const bool on = true)
  {
    if( on ) {
      _value |=  static_cast<value_type>(flag);
    } else {
      _value &= ~static_cast<value_type>(flag);
    }
    return *this;
  }

  // Tests ///////////////////////////////////////////////////////////////////

  inline bool testFlag(const EnumT flag) const
  {
    return (_value & static_cast<value_type>(flag)) == static_cast<value_type>(flag);
  }

  inline bool testFlags(const csFlags<EnumT>& other) const
  {
    return (_value & other._value) == other._value;
  }

#ifdef CS_FLAGS_HAVE_VALUE_OPERATORS
  inline operator value_type() const
  {
    return _value;
  }

  // AND /////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator&=(const value_type value)
  {
    _value &= value;
    return *this;
  }

  inline csFlags<EnumT> operator&(const value_type value) const
  {
    return csFlags<EnumT>(_value & value);
  }

  // OR //////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator|=(const value_type value)
  {
    _value |= value;
    return *this;
  }

  inline csFlags<EnumT> operator|(const value_type value) const
  {
    return csFlags<EnumT>(_value | value);
  }

  // XOR /////////////////////////////////////////////////////////////////////

  inline csFlags<EnumT>& operator^=(const value_type value)
  {
    _value ^= value;
    return *this;
  }

  inline csFlags<EnumT> operator^(const value_type value) const
  {
    return csFlags<EnumT>(_value ^ value);
  }
#endif

private:
  value_type _value;
};

#define CS_DECLARE_OPERATORS_FOR_FLAGS(Flags)       \
  inline Flags operator|(const Flags::enum_type a,  \
                         const Flags::enum_type b)  \
  {                                                 \
    Flags result(a);                                \
    result |= b;                                    \
    return result;                                  \
  }                                                 \
  inline Flags operator|(const Flags::enum_type a,  \
                         const Flags& b)            \
  {                                                 \
    Flags result(a);                                \
    result |= b;                                    \
    return result;                                  \
  }

#endif // __CSFLAGS_H__
