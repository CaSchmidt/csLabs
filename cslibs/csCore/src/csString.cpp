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

#include <csCore/csString.h>

#include "internal/util_string.h"
#include <csCore/csUtil.h>

////// public ////////////////////////////////////////////////////////////////

csString::csString()
  : _data(0)
{
}

csString::csString(const csString& other)
  : _data(0)
{
  _data = priv_s::copy(other);
}

csString::csString(const wchar_t ch)
  : _data(0)
{
  _data = priv_s::alloc(2);
  if( _data != 0 ) {
    _data[0] = ch;
    _data[1] = L'\0';
  }
}

csString::csString(const wchar_t *utf16, const bool copy)
  : _data(0)
{
  if( copy ) {
    _data = priv_s::copy(utf16);
  } else {
    _data = (wchar_t*)utf16;
  }
}

#ifdef CS_OS_WINDOWS
csString::csString(const BSTR bstr)
  : _data(0)
{
  _data = priv_s::copy(bstr);
}
#endif

csString::~csString()
{
  priv_s::free(_data);
}

////// public - Assignment ///////////////////////////////////////////////////

void csString::clear()
{
  priv_s::free(_data);
}

csString& csString::operator=(const csString& other)
{
  if( this != &other ) {
    priv_s::free(_data);
    _data = priv_s::copy(other);
  }
  return *this;
}

csString& csString::operator=(const wchar_t ch)
{
  priv_s::free(_data);
  _data = priv_s::alloc(2);
  if( _data != 0 ) {
    _data[0] = ch;
    _data[1] = L'\0';
  }
  return *this;
}

csString& csString::operator=(const wchar_t *utf16)
{
  priv_s::free(_data);
  _data = priv_s::copy(utf16);
  return *this;
}

////// public - Concatenation ////////////////////////////////////////////////

csString& csString::operator+=(const csString& other)
{
  wchar_t *old = _data;
  _data = priv_s::cat(old, other);
  priv_s::free(old);
  return *this;
}

csString& csString::operator+=(const wchar_t ch)
{
  wchar_t s[2] = { ch, L'\0' };
  wchar_t *old = _data;
  _data = priv_s::cat(old, s);
  priv_s::free(old);
  return *this;
}

csString& csString::operator+=(const wchar_t *utf16)
{
  wchar_t *old = _data;
  _data = priv_s::cat(old, utf16);
  priv_s::free(old);
  return *this;
}

////// public - Explicit conversion //////////////////////////////////////////

#ifdef CS_OS_WINDOWS
BSTR csString::toBSTR() const
{
  return SysAllocString(_data);
}
#endif

////// public - Information //////////////////////////////////////////////////

bool csString::isEmpty() const
{
  return csStringLen(_data) < 1;
}

int csString::length() const
{
  return (int)csStringLen(_data);
}

////// public - search ///////////////////////////////////////////////////////

bool csString::contains(const wchar_t ch, const bool ignoreCase) const
{
  const size_t len = csStringLen(_data);
  if( len < 1 ) {
    return false;
  }
  if( ignoreCase ) {
    const wchar_t needle = csToLower(ch);
    for(size_t i = 0; i < len; i++) {
      if( csToLower(_data[i]) == needle ) {
        return true;
      }
    }
  } else {
    for(size_t i = 0; i < len; i++) {
      if( _data[i] == ch ) {
        return true;
      }
    }
  }
  return false;
}

bool csString::endsWith(const csString& s, const bool ignoreCase) const
{
  return endsWith(s._data, ignoreCase);
}

bool csString::endsWith(const wchar_t ch, const bool ignoreCase) const
{
  const size_t len = csStringLen(_data);
  if( len < 1 ) {
    return false;
  }
  if( ignoreCase ) {
    return csToLower(_data[len-1]) == csToLower(ch);
  }
  return _data[len-1] == ch;
}

bool csString::endsWith(const wchar_t *s, const bool ignoreCase) const
{
  const size_t len  = csStringLen(_data);
  const size_t len2 = csStringLen(s);
  if( len < 1  ||  len2 < 1  ||  len2 > len ) {
    return false;
  }
  if( ignoreCase ) {
    return csStringNCmpI(&_data[len-len2], s, len2) == 0;
  }
  return csStringNCmp(&_data[len-len2], s, len2) == 0;
}

int csString::indexOf(const wchar_t ch, const int from,
                      const bool ignoreCase) const
{
  const size_t len = csStringLen(_data);
  if( len < 1 ) {
    return -1;
  }
  const int start = from < 0  ?  int(len)+from : from;
  if( start < 0  ||  start >= len ) {
    return -1;
  }
  if( ignoreCase ) {
    const wchar_t needle = csToLower(ch);
    for(size_t i = start; i < len; i++) {
      if( csToLower(_data[i]) == needle ) {
        return (int)i;
      }
    }
  } else {
    for(size_t i = start; i < len; i++) {
      if( _data[i] == ch ) {
        return (int)i;
      }
    }
  }
  return -1;
}

int csString::lastIndexOf(const wchar_t ch, const int from,
                          const bool ignoreCase) const
{
  const size_t len = csStringLen(_data);
  if( len < 1 ) {
    return -1;
  }
  const int start = from < 0  ?  int(len)+from : from;
  if( start < 0  ||  start >= len ) {
    return -1;
  }
  if( ignoreCase ) {
#if 1
    const wchar_t needle = csToLower(ch);
    size_t i = start;
    wchar_t *ptr = &_data[start];
    do {
      if( csToLower(_data[i]) == needle ) {
        return (int)i;
      }
      i--;
    } while( ptr-- != _data );
#else
    const wchar_t needle = csToLower(ch);
    for(size_t i = start; i != SIZE_MAX; i--) {
      if( csToLower(_data[i]) == needle ) {
        return (int)i;
      }
    }
#endif
  } else {
#if 1
    size_t i = start;
    wchar_t *ptr = &_data[start];
    do {
      if( _data[i] == ch ) {
        return (int)i;
      }
      i--;
    } while( ptr-- != _data );
#else
    for(size_t i = start; i != SIZE_MAX; i--) {
      if( _data[i] == ch ) {
        return (int)i;
      }
    }
#endif
  }
  return -1;
}

bool csString::startsWith(const csString& s, const bool ignoreCase) const
{
  return startsWith(s._data, ignoreCase);
}

bool csString::startsWith(const wchar_t ch, const bool ignoreCase) const
{
  const size_t len = csStringLen(_data);
  if( len < 1 ) {
    return false;
  }
  if( ignoreCase ) {
    return csToLower(_data[0]) == csToLower(ch);
  }
  return _data[0] == ch;
}

bool csString::startsWith(const wchar_t *s, const bool ignoreCase) const
{
  const size_t len  = csStringLen(_data);
  const size_t len2 = csStringLen(s);
  if( len < 1  ||  len2 < 1  ||  len2 > len ) {
    return false;
  }
  if( ignoreCase ) {
    return csStringNCmpI(&_data[0], s, len2) == 0;
  }
  return csStringNCmp(&_data[0], s, len2) == 0;
}

////// public - Modification /////////////////////////////////////////////////

csString& csString::replace(const wchar_t ch, const wchar_t after,
                            const bool ignoreCase)
{
  const size_t len = csStringLen(_data);
  if( ignoreCase ) {
    const wchar_t needle = csToLower(ch);
    for(size_t i = 0; i < len; i++) {
      if( csToLower(_data[i]) == needle ) {
        _data[i] = after;
      }
    }
  } else {
    for(size_t i = 0; i < len; i++) {
      if( _data[i] == ch ) {
        _data[i] = after;
      }
    }
  }
  return *this;
}

////// public - Access ///////////////////////////////////////////////////////

csString csString::mid(const int pos, const int n) const
{
  const size_t len = csStringLen(_data);
  if( len < 1  ||  pos < 0  ||  pos >= len ) {
    return csString();
  }
  const size_t maxcount = len-pos;
  const size_t count = n < 1  ?  maxcount : csMin(maxcount, (size_t)n);
  wchar_t *data = priv_s::alloc(count+1);
  if( data == 0 ) {
    return csString();
  }
  csStringNCpy(data, &_data[pos], count);
  data[count] = L'\0';
  return csString(data, false);
}

////// Public - Compare equal ////////////////////////////////////////////////

CS_CORE_EXPORT bool operator==(const csString& str1, const csString& str2)
{
  return csStringCmp(str1, str2) == 0;
}

CS_CORE_EXPORT bool operator==(const wchar_t *str1, const csString& str2)
{
  return csStringCmp(str1, str2) == 0;
}

CS_CORE_EXPORT bool operator==(const csString& str1, const wchar_t *str2)
{
  return csStringCmp(str1, str2) == 0;
}

////// Public - Compare not equal ////////////////////////////////////////////

CS_CORE_EXPORT bool operator!=(const csString& str1, const csString& str2)
{
  return csStringCmp(str1, str2) != 0;
}

CS_CORE_EXPORT bool operator!=(const wchar_t *str1, const csString& str2)
{
  return csStringCmp(str1, str2) != 0;
}

CS_CORE_EXPORT bool operator!=(const csString& str1, const wchar_t *str2)
{
  return csStringCmp(str1, str2) != 0;
}

////// Public - Concatenation ////////////////////////////////////////////////

CS_CORE_EXPORT csString operator+(const csString& str1, const csString& str2)
{
  return csString(priv_s::cat(str1, str2), false);
}

CS_CORE_EXPORT csString operator+(const wchar_t *str1, const csString& str2)
{
  return csString(priv_s::cat(str1, str2), false);
}

CS_CORE_EXPORT csString operator+(const csString& str1, const wchar_t *str2)
{
  return csString(priv_s::cat(str1, str2), false);
}
