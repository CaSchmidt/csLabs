/****************************************************************************
** Copyright (c) 2013-2015, Carsten Schmidt. All rights reserved.
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

#include <csPDF/csPdfContentsModel.h>

////// public ////////////////////////////////////////////////////////////////

csPdfContentsModel::csPdfContentsModel(QObject *parent)
  : QAbstractItemModel(parent)
  , _contents(0)
  , _displayed(0)
  , _filtered(0)
{
  _contents = newRootNode();
  _filtered = newRootNode();

  _displayed = _contents;
}

csPdfContentsModel::~csPdfContentsModel()
{
  delete _contents;
  delete _filtered;
}

csPdfContentsNode *csPdfContentsModel::newRootNode()
{
  return new csPdfContentsNode("<Root>", -1, 0);
}

void csPdfContentsModel::setRootNode(csPdfContentsNode *root)
{
  beginResetModel();

  delete _contents;
  delete _filtered;

  if( root == 0 ) {
    _contents = newRootNode();
  } else {
    _contents = root;
  }
  _filtered = newRootNode();

  _displayed = _contents;

  endResetModel();
}

int csPdfContentsModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() ) {
    return static_cast<csPdfContentsNode*>(parent.internalPointer())->columnCount();
  }

  return _displayed->columnCount();
}

QVariant csPdfContentsModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  if( role != Qt::DisplayRole ) {
    return QVariant();
  }

  csPdfContentsNode *item = static_cast<csPdfContentsNode*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags csPdfContentsModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  return QAbstractItemModel::flags(index);
}

QVariant csPdfContentsModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
  if( orientation == Qt::Horizontal  &&  role == Qt::DisplayRole ) {
    return _displayed->data(section);
  }

  return QVariant();
}

QModelIndex csPdfContentsModel::index(int row, int column,
                                      const QModelIndex& parent) const
{
  if( !hasIndex(row, column, parent) ) {
    return QModelIndex();
  }

  csPdfContentsNode *parentItem;
  if( !parent.isValid() ) {
    parentItem = _displayed;
  } else {
    parentItem = static_cast<csPdfContentsNode*>(parent.internalPointer());
  }

  csPdfContentsNode *childItem = parentItem->child(row);
  if( childItem != 0 ) {
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex csPdfContentsModel::parent(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return QModelIndex();
  }

  csPdfContentsNode *childItem  = static_cast<csPdfContentsNode*>(index.internalPointer());
  csPdfContentsNode *parentItem = childItem->parent();

  if( parentItem == _displayed ) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int csPdfContentsModel::rowCount(const QModelIndex& parent) const
{
  csPdfContentsNode *parentItem;
  if( parent.column() > 0 ) {
    return 0;
  }

  if( !parent.isValid() ) {
    parentItem = _displayed;
  } else {
    parentItem = static_cast<csPdfContentsNode*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

////// public slots //////////////////////////////////////////////////////////

void csPdfContentsModel::filter(const QString& pattern)
{
  beginResetModel();

  delete _filtered;
  _filtered = newRootNode();

  if( pattern.isEmpty() ) {
    _displayed = _contents;
  } else {
    filter(_contents, _filtered, pattern);
    _displayed = _filtered;
  }

  endResetModel();
}

////// private ///////////////////////////////////////////////////////////////

void csPdfContentsModel::filter(const csPdfContentsNode *node,
                                csPdfContentsNode *filtered,
                                const QString& pattern)
{
  for(int i = 0; i < node->childCount(); i++) {
    const csPdfContentsNode *child = node->constChild(i);
    if( child->title().contains(pattern, Qt::CaseInsensitive) ) {
      filtered->appendChild(child->title().toUtf8().constData(), child->page());
    }
    filter(child, filtered, pattern);
  }
}
