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

#include "csQt/csSubstringHighlighter.h"

#include "csQt/csQtUtil.h"

////// public ////////////////////////////////////////////////////////////////

csSubstringHighlighter::csSubstringHighlighter(QTextDocument *document,
                                               const Qt::CaseSensitivity cs)
  : QSyntaxHighlighter(document)
  , _matcher(QString(), cs, QRegExp::RegExp)
  , _fmt()
{
  setBackground(QColor());
  setBold(true);
  setForeground(QColor(Qt::black));
}

csSubstringHighlighter::~csSubstringHighlighter()
{
}

void csSubstringHighlighter::setBackground(const QBrush& brush)
{
  _fmt.setBackground(brush);
}

void csSubstringHighlighter::setBackground(const QColor& color)
{
  if( color.isValid() ) {
    _fmt.setBackground(QBrush(color, Qt::SolidPattern));
  } else {
    _fmt.setBackground(QBrush(Qt::NoBrush));
  }
}

void csSubstringHighlighter::setBold(const bool on)
{
  if( on ) {
    _fmt.setFontWeight(QFont::Bold);
  } else {
    _fmt.setFontWeight(QFont::Normal);
  }
}

void csSubstringHighlighter::setCaseSensitivity(const Qt::CaseSensitivity cs)
{
  _matcher.setCaseSensitivity(cs);
}

void csSubstringHighlighter::setFont(const QFont& font)
{
  _fmt.setFont(font);
}

void csSubstringHighlighter::setForeground(const QBrush& brush)
{
  _fmt.setForeground(brush);
}

void csSubstringHighlighter::setForeground(const QColor& color)
{
  if( color.isValid() ) {
    _fmt.setForeground(QBrush(color, Qt::SolidPattern));
  } else {
    _fmt.setForeground(QBrush(Qt::black, Qt::SolidPattern));
  }
}

////// public slots //////////////////////////////////////////////////////////

void csSubstringHighlighter::setSubstring(const QString& substring)
{
  const QStringList words = substring.split(QRegExp(_L1("\\s+")), QString::SkipEmptyParts);
  if( words.isEmpty() ) {
    _matcher.setPattern(QString());
  } else {
    _matcher.setPattern(words.join(_L1("\\s+")));
  }
  rehighlight();
}

////// private ///////////////////////////////////////////////////////////////

void csSubstringHighlighter::highlightBlock(const QString& text)
{
  if( _matcher.pattern().isEmpty() ) {
    return;
  }

  int pos = 0;
  while( (pos = _matcher.indexIn(text, pos)) >= 0 ) {
    setFormat(pos, _matcher.matchedLength(), _fmt);
    pos += _matcher.matchedLength();
  }
}
