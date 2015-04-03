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

#ifndef __UTIL_STRING_H__
#define __UTIL_STRING_H__

#include <csCore/csAlloc.h>
#include <csCore/csStringLib.h>

namespace priv_s {

  // Memory allocation ///////////////////////////////////////////////////////

  inline wchar_t *alloc(const size_t count)
  {
    if( count < 1 ) {
      return 0;
    }
    return (wchar_t*)csAlloc(count*sizeof(wchar_t));
  }

  inline void free(wchar_t* &ptr)
  {
    csFree(ptr);
    ptr = 0;
  }

  // Copy ////////////////////////////////////////////////////////////////////

  inline wchar_t *copy(const wchar_t *other)
  {
    const size_t len = csStringLen(other);
    if( len < 1 ) {
      return 0;
    }

    wchar_t *data = alloc(len+1);
    if( data == 0 ) {
      return 0;
    }

    csStringNCpy(data, other, len);
    data[len] = L'\0';

    return data;
  }

  // Concatenation ///////////////////////////////////////////////////////////

  inline wchar_t *cat(const wchar_t *s1, const wchar_t *s2)
  {
    const size_t len1 = csStringLen(s1);
    const size_t len2 = csStringLen(s2);
    if( len1 < 1  &&  len2 < 1 ) {
      return 0;
    }

    wchar_t *data = alloc(len1+len2+1);
    if( data == 0 ) {
      return 0;
    }

    if( len1 > 0 ) {
      csStringNCpy(&data[0], s1, len1);
    }
    if( len2 > 0 ) {
      csStringNCpy(&data[len1], s2, len2);
    }
    data[len1+len2] = L'\0';

    return data;
  }

}; // namespace priv_s

#endif // __UTIL_STRING_H__
