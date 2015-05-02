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

#ifndef __CSFLATTRIE_H__
#define __CSFLATTRIE_H__

#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <csQt/csqt_config.h>
#include <csQt/csNamespace.h>

class CS_QT_EXPORT csFlatTrie {
public:
  csFlatTrie();
  ~csFlatTrie();

  void clear();

  QStringList complete(const QString& str) const;

  cs::TrieMatch find(const QString& str) const;

  bool isEmpty() const;

  QStringList list() const;

  int nodeCount() const;

  int size() const;

private:
  csFlatTrie(const QVector<quint32>& link, const QVector<ushort>& data);

  int base(const int idxStr, const QString& str, const int baseIndex) const;

  cs::TrieMatch find(const int idxStr, const QString& str, const int baseIndex) const;

  void list(QStringList& words, QString& str, const int baseIndex) const;

  friend class csTrie;

  QVector<quint32> _link;
  QVector<ushort>  _data;
};

#endif // __CSFLATTRIE_H__
