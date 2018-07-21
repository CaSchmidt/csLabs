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

#ifndef CSTEXTCONVERTER_H
#define CSTEXTCONVERTER_H

#include <list>
#include <memory>
#include <string>

#include <csUtil/csutil_config.h>

class csTextConverterData;

class CS_UTIL_EXPORT csTextConverter {
public:
  csTextConverter(csTextConverterData *ptr = nullptr);
  ~csTextConverter();

  csTextConverter(csTextConverter&&) noexcept;
  csTextConverter& operator=(csTextConverter&&) noexcept;

  bool isNull() const;
  void clear();

  const char *name() const;

  std::string fromUnicode(const char16_t *s, const std::string::size_type len = std::string::npos) const;

  inline std::string fromUnicode(const std::u16string& s) const
  {
    return fromUnicode(s.data(), s.size());
  }

  std::u16string toUnicode(const char *s, const std::u16string::size_type len = std::u16string::npos) const;

  inline std::u16string toUnicode(const std::string& s) const
  {
    return toUnicode(s.data(), s.size());
  }

  static csTextConverter create(const char *name);

  static csTextConverter createAscii();
  static csTextConverter createLatin1(); // ISO-8859-1
  static csTextConverter createLatin9(); // ISO-8859-15
  static csTextConverter createUtf8();
  static csTextConverter createWindows1252();

  static std::list<std::string> listAvailable();

private:
  csTextConverter(const csTextConverter&) = delete;
  csTextConverter& operator=(const csTextConverter&) = delete;

  std::unique_ptr<csTextConverterData> d;
};

// 8bit -> UTF-16 ////////////////////////////////////////////////////////////

CS_UTIL_EXPORT std::u16string csAsciiToUnicode(const char *s, const std::u16string::size_type len = std::u16string::npos);

inline std::u16string csAsciiToUnicode(const std::string& s)
{
  return csAsciiToUnicode(s.data(), s.size());
}

CS_UTIL_EXPORT std::u16string csUtf8ToUnicode(const char *s, const std::u16string::size_type len = std::u16string::npos);

inline std::u16string csUtf8ToUnicode(const std::string& s)
{
  return csUtf8ToUnicode(s.data(), s.size());
}

// UTF-16 -> 8bit ////////////////////////////////////////////////////////////

CS_UTIL_EXPORT std::string csUnicodeToAscii(const char16_t *s, const std::string::size_type len = std::string::npos);

inline std::string csUnicodeToAscii(const std::u16string& s)
{
  return csUnicodeToAscii(s.data(), s.size());
}

CS_UTIL_EXPORT std::string csUnicodeToUtf8(const char16_t *s, const std::string::size_type len = std::string::npos);

inline std::string csUnicodeToUtf8(const std::u16string& s)
{
  return csUnicodeToUtf8(s.data(), s.size());
}

#endif // CSTEXTCONVERTER_H
