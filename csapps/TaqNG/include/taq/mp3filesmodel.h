/****************************************************************************
** Copyright (c) 2005-2014, Carsten Schmidt. All rights reserved.
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

#ifndef __MP3FILESMODEL_H__
#define __MP3FILESMODEL_H__

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QStringList>

class Mp3FilesModel : public QAbstractListModel {
  Q_OBJECT
public:
  Mp3FilesModel(const QStringList& strings, const bool validate = false,
		QObject *parent = 0);
  ~Mp3FilesModel();

  QStringList stringList() const;
  void setStringList(const QStringList& list);

  QStringList activeFiles() const;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  bool setData(const QModelIndex& index, const QVariant& value,
	       int role = Qt::EditRole);

private:
  Mp3FilesModel();

  struct Mp3File {
    Mp3File(const QString& fn = QString(), const bool a = false)
      : filename(fn), active(a)
    {
    }

    QString filename;
    bool active;
  };

  QList<Mp3File> _files;
  const bool _validate;
};

#endif // __MP3FILESMODEL_H__
