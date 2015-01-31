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

#include <QtCore/QByteArray>

#include <csPDF/csPdfContentsNode.h>

#include <csPDF/csPdfUtil.h>

////// public ////////////////////////////////////////////////////////////////

csPdfContentsNode::csPdfContentsNode(const char *title,
                                     const int destPage,
                                     csPdfContentsNode *parent)
  : _title(0)
  , _parent(parent)
  , _children(0)
  , _page(destPage)
  , _numChildren(0)
{
  _title = qstrdup(title);
}

csPdfContentsNode::~csPdfContentsNode()
{
  delete[] _title;

  for(int i = 0; i < _numChildren; i++) {
    delete _children[i];
  }
  delete[] _children;
}

QString csPdfContentsNode::title() const
{
  return _U8(_title);
}

int csPdfContentsNode::page() const
{
  return _page;
}

void csPdfContentsNode::appendChild(const char *title, const int destPage)
{
  appendChild(new csPdfContentsNode(title, destPage, this));
}

void csPdfContentsNode::appendChild(const csPdfContentsNode *child)
{
  if( child == 0 ) {
    return;
  }

  csPdfContentsNode **old = _children;

  _children = new csPdfContentsNode*[_numChildren+1];
  if( _children == 0 ) {
    _children = old;
    return;
  }

  for(int i = 0; i < _numChildren; i++) {
    _children[i] = old[i];
  }
  _children[_numChildren] = const_cast<csPdfContentsNode*>(child);

  delete[] old;
  _numChildren++;
}

csPdfContentsNode *csPdfContentsNode::child(int row)
{
  if( row < 0  ||  row >= _numChildren ) {
    return 0;
  }
  return _children[row];
}

int csPdfContentsNode::childCount() const
{
  return _numChildren;
}

int csPdfContentsNode::columnCount() const
{
  return 1;
}

const csPdfContentsNode *csPdfContentsNode::constChild(int row) const
{
  if( row < 0  ||  row >= _numChildren ) {
    return 0;
  }
  return _children[row];
}

QVariant csPdfContentsNode::data(int /*column*/) const
{
  return _U8(_title);
}

csPdfContentsNode *csPdfContentsNode::parent()
{
  return _parent;
}

int csPdfContentsNode::row() const
{
  if( _parent != 0 ) {
    for(int i = 0; i < _parent->_numChildren; i++) {
      if( _parent->_children[i] == this ) {
        return i;
      }
    }
  }
  return 0;
}
