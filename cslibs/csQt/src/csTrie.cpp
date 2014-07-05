/****************************************************************************
** Copyright (c) 2013-2014, Carsten Schmidt. All rights reserved.
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

#include "csQt/csTrie.h"

////// Macros ////////////////////////////////////////////////////////////////

#define STATUS_MASK_CHILDREN  ((quint32)0x00007FFF)
#define STATUS_MASK_LETTER    ((quint32)0xFFFF0000)
#define STATUS_MASK_MATCH     ((quint32)0x00008000)

#define STATUS_SHIFT_LETTER  16

////// Private ///////////////////////////////////////////////////////////////

typedef ushort csUtf16;

class csTrieNode {
public:
  csTrieNode(const csUtf16 _letter)
    : _status(0),
      _children(0)
  {
    _status   = _letter;
    _status <<= STATUS_SHIFT_LETTER;
  }

  ~csTrieNode()
  {
    for(int i = 0; i < childCount(); i++) {
      delete _children[i];
    }
    delete[] _children;
  }

  const csTrieNode *base(const int idxStr, const QString& str) const
  {
    if( idxStr == str.size() ) {
      return this;
    }

    csTrieNode *child = findLetter(str[idxStr].unicode());
    if( child == 0 ) {
      return 0;
    }

    return child->base(idxStr+1, str);
  }

  csTrie::MatchResult find(const int idxStr, const QString& str) const
  {
    if( idxStr == str.size() ) {
      if( isMatch() ) {
        return csTrie::ExactMatch;
      } else {
        return csTrie::PartialMatch;
      }
    }

    csTrieNode *idxMatch = findLetter(str[idxStr].unicode());

    if( idxMatch == 0 ) {
      return csTrie::NoMatch;
    }

    return idxMatch->find(idxStr+1, str);
  }

  void insert(const int idxStr, const QString& str)
  {
    if( idxStr == str.size() ) {
      _status |= STATUS_MASK_MATCH;
      return;
    }

    const csUtf16 curLetter = str[idxStr].unicode();
    csTrieNode *idxMatch = findLetter(curLetter);

    if( idxMatch == 0 ) {
      int idxInsert = 0;
      // Increment Child Count BEFORE Insertion
      _status++; // Increases childCount()!!!
      if( _children == 0 ) { // No Children Yet! -> Add First
        _children = new csTrieNode*[1];
      } else { // Grow Child Population to Insert New Child
        // Compute (Sorted) Insertion Index Into OLD Children
        for(idxInsert = 0; idxInsert < childCount()-1; idxInsert++) {
          if( _children[idxInsert]->letter() > curLetter ) {
            break;
          }
        }
        // Save Old Pointers
        csTrieNode **childrenOld = _children;
        // New Pointers
        _children = new csTrieNode*[childCount()];
        // Copy Old
        for(int i = 0; i < idxInsert; i++) {
          _children[i] = childrenOld[i];
        }
        for(int i = idxInsert; i < childCount()-1; i++) {
          _children[i+1] = childrenOld[i];
        }
        // Free Old Pointers
        delete[] childrenOld;
      }
      // Insert New
      _children[idxInsert] = new csTrieNode(curLetter);
      idxMatch = _children[idxInsert];
    }

    idxMatch->insert(idxStr+1, str);
  }

  void list(QStringList& words, QString& str) const
  {
    if( isMatch() ) {
      words.push_back(str);
    }

    for(int i = 0; i < childCount(); i++) {
      csTrieNode *child = _children[i];
      str.append(child->letter());
      child->list(words, str);
      str.remove(str.size()-1, 1);
    }
  }

  void statistics(int& numNodes, int& numLinks) const
  {
    numNodes++;
    numLinks += childCount();

    for(int i = 0; i < childCount(); i++) {
      _children[i]->statistics(numNodes, numLinks);
    }
  }

private:
  inline int childCount() const
  {
    return (int)(_status & STATUS_MASK_CHILDREN);
  }

  csTrieNode *findLetter(const csUtf16 l) const
  {
    for(int i = 0; i < childCount(); i++) {
      if( _children[i]->letter() == l ) {
        return _children[i];
      }
    }

    return 0;
  }

  inline csUtf16 letter() const
  {
    return (csUtf16)(_status >> STATUS_SHIFT_LETTER);
  }

  inline bool isMatch() const
  {
    return (_status & STATUS_MASK_MATCH) != 0;
  }

  quint32 _status;
  csTrieNode **_children;
};

////// public ////////////////////////////////////////////////////////////////

csTrie::csTrie()
  : _root(0)
{
  _root = new csTrieNode(0);
}

csTrie::~csTrie()
{
  clear();
}

void csTrie::clear()
{
  delete _root;
}

QStringList csTrie::complete(const QString& base) const
{
  if( base.isEmpty() ) {
    return QStringList();
  }

  const csTrieNode *root = _root->base(0, base);
  if( root == 0 ) {
    return QStringList();
  }

  QString str;
  str.reserve(1024);
  str = base;

  QStringList words;
  root->list(words, str);

  return words;
}

csTrie::MatchResult csTrie::find(const QString& str) const
{
  if( str.isEmpty() ) {
    return csTrie::NoMatch;
  }

  return _root->find(0, str);
}

void csTrie::insert(const QString& str)
{
  if( str.isEmpty() ) {
    return;
  }

  _root->insert(0, str);
}

QStringList csTrie::list() const
{
  QStringList words;

  QString str;
  str.reserve(1024);
  _root->list(words, str);

  return words;
}

int csTrie::size() const
{
  int numNodes = 0;
  int numLinks = 0;
  _root->statistics(numNodes, numLinks);

  return sizeof(csTrieNode)*numNodes + sizeof(csTrieNode*)*numLinks;
}
