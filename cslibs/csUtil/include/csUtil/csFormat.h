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

#ifndef CSFORMAT_H
#define CSFORMAT_H

#include <cstdint>

#include <memory>
#include <string>

#include <csUtil/csutil_config.h>

namespace cs {
  namespace format {
    template<typename CharT>
    class Formatter;
  }
}

struct csFormatChar {
  inline explicit csFormatChar(const char& c)
    : value(c)
  {
  }

  inline explicit csFormatChar(const signed char& c)
    : value(static_cast<char>(c))
  {
  }

  inline explicit csFormatChar(const unsigned char& c)
    : value(static_cast<char>(c))
  {
  }

  const char value;
};

class CS_UTIL_EXPORT csFormat {
public:
  csFormat(const std::string& s);
  csFormat(const char *s);
  ~csFormat();

  operator std::string() const;

  csFormat& arg(const csFormatChar& c, const int width = 0, const char fill = ' ');

  csFormat& arg(const std::string& s, const int width = 0, const char fill = ' ');
  csFormat& arg(const char *s, const int width = 0, const char fill = ' ');

  csFormat& arg(const int8_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const uint8_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const int16_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const uint16_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const int32_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const uint32_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const int64_t value, const int width = 0, const int base = 10, const char fill = ' ');
  csFormat& arg(const uint64_t value, const int width = 0, const int base = 10, const char fill = ' ');

  csFormat& arg(const float value, const int width = 0, const char format = 'g', const int precision = 6, const char fill = ' ');
  csFormat& arg(const double value, const int width = 0, const char format = 'g', const int precision = 6, const char fill = ' ');

  csFormat& operator%(const csFormatChar& c);

  csFormat& operator%(const std::string& s);
  csFormat& operator%(const char *s);

  csFormat& operator%(const int8_t value);
  csFormat& operator%(const uint8_t value);
  csFormat& operator%(const int16_t value);
  csFormat& operator%(const uint16_t value);
  csFormat& operator%(const int32_t value);
  csFormat& operator%(const uint32_t value);
  csFormat& operator%(const int64_t value);
  csFormat& operator%(const uint64_t value);

  csFormat& operator%(const float value);
  csFormat& operator%(const double value);

private:
  using Formatter = cs::format::Formatter<char>;

  csFormat() = delete;
  csFormat(const csFormat&) = delete;
  csFormat& operator=(const csFormat&) = delete;
  csFormat(csFormat&&) = delete;
  csFormat& operator=(csFormat&&) = delete;

  std::unique_ptr<Formatter> impl;
};

#endif // CSFORMAT_H
