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

#ifndef __CHAPTER_H__
#define __CHAPTER_H__

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <csQt/csAbstractTreeItem.h>

class ChapterRoot;
class ChapterNode;
class ChapterFile;

bool isRoot(csAbstractTreeItem *item);
bool isNode(csAbstractTreeItem *item);
bool isFile(csAbstractTreeItem *item);

class ChapterRoot : public csAbstractTreeItem { // The Root of All Evil /////////////
public:
  ChapterRoot();
  ~ChapterRoot();

  bool insert(ChapterNode *node);
  bool removeAt(const int index);

  int columnCount() const;
  QVariant data(int column, int role) const;
};

class ChapterNode : public csAbstractTreeItem { // One Chapter //////////////////////
public:
  ChapterNode(csAbstractTreeItem *parent, const bool isSource = false);
  ~ChapterNode();

  bool isSource() const;

  QStringList files(const int count) const;
  int insertFiles(const QStringList& fileNames);
  int remove(const int count);
  int setFiles(const QStringList& fileNames);

  const QString& title() const;
  void setTitle(const QString& title);

  int columnCount() const;
  QVariant data(int column, int role) const;

private:
  bool _isSource;
  QString _title;
};

class ChapterFile : public csAbstractTreeItem { // One File in a Chapter ////////////
public:
  ChapterFile(csAbstractTreeItem *parent);
  ~ChapterFile();

  const QString& fileName() const;
  void setFileName(const QString& fileName);

  int columnCount() const;
  QVariant data(int column, int role) const;

private:
  QString _fileName;
};

#endif // __CHAPTER_H__
