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

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtCore/QDate>

#include "csQt/csQtUtil.h"

////// Public ////////////////////////////////////////////////////////////////

CS_QT_EXPORT QRect csScreenGeometry(const QPoint& globalPos, QWidget *widget)
{
  const int no = csScreenNumber(globalPos, widget);
  if( no < 0 ) {
    return QRect();
  }
  return QApplication::desktop()->screenGeometry(no);
}

CS_QT_EXPORT int csScreenNumber(const QPoint& globalPos, QWidget *widget)
{
  if( QApplication::desktop()->isVirtualDesktop() ) {
    return QApplication::desktop()->screenNumber(globalPos);
  }
  return QApplication::desktop()->screenNumber(widget);
}

CS_QT_EXPORT void csSetBackgroundColor(QWidget *w, const QColor &c,
                                       const bool inactive)
{
  QPalette p(w->palette());
  p.setColor(QPalette::Active, QPalette::Base, c);
  if( inactive ) {
    p.setColor(QPalette::Inactive, QPalette::Base, c);
  }
  w->setPalette(p);
}

CS_QT_EXPORT QString csTableToString(const QTableView *table,
                                     const QModelIndex& topLeft,
                                     const QModelIndex& bottomRight,
                                     const int role,
                                     const cs::TableExtractionFlags flags)
{
  const QAbstractItemModel *model =
      dynamic_cast<const QAbstractItemModel*>(table->model());

  // (1) Column Range ////////////////////////////////////////////////////////

  const int colFrom = model->hasIndex(topLeft.row(), topLeft.column())
      ? topLeft.column()
      : 0;

  const int colTo = model->hasIndex(bottomRight.row(), bottomRight.column())
      ? bottomRight.column()
      : model->columnCount() - 1;

  // (2) Row Range ///////////////////////////////////////////////////////////

  const int rowFrom = model->hasIndex(topLeft.row(), topLeft.column())
      ? topLeft.row()
      : 0;

  const int rowTo = model->hasIndex(bottomRight.row(), bottomRight.column())
      ? bottomRight.row()
      : model->rowCount() - 1;

  // (3) Horizontal Headers //////////////////////////////////////////////////

  const bool doHHeader =
      table->horizontalHeader()->isVisible() && flags.testFlag(cs::ExtractHHeader);

  // (4) Vertical Headers ////////////////////////////////////////////////////

  const bool doVHeader =
      table->verticalHeader()->isVisible() && flags.testFlag(cs::ExtractVHeader);

  // (5) Extraction //////////////////////////////////////////////////////////

  QString text;

  if( doHHeader ) {
    QStringList cols;
    if( doVHeader ) {
      cols.push_back(QString());
    }
    for(int c = colFrom; c <= colTo; c++) {
      cols.push_back(csVariantToString(model->headerData(c, Qt::Horizontal, role)));
    }
    text += cols.join(_L1C('\t'));
    text += _L1C('\n');
  }

  for(int r = rowFrom; r <= rowTo; r++) {
    QStringList cols;
    if( doVHeader ) {
      cols.push_back(csVariantToString(model->headerData(r, Qt::Vertical, role)));
    }
    for(int c = colFrom; c <= colTo; c++) {
      cols.push_back(csVariantToString(model->data(model->index(r, c), role)));
    }
    text += cols.join(_L1C('\t'));
    text += _L1C('\n');
  }

  return text;
}

CS_QT_EXPORT QString csVariantToString(const QVariant& variant, const QLocale& locale,
                                       const char format, const int precision)
{
  QString text;
  switch( variant.userType() ) {
  // case QMetaType::Short:
  case QMetaType::Int:
    // case QMetaType::Long:
  case QMetaType::LongLong:
    text = locale.toString(variant.toLongLong());
    break;

    // case QMetaType::UShort:
  case QMetaType::UInt:
    // case QMetaType::ULong:
  case QMetaType::ULongLong:
    text = locale.toString(variant.toULongLong());
    break;

  case QMetaType::Float:
  case QMetaType::Double:
    text = locale.toString(variant.toReal(), format, precision);
    break;

  case QMetaType::QDate:
    text = locale.toString(variant.toDate());
    break;

  case QMetaType::QTime:
    text = locale.toString(variant.toTime());
    break;

  case QMetaType::QDateTime:
    text = locale.toString(variant.toDateTime());
    break;

  default:
    text = variant.toString();
    break;
  }

  return text;
}
