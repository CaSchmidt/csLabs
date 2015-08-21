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

#ifndef __CSSTRINGLIST_H__
#define __CSSTRINGLIST_H__

#include <list>

#include <csCore2/cscore2_config.h>

#include <csCore2/csString.h>

template<typename CharT>
class csBasicStringList : public std::list<csBasicString<CharT> > {
public:
  csBasicStringList();
  csBasicStringList(const csBasicStringList<CharT>& other);

  csBasicStringList(const std::list<csBasicString<CharT> >& other);

  ~csBasicStringList();

  csBasicStringList<CharT>& operator=(const csBasicStringList<CharT>& other);

  csBasicStringList<CharT>& operator=(const std::list<csBasicString<CharT> >& other);

  void push_back(const CharT *data);

  void push_front(const CharT *data);
};

#ifdef HAVE_CHAR
extern template class CS_CORE2_EXPORT csBasicStringList<char>;

typedef csBasicStringList<char> csStringList;
#endif

#ifdef HAVE_WCHAR_T
extern template class CS_CORE2_EXPORT csBasicStringList<wchar_t>;

typedef csBasicStringList<wchar_t> csWStringList;
#endif

#endif // __CSSTRINGLIST_H__
