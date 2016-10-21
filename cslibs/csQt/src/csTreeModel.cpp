/****************************************************************************
** Copyright (c) 2016, Carsten Schmidt. All rights reserved.
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

#include "csQt/csTreeModel.h"

#include "csQt/csAbstractTreeItem.h"

////// Public ////////////////////////////////////////////////////////////////

CS_QT_EXPORT csAbstractTreeItem *csTreeItem(const QModelIndex& index)
{
  return static_cast<csAbstractTreeItem*>(index.internalPointer());
}

////// public ////////////////////////////////////////////////////////////////

csTreeModel::csTreeModel(csAbstractTreeItem *rootItem, QObject *parent)
  : QAbstractItemModel(parent)
  , _rootItem(rootItem)
{
}

csTreeModel::~csTreeModel()
{
  delete _rootItem;
}

void csTreeModel::setTree(csAbstractTreeItem *rootItem)
{
  if( rootItem != 0 ) {
    beginResetModel();
    delete _rootItem;
    _rootItem = rootItem;
    endResetModel();
  }
}

int csTreeModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() ) {
    return csTreeItem(parent)->columnCount();
  }
  return _rootItem->columnCount();
}

QVariant csTreeModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  return csTreeItem(index)->data(index.column(), role);
}

Qt::ItemFlags csTreeModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return 0;
  }
  return QAbstractItemModel::flags(index);
}

QVariant csTreeModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
  if( orientation == Qt::Horizontal ) {
    return _rootItem->data(section, role);
  }
  return QVariant();
}

QModelIndex csTreeModel::index(int row, int column,
                               const QModelIndex& parent) const
{
  if( !hasIndex(row, column, parent) ) {
    return QModelIndex();
  }

  csAbstractTreeItem *parentItem = parent.isValid()
      ? csTreeItem(parent)
      : _rootItem;

  csAbstractTreeItem *childItem = parentItem->childItem(row);
  if( childItem != 0 ) {
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex csTreeModel::parent(const QModelIndex& child) const
{
  if( !child.isValid() ) {
    return QModelIndex();
  }

  csAbstractTreeItem  *childItem = csTreeItem(child);
  csAbstractTreeItem *parentItem = childItem->parentItem();

  if( parentItem == _rootItem ) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int csTreeModel::rowCount(const QModelIndex& parent) const
{
  if( parent.column() > 0 ) {
    return 0;
  }

  csAbstractTreeItem *parentItem = parent.isValid()
      ? csTreeItem(parent)
      : _rootItem;

  return parentItem->rowCount();
}
