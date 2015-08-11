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

#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QTextDocument>
#include <QtWidgets/QApplication>

#include "csQt/csHighlightingDelegate.h"

#include "csQt/csSubstringHighlighter.h"

////// public ////////////////////////////////////////////////////////////////

csHighlightingDelegate::csHighlightingDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
  , _substring()
{
}

csHighlightingDelegate::~csHighlightingDelegate()
{
}

void csHighlightingDelegate::setSubstring(const QString& substring)
{
  _substring = substring;
}

void csHighlightingDelegate::paint(QPainter *painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
  if( index.data(Qt::DisplayRole).type() != QMetaType::QString ) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  const QWidget *widget = opt.widget;
  QStyle *style = widget != 0 ?  widget->style() : QApplication::style();

  painter->save();
  painter->setClipRect(opt.rect);

  // Draw Background
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

  // Draw Text
  if( !opt.text.isEmpty() ) {
    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled
        ? QPalette::Normal
        : QPalette::Disabled;
    if( cg == QPalette::Normal && !(opt.state & QStyle::State_Active) ) {
      cg = QPalette::Inactive;
    }

    QColor textColor;
    if( opt.state & QStyle::State_Selected ) {
      textColor = opt.palette.color(cg, QPalette::HighlightedText);
    } else {
      textColor = opt.palette.color(cg, QPalette::Text);
    }

    QTextDocument document;
    csSubstringHighlighter highlighter(&document, Qt::CaseInsensitive);
    highlighter.setBackground(QBrush(Qt::NoBrush));
    highlighter.setForeground(QBrush(textColor, Qt::SolidPattern));
    layoutText(&document, &highlighter, opt);

    painter->setTransform(QTransform::fromTranslate(opt.rect.x(),
                                                    opt.rect.y()));
#if 1
    QAbstractTextDocumentLayout::PaintContext context;
    context.palette.setColor(QPalette::Text, textColor);
    document.documentLayout()->draw(painter, context);
#else
    document.drawContents(painter);
#endif
    painter->resetTransform(); // Don't Choke on the Focus Rect
  }

  // Draw Focus Rect
  if( opt.state & QStyle::State_HasFocus ) {
    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(opt);
    o.rect = style->subElementRect(QStyle::SE_ItemViewItemFocusRect, &opt, widget);
    o.state |= QStyle::State_KeyboardFocusChange;
    o.state |= QStyle::State_Item;
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
        ? QPalette::Normal
        : QPalette::Disabled;
    o.backgroundColor = opt.palette.color(cg, (opt.state & QStyle::State_Selected)
                                          ? QPalette::Highlight
                                          : QPalette::Window);
    style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
  }

  painter->restore();
}

QSize csHighlightingDelegate::sizeHint(const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
{
  if( index.data(Qt::DisplayRole).type() != QMetaType::QString ) {
    return QStyledItemDelegate::sizeHint(option, index);
  }

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  QTextDocument document;
  csSubstringHighlighter highlighter(&document, Qt::CaseInsensitive);
  layoutText(&document, &highlighter, opt);

  return document.size().toSize();
}

////// private ///////////////////////////////////////////////////////////////

void csHighlightingDelegate::layoutText(QTextDocument *document,
                                        class csSubstringHighlighter *highlighter,
                                        const QStyleOptionViewItem& option) const
{
  // Configure Document
  document->setDefaultFont(option.font);
  // Configure Highlighter
  highlighter->setFont(option.font);
  highlighter->setBold(true);
  // Layout
  document->setPlainText(option.text);
  highlighter->setSubstring(_substring);
}
