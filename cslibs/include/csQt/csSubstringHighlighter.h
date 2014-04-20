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

#ifndef __CSSUBSTRINGHIGHLIGHTER_H__
#define __CSSUBSTRINGHIGHLIGHTER_H__

#include <QtCore/QRegExp>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>

#include <csQt/csqt_config.h>

class CS_QT_EXPORT csSubstringHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
public:
  csSubstringHighlighter(QTextDocument *document,
                         const Qt::CaseSensitivity cs = Qt::CaseSensitive);
  ~csSubstringHighlighter();

  void setBackground(const QBrush& brush);
  void setBackground(const QColor& color);
  void setBold(const bool on);
  void setCaseSensitivity(const Qt::CaseSensitivity cs);
  void setFont(const QFont& font);
  void setForeground(const QBrush& brush);
  void setForeground(const QColor& color);

public slots:
  void setSubstring(const QString& substring);

private:
  void highlightBlock(const QString& text);

  QRegExp _matcher;
  QTextCharFormat _fmt;
};

#endif // __CSSUBSTRINGHIGHLIGHTER_H__
