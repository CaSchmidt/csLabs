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

#ifndef __CSWINSTRING_H__
#define __CSWINSTRING_H__

#include <csCore/cscore_config.h>

#ifdef CS_OS_WINDOWS
# include <Windows.h>
#endif

class CS_CORE_EXPORT csString {
public:
  csString();
  csString(const csString& other);
  csString(const wchar_t ch);
  csString(const wchar_t *utf16, const bool copy = true);
#ifdef CS_OS_WINDOWS
  csString(const BSTR bstr);
#endif

  ~csString();

  // Assignment

  void clear();

  csString& operator=(const csString& other);
  csString& operator=(const wchar_t ch);
  csString& operator=(const wchar_t *utf16);

  // Concatenation

  csString& operator+=(const csString& other);
  csString& operator+=(const wchar_t ch);
  csString& operator+=(const wchar_t *utf16);

  // Conversion

  inline operator const unsigned short *() const
  {
    return (const unsigned short*)_data;
  }

  inline operator const wchar_t *() const
  {
    return _data;
  }

  // Explicit conversion

  inline const wchar_t *c_str() const
  {
    return _data;
  }

#ifdef CS_OS_WINDOWS
  BSTR toBSTR() const;
#endif

  inline const unsigned short *utf16() const
  {
    return (const unsigned short*)_data;
  }

  // Information

  bool isEmpty() const;

  int length() const;

  // Search

  bool contains(const wchar_t ch, const bool ignoreCase = false) const;

  bool endsWith(const csString& s, const bool ignoreCase = false) const;
  bool endsWith(const wchar_t ch, const bool ignoreCase = false) const;
  bool endsWith(const wchar_t *s, const bool ignoreCase = false) const;

  int indexOf(const wchar_t ch, const int from = 0,
              const bool ignoreCase = false) const;

  int lastIndexOf(const wchar_t ch, const int from = -1,
                  const bool ignoreCase = false) const;

  bool startsWith(const csString& s, const bool ignoreCase = false) const;
  bool startsWith(const wchar_t ch, const bool ignoreCase = false) const;
  bool startsWith(const wchar_t *s, const bool ignoreCase = false) const;

  // Modification

  csString& replace(const wchar_t ch, const wchar_t after,
                    const bool ignoreCase = false);

  // Access

  inline const wchar_t& operator[](const int index) const
  {
    return _data[index];
  }

  inline wchar_t& operator[](const int index)
  {
    return _data[index];
  }

  csString mid(const int pos, const int n = -1) const;

private:
  wchar_t *_data;
};

CS_CORE_EXPORT bool operator==(const csString& str1, const csString& str2);
CS_CORE_EXPORT bool operator==(const wchar_t *str1, const csString& str2);
CS_CORE_EXPORT bool operator==(const csString& str1, const wchar_t *str2);

CS_CORE_EXPORT bool operator!=(const csString& str1, const csString& str2);
CS_CORE_EXPORT bool operator!=(const wchar_t *str1, const csString& str2);
CS_CORE_EXPORT bool operator!=(const csString& str1, const wchar_t *str2);

CS_CORE_EXPORT csString operator+(const csString& str1, const csString& str2);
CS_CORE_EXPORT csString operator+(const wchar_t *str1, const csString& str2);
CS_CORE_EXPORT csString operator+(const csString& str1, const wchar_t *str2);

#endif // __CSWINSTRING_H__
