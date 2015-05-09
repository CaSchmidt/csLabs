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

#include <QtCore/QString>

#include <csQt/csFlatTrie.h>

#include "private/csFlatTriePrivate.h"

////// public ////////////////////////////////////////////////////////////////

csFlatTrie::csFlatTrie()
  : _link()
  , _data()
{
}

csFlatTrie::~csFlatTrie()
{
}

void csFlatTrie::clear()
{
  _link.clear();
  _data.clear();
}

QStringList csFlatTrie::complete(const QString& baseStr) const
{
  if( isEmpty()  ||  baseStr.isEmpty() ) {
    return QStringList();
  }

  QStringList words;

  const int baseIndex = base(0, baseStr, 0);
  if( baseIndex < 0 ) {
    return QStringList();
  }

  if( isEndOfWord(_link[baseIndex]) ) {
    words.push_back(baseStr);
  }

  const int destLink = linkIndex(_link[baseIndex]);
  if( destLink > 0 ) {
    QString str;
    str.reserve(1024);
    str = baseStr;
    list(words, str, destLink);
  }

  return words;
}

cs::TrieMatch csFlatTrie::find(const QString& str) const
{
  if( isEmpty()  ||  str.isEmpty() ) {
    return cs::NoMatch;
  }
  return find(0, str, 0);
}

QStringList csFlatTrie::list() const
{
  if( isEmpty() ) {
    return QStringList();
  }

  QStringList words;

  QString str;
  str.reserve(1024);
  list(words, str, 0);

  return words;
}

bool csFlatTrie::isEmpty() const
{
  return _link.size() != _data.size()  ||  _link.isEmpty()  ||  _data.isEmpty();
}

int csFlatTrie::nodeCount() const
{
  if( isEmpty() ) {
    return 0;
  }
  return _link.size();
}

int csFlatTrie::size() const
{
  if( isEmpty() ) {
    return 0;
  }
  return sizeof(quint32)*_link.size() + sizeof(ushort)*_data.size();
}

////// private ///////////////////////////////////////////////////////////////

csFlatTrie::csFlatTrie(const QVector<quint32>& link, const QVector<ushort>& data)
  : _link(link)
  , _data(data)
{
}

int csFlatTrie::base(const int idxStr, const QString& str, const int baseIndex) const
{
  int i = baseIndex;
  do {
    if( _data[i] != str[idxStr].unicode() ) {
      continue;
    }

    if( idxStr+1 == str.size() ) { // Last character
      return i;
    }

    const int destIndex = linkIndex(_link[i]);
    if( destIndex != 0 ) {
      return base(idxStr+1, str, destIndex);
    }
  } while( !isEndOfList(_link[i++]) );

  return -1;
}

cs::TrieMatch csFlatTrie::find(const int idxStr, const QString& str,
                               const int baseIndex) const
{
  int i = baseIndex;
  do {
    if( _data[i] != str[idxStr].unicode() ) {
      continue;
    }

    if( idxStr+1 == str.size() ) { // Last character
      if( isEndOfWord(_link[i]) ) {
        return cs::ExactMatch;
      } else {
        return cs::PartialMatch;
      }
    }

    const int destIndex = linkIndex(_link[i]);
    if( destIndex != 0 ) {
      return find(idxStr+1, str, destIndex);
    }
  } while( !isEndOfList(_link[i++]) );

  return cs::NoMatch;
}

void csFlatTrie::list(QStringList& words, QString& str,
                      const int baseIndex) const
{
  int i = baseIndex;
  do {
    str.append(_data[i]);

    if( isEndOfWord(_link[i]) ) {
      words.push_back(str);
    }

    const int destIndex = linkIndex(_link[i]);
    if( destIndex != 0 ) {
      list(words, str, destIndex);
    }

    str.remove(str.size()-1, 1);
  } while( !isEndOfList(_link[i++]) );
}
