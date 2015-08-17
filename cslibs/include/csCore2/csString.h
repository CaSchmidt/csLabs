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

#ifndef __CSSTRING_H__
#define __CSSTRING_H__

#include <string>

#include <csCore2/cscore2_config.h>

template<typename CharT>
class csBasicString : public std::basic_string<CharT> {
public:
  csBasicString();
  csBasicString(const csBasicString<CharT>& other);

  csBasicString(const size_t count, const CharT ch = CharT(0));
  csBasicString(const CharT *data);
  csBasicString(const std::basic_string<CharT>& other);

  ~csBasicString();

  csBasicString<CharT>& operator=(const csBasicString<CharT>& other);

  csBasicString<CharT>& operator=(const CharT *data);
  csBasicString<CharT>& operator=(const std::basic_string<CharT>& other);

  bool contains(const CharT ch, const bool ignoreCase = false) const;

  bool endsWith(const CharT ch, const bool ignoreCase = false) const;
  bool endsWith(const CharT *s, const bool ignoreCase = false) const;
  bool endsWith(const csBasicString<CharT>& other, const bool ignoreCase = false) const;

  int indexOf(const CharT ch, const int from = 0, const bool ignoreCase = false) const;

  int lastIndexOf(const CharT ch, const int from = -1, const bool ignoreCase = false) const;

  csBasicString<CharT> mid(const int pos, const int n = -1) const;

  csBasicString<CharT>& replace(const wchar_t before, const wchar_t after,
                                const bool ignoreCase = false);

  bool startsWith(const CharT ch, const bool ignoreCase = false) const;
  bool startsWith(const CharT *s, const bool ignoreCase = false) const;
  bool startsWith(const csBasicString<CharT>& other, const bool ignoreCase = false) const;

  uint32_t toUInt(bool *ok = 0, const int base = 10) const;
};

#ifdef HAVE_CHAR
extern template class CS_CORE2_EXPORT csBasicString<char>;

typedef csBasicString<char> csString;
#endif

#ifdef HAVE_WCHAR_T
extern template class CS_CORE2_EXPORT csBasicString<wchar_t>;

typedef csBasicString<wchar_t> csWString;
#endif

#endif // __CSSTRING_H__
