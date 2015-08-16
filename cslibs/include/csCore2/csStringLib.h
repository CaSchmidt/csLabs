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

#ifndef __CSSTRINGLIB_H__
#define __CSSTRINGLIB_H__

#include <csCore2/cscore2_config.h>

template<typename CharT>
CharT *csStringCat(CharT *dest, const CharT *src);

template<typename CharT>
int csStringCmp(const CharT *s1, const CharT *s2);

template<typename CharT>
int csStringCmpI(const CharT *s1, const CharT *s2);

template<typename CharT>
CharT *csStringCpy(CharT *dest, const CharT *src);

template<typename CharT>
size_t csStringLen(const CharT *s);

template<typename CharT>
CharT *csStringNCat(CharT *dest, const CharT *src, const size_t maxsize);

template<typename CharT>
int csStringNCmp(const CharT *s1, const CharT *s2, const size_t maxsize);

template<typename CharT>
int csStringNCmpI(const CharT *s1, const CharT *s2, const size_t maxsize);

template<typename CharT>
CharT *csStringNCpy(CharT *dest, const CharT *src, const size_t maxsize);

template<typename CharT>
size_t csStringNLen(const CharT *s, const size_t maxsize);

#ifdef HAVE_CHAR
extern template CS_CORE2_EXPORT char *csStringCat<char>(char *dest, const char *src);
extern template CS_CORE2_EXPORT int csStringCmp<char>(const char *s1, const char *s2);
extern template CS_CORE2_EXPORT int csStringCmpI<char>(const char *s1, const char *s2);
extern template CS_CORE2_EXPORT char *csStringCpy<char>(char *dest, const char *src);
extern template CS_CORE2_EXPORT size_t csStringLen<char>(const char *s);
extern template CS_CORE2_EXPORT char *csStringNCat<char>(char *dest, const char *src, const size_t maxsize);
extern template CS_CORE2_EXPORT int csStringNCmp<char>(const char *s1, const char *s2, const size_t maxsize);
extern template CS_CORE2_EXPORT int csStringNCmpI<char>(const char *s1, const char *s2, const size_t maxsize);
extern template CS_CORE2_EXPORT char *csStringNCpy<char>(char *dest, const char *src, const size_t maxsize);
extern template CS_CORE2_EXPORT size_t csStringNLen<char>(const char *s, const size_t maxsize);
#endif

#ifdef HAVE_WCHAR_T
extern template CS_CORE2_EXPORT wchar_t *csStringCat<wchar_t>(wchar_t *dest, const wchar_t *src);
extern template CS_CORE2_EXPORT  int csStringCmp<wchar_t>(const wchar_t *s1, const wchar_t *s2);
extern template CS_CORE2_EXPORT int csStringCmpI<wchar_t>(const wchar_t *s1, const wchar_t *s2);
extern template CS_CORE2_EXPORT wchar_t *csStringCpy<wchar_t>(wchar_t *dest, const wchar_t *src);
extern template CS_CORE2_EXPORT size_t csStringLen<wchar_t>(const wchar_t *s);
extern template CS_CORE2_EXPORT wchar_t *csStringNCat<wchar_t>(wchar_t *dest, const wchar_t *src, const size_t maxsize);
extern template CS_CORE2_EXPORT int csStringNCmp<wchar_t>(const wchar_t *s1, const wchar_t *s2, const size_t maxsize);
extern template CS_CORE2_EXPORT int csStringNCmpI<wchar_t>(const wchar_t *s1, const wchar_t *s2, const size_t maxsize);
extern template CS_CORE2_EXPORT wchar_t *csStringNCpy<wchar_t>(wchar_t *dest, const wchar_t *src, const size_t maxsize);
extern template CS_CORE2_EXPORT size_t csStringNLen<wchar_t>(const wchar_t *s, const size_t maxsize);
#endif

#endif // __CSSTRINGLIB_H__
