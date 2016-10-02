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

#ifndef __CSTREEITEM_H__
#define __CSTREEITEM_H__

#include <QtCore/QList>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

#include <csQt/csqt_config.h>

class CS_QT_EXPORT csITreeItem {
public:
  csITreeItem(csITreeItem *parent = 0);
  virtual ~csITreeItem();

  void appendChild(csITreeItem *child);
  void insertChild(int i, csITreeItem *child);
  csITreeItem *takeChild(int i);
  void removeChild(int i);

  csITreeItem *childItem(int row) const;
  csITreeItem *parentItem() const;
  int row() const;

  int rowCount() const;

  virtual int columnCount() const = 0;
  virtual QVariant data(int column, int role = Qt::DisplayRole) const = 0;

private:
  Q_DISABLE_COPY(csITreeItem)

  QList<csITreeItem*> _children;
  csITreeItem *_parent;
};

CS_QT_EXPORT csITreeItem *csTreeItem(const QModelIndex& index);

#endif // __CSTREEITEM_H__
