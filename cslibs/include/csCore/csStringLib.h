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

#include <csCore/csChar.h>

CS_CORE_EXPORT wchar_t *csStringCat(wchar_t *dest, const wchar_t *src);

CS_CORE_EXPORT int csStringCmp(const wchar_t *s1, const wchar_t *s2);

CS_CORE_EXPORT int csStringCmpI(const wchar_t *s1, const wchar_t *s2);

CS_CORE_EXPORT wchar_t *csStringCpy(wchar_t *dest, const wchar_t *src);

CS_CORE_EXPORT size_t csStringLen(const wchar_t *s);

CS_CORE_EXPORT wchar_t *csStringNCat(wchar_t *dest, const wchar_t *src,
                                     const size_t maxsize);

CS_CORE_EXPORT int csStringNCmp(const wchar_t *s1, const wchar_t *s2,
                                const size_t maxsize);

CS_CORE_EXPORT int csStringNCmpI(const wchar_t *s1, const wchar_t *s2,
                                 const size_t maxsize);

CS_CORE_EXPORT wchar_t *csStringNCpy(wchar_t *dest, const wchar_t *src,
                                     const size_t maxsize);

CS_CORE_EXPORT size_t csStringNLen(const wchar_t *s,
                                   const size_t maxsize);

#endif // __CSSTRINGLIB_H__
