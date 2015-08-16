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

#include "csCore2/csStringLib.h"

#include "csCore2/csChar.h"
#include "csCore2/csUtil.h"

////// Implementation ////////////////////////////////////////////////////////

template<typename CharT>
CharT *csStringCat(CharT *dest, const CharT *src)
{
  if( dest == 0 ) {
    return 0;
  }
  if( src == 0 ) {
    return dest;
  }
  const size_t N = csStringLen(src);
  CharT *to = &dest[csStringLen(dest)];
  for(size_t i = 0; i < N; i++) {
    *to++ = *src++;
  }
  *to = CharT(0);
  return dest;
}

template<typename CharT>
int csStringCmp(const CharT *s1, const CharT *s2)
{
  const size_t len1 = csStringLen(s1);
  const size_t len2 = csStringLen(s2);
  if( len1 < 1  ||  len2 < 1 ) {
    if(        len1 < 1  &&  len2 < 1 ) {
      return 0;
    } else if( len1 < 1 ) {
      return -1;
    } else if( len2 < 1 ) {
      return 1;
    }
  }
  const size_t len = csMin(len1, len2);
  for(size_t i = 0; i < len; i++) {
    const int diff = int(s1[i])-int(s2[i]);
    if( diff != 0 ) {
      return diff;
    }
  }
  if(        len1 < len2 ) {
    return -1;
  } else if( len1 > len2 ) {
    return 1;
  }
  return 0;
}

template<typename CharT>
int csStringCmpI(const CharT *s1, const CharT *s2)
{
  const size_t len1 = csStringLen(s1);
  const size_t len2 = csStringLen(s2);
  if( len1 < 1  ||  len2 < 1 ) {
    if(        len1 < 1  &&  len2 < 1 ) {
      return 0;
    } else if( len1 < 1 ) {
      return -1;
    } else if( len2 < 1 ) {
      return 1;
    }
  }
  const size_t len = csMin(len1, len2);
  for(size_t i = 0; i < len; i++) {
    const int diff = int(csToLower(s1[i]))-int(csToLower(s2[i]));
    if( diff != 0 ) {
      return diff;
    }
  }
  if(        len1 < len2 ) {
    return -1;
  } else if( len1 > len2 ) {
    return 1;
  }
  return 0;
}

template<typename CharT>
CharT *csStringCpy(CharT *dest, const CharT *src)
{
  if( dest == 0 ) {
    return 0;
  }
  if( src == 0 ) {
    return dest;
  }
  const size_t N = csStringLen(src);
  CharT *to = dest;
  for(size_t i = 0; i < N; i++) {
    *to++ = *src++;
  }
  *to = CharT(0);
  return dest;
}

template<typename CharT>
size_t csStringLen(const CharT *s)
{
  if( s == 0 ) {
    return 0;
  }
  size_t size = 0;
  while( *s++ != CharT(0) ) {
    size++;
  }
  return size;
}

template<typename CharT>
CharT *csStringNCat(CharT *dest, const CharT *src, const size_t maxsize)
{
  if( dest == 0 ) {
    return 0;
  }
  if( src == 0 ) {
    return dest;
  }
  CharT *to = &dest[csStringLen(dest)];
  for(size_t i = 0; i < maxsize  &&  *src != CharT(0); i++) {
    *to++ = *src++;
  }
  *to = CharT(0);
  return dest;
}

template<typename CharT>
int csStringNCmp(const CharT *s1, const CharT *s2, const size_t maxsize)
{
  if( s1 == 0  ||  s2 == 0 ) {
    if(        s1 == 0  &&  s2 == 0 ) {
      return 0;
    } else if( s1 == 0 ) {
      return -1;
    } else if( s2 == 0 ) {
      return 1;
    }
  }
  size_t i;
  for(i = 0; i < maxsize  &&  s1[i] != CharT(0)  &&  s2[i] != CharT(0); i++) {
    const int diff = int(s1[i])-int(s2[i]);
    if( diff != 0 ) {
      return diff;
    }
  }
  if( i < maxsize ) {
    if(        s1[i] == CharT(0)  &&  s2[i] != CharT(0) ) { // len1 < len2
      return -1;
    } else if( s1[i] != CharT(0)  &&  s2[i] == CharT(0) ) { // len1 > len2
      return 1;
    }
  }
  return 0;
}

template<typename CharT>
int csStringNCmpI(const CharT *s1, const CharT *s2, const size_t maxsize)
{
  if( s1 == 0  ||  s2 == 0 ) {
    if(        s1 == 0  &&  s2 == 0 ) {
      return 0;
    } else if( s1 == 0 ) {
      return -1;
    } else if( s2 == 0 ) {
      return 1;
    }
  }
  size_t i;
  for(i = 0; i < maxsize  &&  s1[i] != CharT(0)  &&  s2[i] != CharT(0); i++) {
    const int diff = int(csToLower(s1[i]))-int(csToLower(s2[i]));
    if( diff != 0 ) {
      return diff;
    }
  }
  if( i < maxsize ) {
    if(        s1[i] == CharT(0)  &&  s2[i] != CharT(0) ) { // len1 < len2
      return -1;
    } else if( s1[i] != CharT(0)  &&  s2[i] == CharT(0) ) { // len1 > len2
      return 1;
    }
  }
  return 0;
}

template<typename CharT>
CharT *csStringNCpy(CharT *dest, const CharT *src, const size_t maxsize)
{
  if( dest == 0 ) {
    return 0;
  }
  if( src == 0 ) {
    return dest;
  }
  CharT *to = dest;
  size_t i;
  for(i = 0; i < maxsize  &&  *src != CharT(0); i++) {
    *to++ = *src++;
  }
  for(size_t fill = i; fill < maxsize; fill++) {
    *to++ = CharT(0);
  }
  return dest;
}

template<typename CharT>
size_t csStringNLen(const CharT *s, const size_t maxsize)
{
  if( s == 0  ||  maxsize < 1 ) {
    return 0;
  }
  size_t size = 0;
  while( size < maxsize  &&  *s++ != CharT(0) ) {
    size++;
  }
  return size;
}

////// Explicit instantiation ////////////////////////////////////////////////

#ifdef HAVE_CHAR
template CS_CORE2_EXPORT char *csStringCat<char>(char *dest, const char *src);
template CS_CORE2_EXPORT int csStringCmp<char>(const char *s1, const char *s2);
template CS_CORE2_EXPORT int csStringCmpI<char>(const char *s1, const char *s2);
template CS_CORE2_EXPORT char *csStringCpy<char>(char *dest, const char *src);
template CS_CORE2_EXPORT size_t csStringLen<char>(const char *s);
template CS_CORE2_EXPORT char *csStringNCat<char>(char *dest, const char *src, const size_t maxsize);
template CS_CORE2_EXPORT int csStringNCmp<char>(const char *s1, const char *s2, const size_t maxsize);
template CS_CORE2_EXPORT int csStringNCmpI<char>(const char *s1, const char *s2, const size_t maxsize);
template CS_CORE2_EXPORT char *csStringNCpy<char>(char *dest, const char *src, const size_t maxsize);
template CS_CORE2_EXPORT size_t csStringNLen<char>(const char *s, const size_t maxsize);
#endif

#ifdef HAVE_WCHAR_T
template CS_CORE2_EXPORT wchar_t *csStringCat<wchar_t>(wchar_t *dest, const wchar_t *src);
template CS_CORE2_EXPORT  int csStringCmp<wchar_t>(const wchar_t *s1, const wchar_t *s2);
template CS_CORE2_EXPORT int csStringCmpI<wchar_t>(const wchar_t *s1, const wchar_t *s2);
template CS_CORE2_EXPORT wchar_t *csStringCpy<wchar_t>(wchar_t *dest, const wchar_t *src);
template CS_CORE2_EXPORT size_t csStringLen<wchar_t>(const wchar_t *s);
template CS_CORE2_EXPORT wchar_t *csStringNCat<wchar_t>(wchar_t *dest, const wchar_t *src, const size_t maxsize);
template CS_CORE2_EXPORT int csStringNCmp<wchar_t>(const wchar_t *s1, const wchar_t *s2, const size_t maxsize);
template CS_CORE2_EXPORT int csStringNCmpI<wchar_t>(const wchar_t *s1, const wchar_t *s2, const size_t maxsize);
template CS_CORE2_EXPORT wchar_t *csStringNCpy<wchar_t>(wchar_t *dest, const wchar_t *src, const size_t maxsize);
template CS_CORE2_EXPORT size_t csStringNLen<wchar_t>(const wchar_t *s, const size_t maxsize);
#endif
