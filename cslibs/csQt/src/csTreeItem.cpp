/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
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

#include "csQt/csTreeItem.h"

////// Public ////////////////////////////////////////////////////////////////

CS_QT_EXPORT csITreeItem *csTreeItem(const QModelIndex& index)
{
  return static_cast<csITreeItem*>(index.internalPointer());
}

////// public ////////////////////////////////////////////////////////////////

csITreeItem::csITreeItem(csITreeItem *parent)
  : _children()
  , _parent(parent)
{
}

csITreeItem::~csITreeItem()
{
  qDeleteAll(_children);
}

void csITreeItem::appendChild(csITreeItem *child)
{
  if( child != 0 ) {
    child->_parent = this;
    _children.append(child);
  }
}

void csITreeItem::insertChild(int i, csITreeItem *child)
{
  if( child != 0 ) {
    child->_parent = this;
    _children.insert(i, child);
  }
}

csITreeItem *csITreeItem::takeChild(int i)
{
  if( i < 0  ||  i >= _children.size() ) {
    return 0;
  }
  csITreeItem *child = _children.takeAt(i);
  child->_parent = 0;
  return child;
}

void csITreeItem::removeChild(int i)
{
  if( i >= 0  &&  i < _children.size() ) {
    delete _children.takeAt(i);
  }
}

csITreeItem *csITreeItem::childItem(int row) const
{
  return _children.value(row, 0);
}

csITreeItem *csITreeItem::parentItem() const
{
  return _parent;
}

int csITreeItem::row() const
{
  if( _parent != 0 ) {
    return _parent->_children.indexOf(const_cast<csITreeItem*>(this));
  }
  return 0;
}

int csITreeItem::rowCount() const
{
  return _children.size();
}
