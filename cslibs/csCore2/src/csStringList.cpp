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

#include "csCore2/csStringList.h"

////// Implementation ////////////////////////////////////////////////////////

template<typename CharT>
csBasicStringList<CharT>::csBasicStringList()
  : std::list<csBasicString<CharT> >()
{
}

template<typename CharT>
csBasicStringList<CharT>::csBasicStringList(const csBasicStringList<CharT>& other)
  : std::list<csBasicString<CharT> >(other)
{
}

template<typename CharT>
csBasicStringList<CharT>::csBasicStringList(const std::list<csBasicString<CharT> >& other)
  : std::list<csBasicString<CharT> >(other)
{
}

template<typename CharT>
csBasicStringList<CharT>::~csBasicStringList()
{
}

template<typename CharT>
csBasicStringList<CharT>& csBasicStringList<CharT>::operator=(const csBasicStringList<CharT>& other)
{
  if( this != &other ) {
    std::list<csBasicString<CharT> >::operator=(other);
  }
  return *this;
}

template<typename CharT>
csBasicStringList<CharT>& csBasicStringList<CharT>::operator=(const std::list<csBasicString<CharT> >& other)
{
  std::list<csBasicString<CharT> >::operator=(other);
  return *this;
}

template<typename CharT>
void csBasicStringList<CharT>::push_back(const CharT *data)
{
  std::list<csBasicString<CharT> >::push_back(csBasicString<CharT>(data));
}

////// Explicit instantiation ////////////////////////////////////////////////

#ifdef HAVE_CHAR
template class CS_CORE2_EXPORT csBasicStringList<char>;
#endif

#ifdef HAVE_WCHAR_T
template class CS_CORE2_EXPORT csBasicStringList<wchar_t>;
#endif
