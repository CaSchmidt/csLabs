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

#include <QtGui/QColor>

#include <csQt/csQtUtil.h>

#include "chaptermodel.h"

#include "chapter.h"

////// public ////////////////////////////////////////////////////////////////

ChapterModel::ChapterModel(QObject *parent)
  : QAbstractItemModel(parent)
  , _root(0)
  , _showChapterNo(true)
  , _firstChapterNo(1)
  , _widthChapterNo(2)
  , _playingFileName()
{
  _root = new ChapterRoot();
}

ChapterModel::~ChapterModel()
{
  delete _root;
}

QModelIndex ChapterModel::createNewChapter(const QModelIndex& index)
{
  ChapterFile *file = dynamic_cast<ChapterFile*>(csTreeItem(index));
  if( file == 0 ) {
    return QModelIndex();
  }
  ChapterNode *sources = dynamic_cast<ChapterNode*>(file->parent());
  if( sources == 0  ||  !sources->isSource() ) {
    return QModelIndex();
  }

  const int count = index.row()+1;

  const QStringList files = sources->files(count);

  const QModelIndex parent = ChapterModel::parent(index);
  beginRemoveRows(parent, 0, count-1);
  sources->remove(count);
  endRemoveRows();

  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  beginInsertRows(QModelIndex(), sources->row(), sources->row());
  ChapterNode *newNode = new ChapterNode(root);
  root->insert(newNode);
  newNode->setTitle(tr("New Chapter"));
  newNode->setFiles(files);
  endInsertRows();

  return ChapterModel::index(newNode->row(), 0, QModelIndex());
}

void ChapterModel::setData(csITreeItem *data)
{
  beginResetModel();

  delete _root;

  if( data != 0 ) {
    _root = data;
  } else {
    _root = new ChapterRoot();
  }

  endResetModel();
}

bool ChapterModel::showChapterNo() const
{
  return _showChapterNo;
}

int ChapterModel::firstChaopterNo() const
{
  return _firstChapterNo;
}

int ChapterModel::widthChapterNo() const
{
  return _widthChapterNo;
}

int ChapterModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() ) {
    return csTreeItem(parent)->columnCount();
  }
  return _root->columnCount();
}

QVariant ChapterModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  csITreeItem *item = csTreeItem(index);
  if(        role == Qt::DisplayRole ) {
    ChapterNode *node = dynamic_cast<ChapterNode*>(csTreeItem(index));
    if( node != 0  &&  !node->isSource() ) {
      return chapterTitle(node);
    } else {
      return item->data(index.column());
    }
  } else if( role == Qt::EditRole ) {
    if( isNode(item) ) {
      ChapterNode *node = dynamic_cast<ChapterNode*>(item);
      return node->title();
    }
  } else if( role == Qt::DecorationRole ) {
    if( isFile(item) ) {
      ChapterFile *file = dynamic_cast<ChapterFile*>(item);
      if( file->fileName() == _playingFileName ) {
        return QColor(Qt::green);
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags ChapterModel::flags(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags f(Qt::ItemIsEnabled);

  csITreeItem   *item   = csTreeItem(index);
  ChapterNode *nodeItem = dynamic_cast<ChapterNode*>(item);
  if( isFile(item) ) {
    f |= Qt::ItemIsSelectable;
  } else if( nodeItem != 0  &&  !nodeItem->isSource() ) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}

QModelIndex ChapterModel::index(int row, int column,
                                const QModelIndex& parent) const
{
  if( !hasIndex(row, column, parent) ) {
    return QModelIndex();
  }

  csITreeItem *parentItem(0);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = csTreeItem(parent);
  }

  csITreeItem *childItem = parentItem->child(row);
  if( childItem != 0 ) {
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex ChapterModel::parent(const QModelIndex& index) const
{
  if( !index.isValid() ) {
    return QModelIndex();
  }

  csITreeItem *childItem  = csTreeItem(index);
  csITreeItem *parentItem = childItem->parent();

  if( parentItem == _root ) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int ChapterModel::rowCount(const QModelIndex& parent) const
{
  if( parent.column() > 0 ) {
    return 0;
  }

  csITreeItem *parentItem(0);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = csTreeItem(parent);
  }

  return parentItem->childCount();
}

bool ChapterModel::setData(const QModelIndex& index, const QVariant& value,
                           int role)
{
  ChapterNode *node = dynamic_cast<ChapterNode*>(csTreeItem(index));
  if(     node == 0                           ||  node->isSource()
      ||  value.type() != QMetaType::QString  ||  role != Qt::EditRole ) {
    return false;
  }

  node->setTitle(value.toString());
  emit dataChanged(index, index);

  return true;
}

////// public slots //////////////////////////////////////////////////////////

void ChapterModel::setFirstChapterNo(int no)
{
  _firstChapterNo = no;
  updateChapters();
}

void ChapterModel::setShowChapterNo(bool state)
{
  _showChapterNo = state;
  updateChapters();
}

void ChapterModel::setWidthChapterNo(int width)
{
  _widthChapterNo = width;
  updateChapters();
}

void ChapterModel::setPlayingFileName(const QString& fileName)
{
  _playingFileName = fileName;

  const QVector<int> roles = QVector<int>() << Qt::DecorationRole;
  for(int cntNode = 0; cntNode < rowCount(QModelIndex()); cntNode++) {
    QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    for(int cntFile = 0; cntFile < rowCount(nodeIndex); cntFile++) {
      QModelIndex fileIndex = index(cntFile, 0, nodeIndex);

      emit dataChanged(fileIndex, fileIndex, roles);
    }
  }
}

////// private ///////////////////////////////////////////////////////////////

QString ChapterModel::chapterTitle(const class ChapterNode *node) const
{
  if( _showChapterNo ) {
    return _L1("%1 - %2")
        .arg(_firstChapterNo+node->row(), _widthChapterNo, 10, QChar::fromLatin1('0'))
        .arg(node->title());
  }
  return node->title();
}

void ChapterModel::updateChapters()
{
  const QVector<int> roles = QVector<int>() << Qt::DisplayRole;
  for(int cntNode = 0; cntNode < rowCount(QModelIndex()); cntNode++) {
    QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    emit dataChanged(nodeIndex, nodeIndex, roles);
  }
}
