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

#include "csCore2/csString.h"

#include "csCore2/csAlphaNum.h"
#include "csCore2/csChar.h"
#include "csCore2/csUtil.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv_string {

  template<typename CharT>
  bool endsWith(const csBasicString<CharT>& ref, const CharT *s, const size_t len, const bool ignoreCase)
  {
    if( ignoreCase ) {
      for(size_t i = 0; i < len; i++) {
        if( csToLower(ref[ref.size()-len+i]) != csToLower(s[i]) ) {
          return false;
        }
      }
      return true;
    }
    for(size_t i = 0; i < len; i++) {
      if( ref[ref.size()-len+i] != s[i] ) {
        return false;
      }
    }
    return true;
  }

  template<typename CharT>
  bool startsWith(const csBasicString<CharT>& ref, const CharT *s, const size_t len, const bool ignoreCase)
  {
    if( ignoreCase ) {
      for(size_t i = 0; i < len; i++) {
        if( csToLower(ref[i]) != csToLower(s[i]) ) {
          return false;
        }
      }
      return true;
    }
    for(size_t i = 0; i < len; i++) {
      if( ref[i] != s[i] ) {
        return false;
      }
    }
    return true;
  }

  template<typename CharT>
  size_t stringLen(const CharT *s)
  {
    if( s == 0 ) {
      return 0;
    }
    size_t len = 0;
    while( *s++ ) {
      len++;
    }
    return len;
  }

}; // namespace priv_string

////// Implementation ////////////////////////////////////////////////////////

template<typename CharT>
csBasicString<CharT>::csBasicString()
  : std::basic_string<CharT>()
{
}

template<typename CharT>
csBasicString<CharT>::csBasicString(const csBasicString<CharT>& other)
  : std::basic_string<CharT>(other)
{
}

template<typename CharT>
csBasicString<CharT>::csBasicString(const size_t count, const CharT ch)
  : std::basic_string<CharT>(count, ch)
{
}

template<typename CharT>
csBasicString<CharT>::csBasicString(const CharT *data)
  : std::basic_string<CharT>(data)
{
}

template<typename CharT>
csBasicString<CharT>::csBasicString(const std::basic_string<CharT>& other)
  : std::basic_string<CharT>(other)
{
}

template<typename CharT>
csBasicString<CharT>::~csBasicString()
{
}

template<typename CharT>
csBasicString<CharT>& csBasicString<CharT>::operator=(const csBasicString<CharT>& other)
{
  if( this != &other ) {
    std::basic_string<CharT>::operator=(other);
  }
  return *this;
}

template<typename CharT>
csBasicString<CharT>& csBasicString<CharT>::operator=(const CharT *data)
{
  std::basic_string<CharT>::operator=(data);
  return *this;
}

template<typename CharT>
csBasicString<CharT>& csBasicString<CharT>::operator=(const std::basic_string<CharT>& other)
{
  std::basic_string<CharT>::operator=(other);
  return *this;
}

template<typename CharT>
bool csBasicString<CharT>::contains(const CharT ch, const bool ignoreCase) const
{
  return indexOf(ch, 0, ignoreCase) >= 0;
}

template<typename CharT>
bool csBasicString<CharT>::endsWith(const CharT ch, const bool ignoreCase) const
{
  if( empty() ) {
    return false;
  }
  if( ignoreCase ) {
    return csToLower(operator[](size()-1)) == csToLower(ch);
  }
  return operator[](size()-1) == ch;
}

template<typename CharT>
bool csBasicString<CharT>::endsWith(const CharT *s, const bool ignoreCase) const
{
  if( empty()  ||  priv_string::stringLen(s) < 1  ||
      priv_string::stringLen(s) > size() ) {
    return false;
  }
  return priv_string::endsWith(*this, s, priv_string::stringLen(s), ignoreCase);
}

template<typename CharT>
bool csBasicString<CharT>::endsWith(const csBasicString<CharT>& other,
                                    const bool ignoreCase) const
{
  if( empty()  ||  other.empty()  ||  other.size() > size() ) {
    return false;
  }
  return priv_string::endsWith(*this, other.c_str(), other.size(), ignoreCase);
}

template<typename CharT>
int csBasicString<CharT>::indexOf(const CharT ch, const int from,
                                  const bool ignoreCase) const
{
  const int indexFrom = from < 0
      ? (int)size()+from
      : from;

  if( empty()  ||  indexFrom < 0  ||  (size_t)indexFrom >= size() ) {
    return -1;
  }

  if( ignoreCase ) {
    const CharT needle = csToLower(ch);
    for(size_t i = (size_t)indexFrom; i < size(); i++) {
      if( csToLower(operator[](i)) == needle ) {
        return (int)i;
      }
    }
  } else {
    for(size_t i = (size_t)indexFrom; i < size(); i++) {
      if( operator[](i) == ch ) {
        return (int)i;
      }
    }
  }

  return -1;
}

template<typename CharT>
int csBasicString<CharT>::lastIndexOf(const CharT ch, const int from,
                                      const bool ignoreCase) const
{
  const int indexFrom = from < 0
      ? (int)size()+from
      : from;

  if( empty()  ||  indexFrom < 0  ||  (size_t)indexFrom >= size() ) {
    return -1;
  }

  if( ignoreCase ) {
    const CharT needle = csToLower(ch);
    size_t i = (size_t)indexFrom;
    do {
      if( csToLower(operator[](i)) == needle ) {
        return (int)i;
      }
    } while( i-- );
  } else {
    size_t i = (size_t)indexFrom;
    do {
      if( operator[](i) == ch ) {
        return (int)i;
      }
    } while( i-- );
  }

  return -1;
}

template<typename CharT>
csBasicString<CharT> csBasicString<CharT>::mid(const int pos, const int n) const
{
  if( empty()  ||  pos < 0  ||  (size_t)pos >= size() ) {
    csBasicString<CharT>();
  }

  const size_t len = n < 1
      ? size() - (size_t)pos
      : csMin((size_t)n, size() - (size_t)pos);

  csBasicString<CharT> res(len+1);
  for(size_t i = 0; i < len; i++) {
    res[i] = operator[]((size_t)pos+i);
  }
  return res;
}

template<typename CharT>
csBasicString<CharT> csBasicString<CharT>::number(const uint32_t num,
                                                  const int base)
{
  const size_t maxsize = 33;
  CharT s[maxsize];
  CharT *p = csToStr(s, maxsize, num, base);
  if( p == 0 ) {
    return csBasicString<CharT>();
  }
  csBasicString<CharT> res(s);
  return res;
}

template<typename CharT>
csBasicString<CharT>& csBasicString<CharT>::replace(const CharT before,
                                                    const CharT after,
                                                    const bool ignoreCase)
{
  if( ignoreCase ) {
    const CharT needle = csToLower(before);
    for(size_t i = 0; i < size(); i++) {
      if( csToLower(operator[](i)) == needle ) {
        operator[](i) = after;
      }
    }
  } else {
    for(size_t i = 0; i < size(); i++) {
      if( operator[](i) == before ) {
        operator[](i) = after;
      }
    }
  }
  return *this;
}

template<typename CharT>
bool csBasicString<CharT>::startsWith(const CharT ch, const bool ignoreCase) const
{
  if( empty() ) {
    return false;
  }
  if( ignoreCase ) {
    return csToLower(operator[](0)) == csToLower(ch);
  }
  return operator[](0) == ch;
}

template<typename CharT>
bool csBasicString<CharT>::startsWith(const CharT *s, const bool ignoreCase) const
{
  if( empty()  ||  priv_string::stringLen(s) < 1  ||
      priv_string::stringLen(s) > size() ) {
    return false;
  }
  return priv_string::startsWith(*this, s, priv_string::stringLen(s), ignoreCase);
}

template<typename CharT>
bool csBasicString<CharT>::startsWith(const csBasicString<CharT>& other,
                                      const bool ignoreCase) const
{
  if( empty()  ||  other.empty()  ||  other.size() > size() ) {
    return false;
  }
  return priv_string::startsWith(*this, other.c_str(), other.size(), ignoreCase);
}

template<typename CharT>
uint32_t csBasicString<CharT>::toUInt(bool *ok, const int base) const
{
  if( ok != 0 ) {
    *ok = false;
  }
  if( empty() ) {
    return 0;
  }
  return csToUInt<CharT>(c_str(), ok, base);
}

////// Instantiation /////////////////////////////////////////////////////////

#ifdef HAVE_CHAR
template class CS_CORE2_EXPORT csBasicString<char>;
#endif

#ifdef HAVE_WCHAR_T
template class CS_CORE2_EXPORT csBasicString<wchar_t>;
#endif
