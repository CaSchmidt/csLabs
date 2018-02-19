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

#ifndef __CSCHARCONV_H__
#define __CSCHARCONV_H__

#include <cstdint>

#include <system_error>

#include <csUtil/csutil_config.h>

namespace cs {

  struct from_chars_result {
    const char *ptr;
    std::errc ec;
  };

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int8_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int16_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int32_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              int64_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint8_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint16_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint32_t& value, int base = 10);

  CS_UTIL_EXPORT from_chars_result from_chars(const char *first, const char *last,
                                              uint64_t& value, int base = 10);

  struct to_chars_result {
    char *ptr;
    std::errc ec;
  };

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int8_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int16_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int32_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          int64_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint8_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint16_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint32_t value, int base = 10);

  CS_UTIL_EXPORT to_chars_result to_chars(char *first, char *last,
                                          uint64_t value, int base = 10);

} // namespace cs

#endif // __CSCHARCONV_H__
