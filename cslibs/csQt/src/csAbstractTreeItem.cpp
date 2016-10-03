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

#include "csQt/csAbstractTreeItem.h"

////// public ////////////////////////////////////////////////////////////////

csAbstractTreeItem::csAbstractTreeItem(csAbstractTreeItem *parent)
  : _children()
  , _parent(parent)
{
}

csAbstractTreeItem::~csAbstractTreeItem()
{
  qDeleteAll(_children);
}

void csAbstractTreeItem::appendChild(csAbstractTreeItem *child)
{
  if( child != 0 ) {
    child->_parent = this;
    _children.append(child);
  }
}

void csAbstractTreeItem::insertChild(int row, csAbstractTreeItem *child)
{
  if( child != 0 ) {
    child->_parent = this;
    _children.insert(row, child);
  }
}

csAbstractTreeItem *csAbstractTreeItem::takeChild(int row)
{
  if( row < 0  ||  row >= _children.size() ) {
    return 0;
  }
  csAbstractTreeItem *child = _children.takeAt(row);
  child->_parent = 0;
  return child;
}

void csAbstractTreeItem::removeChild(int row)
{
  if( row >= 0  &&  row < _children.size() ) {
    delete _children.takeAt(row);
  }
}

csAbstractTreeItem *csAbstractTreeItem::childItem(int row) const
{
  return _children.value(row, 0);
}

csAbstractTreeItem *csAbstractTreeItem::parentItem() const
{
  return _parent;
}

int csAbstractTreeItem::row() const
{
  if( _parent != 0 ) {
    return _parent->_children.indexOf(const_cast<csAbstractTreeItem*>(this));
  }
  return 0;
}

int csAbstractTreeItem::rowCount() const
{
  return _children.size();
}
