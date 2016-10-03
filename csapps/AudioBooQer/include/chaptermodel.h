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

#ifndef __CHAPTERMODEL_H__
#define __CHAPTERMODEL_H__

#include <QtCore/QAbstractItemModel>

#include "job.h"

class csAbstractTreeItem;

class ChapterModel : public QAbstractItemModel {
  Q_OBJECT
public:
  ChapterModel(QObject *parent = 0);
  ~ChapterModel();

  QModelIndex createNewChapter(const QModelIndex& index);
  void setData(csAbstractTreeItem *data);

  bool showChapterNo() const;
  int firstChaopterNo() const;
  int widthChapterNo() const;

  Jobs buildJobs() const;
  void deleteJobs();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  bool setData(const QModelIndex& index, const QVariant& value,
               int role = Qt::EditRole);

public slots:
  void dissolveLastChapter();
  void setFirstChapterNo(int no);
  void setShowChapterNo(bool state);
  void setWidthChapterNo(int width);
  void setPlayingFileName(const QString& fileName);

private:
  QString chapterTitle(const class ChapterNode *node) const;
  void updateChapters();

  csAbstractTreeItem *_root;
  bool _showChapterNo;
  int _firstChapterNo;
  int _widthChapterNo;
  QString _playingFileName;
};

#endif // __CHAPTERMODEL_H__
