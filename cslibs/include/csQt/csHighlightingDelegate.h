/****************************************************************************
** Copyright (c) 2013-2014, Carsten Schmidt. All rights reserved.
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

#ifndef __CSHIGHLIGHTINGDELEGATE_H__
#define __CSHIGHLIGHTINGDELEGATE_H__

#include <QtCore/QModelIndex>
#include <QtGui/QPainter>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyleOptionViewItem>

#include <csQt/csqt_config.h>

class CS_QT_EXPORT csHighlightingDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  csHighlightingDelegate(QObject *parent = 0);
  ~csHighlightingDelegate();

  void setSubstring(const QString& substring);

  void paint(QPainter *painter,
             const QStyleOptionViewItem& option, const QModelIndex& index) const;
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
  void layoutText(class QTextDocument *document,
                  class csSubstringHighlighter *highlighter,
                  const QStyleOptionViewItem& option) const;

  QString _substring;
};

#endif // __CSHIGHLIGHTINGDELEGATE_H__
