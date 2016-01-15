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

#ifndef __CSQTUTIL_H__
#define __CSQTUTIL_H__

#include <QtCore/QLocale>
#include <QtCore/QModelIndex>
#include <QtCore/QRect>
#include <QtCore/QRectF>
#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include <csQt/csqt_config.h>
#include <csQt/csNamespace.h>

#ifndef _L1
# define _L1(s)  QString::fromLatin1(s)
#endif

#ifndef _L1C
# define _L1C(c)  QChar::fromLatin1(c)
#endif

#ifndef _U8
# define _U8(s)  QString::fromUtf8(s)
#endif

class QColor;
class QPoint;
class QTableView;
class QVariant;
class QWidget;

inline qreal csScale(const QSizeF& from, const QSizeF& to)
{
  const qreal sh = to.width() /from.width();
  const qreal sv = to.height()/from.height();
  return from.height()*sh > to.height()
      ? sv
      : sh;
}

CS_QT_EXPORT QRect csScreenGeometry(const QPoint& globalPos, QWidget *widget);

CS_QT_EXPORT int csScreenNumber(const QPoint& globalPos, QWidget *widget);

CS_QT_EXPORT void csSetBackgroundColor(QWidget *w, const QColor& c,
                                       const bool inactive = true);

CS_QT_EXPORT QString csTableToString(const QTableView *table,
                                     const QModelIndex& topLeft = QModelIndex(),
                                     const QModelIndex& bottomRight = QModelIndex(),
                                     const int role = Qt::DisplayRole,
                                     const cs::TableExtractionFlags flags = cs::NoTableExtractionFlags);

CS_QT_EXPORT QString csVariantToString(const QVariant& variant,
                                       const QLocale& locale = QLocale(),
                                       const char format = 'g', const int precision = 6);

template<class T>
T *csFindParentWidget(QWidget *widget)
{
  if( widget == 0 ) {
    return 0;
  }

  while( (widget = widget->parentWidget()) != 0 ) {
    if( dynamic_cast<T*>(widget) != 0 ) {
      return dynamic_cast<T*>(widget);
    }
  }

  return 0;
}

#endif // __CSQTUTIL_H__
