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

#include <QtCore/QDir>
#include <QtGui/QColor>

#include <csQt/csQtUtil.h>

#include "chaptermodel.h"

#include "chapter.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  csAbstractTreeItem *treeItem(const QModelIndex& index)
  {
    return static_cast<csAbstractTreeItem*>(index.internalPointer());
  }

}

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
  ChapterFile *file = dynamic_cast<ChapterFile*>(priv::treeItem(index));
  if( file == 0 ) {
    return QModelIndex();
  }
  ChapterNode *sources = dynamic_cast<ChapterNode*>(file->parentItem());
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

void ChapterModel::setData(csAbstractTreeItem *data)
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

Jobs ChapterModel::buildJobs() const
{
  Jobs jobs;

  // NOTE: Don't Build Job for <Files>!!! -> rowCount()-1
  for(int cntNode = 0; cntNode < rowCount(QModelIndex())-1; cntNode++) {
    QModelIndex nodeIndex = index(cntNode, 0, QModelIndex());

    Job job;

    for(int cntFile = 0; cntFile < rowCount(nodeIndex); cntFile++) {
      QModelIndex fileIndex = index(cntFile, 0, nodeIndex);

      ChapterFile *file = dynamic_cast<ChapterFile*>(priv::treeItem(fileIndex));
      job.inputFiles.push_back(file->fileName());
    } // File

    ChapterNode *node = dynamic_cast<ChapterNode*>(priv::treeItem(nodeIndex));
    const QString title = chapterTitle(node);

    const QString outputDir = QFileInfo(job.inputFiles.front()).absolutePath();
    job.outputFile = QFileInfo(QDir(outputDir), title+_L1(".mp3")).absoluteFilePath();

    jobs.push_back(job);
  } // Node

  return jobs;
}

void ChapterModel::deleteJobs()
{
  if( rowCount(QModelIndex()) < 2 ) {
    return;
  }

  const int numJobs = rowCount(QModelIndex())-1;
  beginRemoveRows(QModelIndex(), 0, numJobs-1);
  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  for(int i = 0; i < numJobs; i++) {
    root->removeAt(0);
  }
  endRemoveRows();
}

int ChapterModel::columnCount(const QModelIndex& parent) const
{
  if( parent.isValid() ) {
    return priv::treeItem(parent)->columnCount();
  }
  return _root->columnCount();
}

QVariant ChapterModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }

  csAbstractTreeItem *item = priv::treeItem(index);
  if(        role == Qt::DisplayRole ) {
    ChapterNode *node = dynamic_cast<ChapterNode*>(priv::treeItem(index));
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

  csAbstractTreeItem *item = priv::treeItem(index);
  ChapterNode    *nodeItem = dynamic_cast<ChapterNode*>(item);
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

  csAbstractTreeItem *parentItem(0);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = priv::treeItem(parent);
  }

  csAbstractTreeItem *childItem = parentItem->childItem(row);
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

  csAbstractTreeItem  *childItem = priv::treeItem(index);
  csAbstractTreeItem *parentItem = childItem->parentItem();

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

  csAbstractTreeItem *parentItem(0);
  if( !parent.isValid() ) {
    parentItem = _root;
  } else {
    parentItem = priv::treeItem(parent);
  }

  return parentItem->rowCount();
}

bool ChapterModel::setData(const QModelIndex& index, const QVariant& value,
                           int role)
{
  ChapterNode *node = dynamic_cast<ChapterNode*>(priv::treeItem(index));
  if(     node == 0                           ||  node->isSource()
      ||  value.type() != QMetaType::QString  ||  role != Qt::EditRole ) {
    return false;
  }

  node->setTitle(value.toString());
  emit dataChanged(index, index);

  return true;
}

////// public slots //////////////////////////////////////////////////////////

void ChapterModel::dissolveLastChapter()
{
  if( rowCount(QModelIndex()) < 2 ) {
    return;
  }

  const int dissolveRow = rowCount(QModelIndex())-2;

  beginRemoveRows(QModelIndex(), dissolveRow, dissolveRow);
  ChapterNode *node = dynamic_cast<ChapterNode*>(_root->childItem(dissolveRow));
  const QStringList dissolvedFiles = node->files(node->rowCount());

  ChapterRoot *root = dynamic_cast<ChapterRoot*>(_root);
  root->removeAt(dissolveRow);
  endRemoveRows();

  const QModelIndex sourcesIndex =
      index(rowCount(QModelIndex())-1, 0, QModelIndex());
  beginInsertRows(sourcesIndex, 0, dissolvedFiles.size()-1);
  ChapterNode *sources = dynamic_cast<ChapterNode*>(priv::treeItem(sourcesIndex));
  sources->insertFiles(dissolvedFiles);
  endInsertRows();
}

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
        .arg(_firstChapterNo+node->row(), _widthChapterNo, 10, _L1C('0'))
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
