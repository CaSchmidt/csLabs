/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
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

#include <QtWidgets/QDoubleSpinBox>

#include <SimCore/SimUtil.h>
#include <SimCore/SimValuesModel.h>

#include "valuedelegate.h"

////// public ////////////////////////////////////////////////////////////////

ValueDelegate::ValueDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

ValueDelegate::~ValueDelegate()
{
}

QWidget *ValueDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem& /*option*/,
                                     const QModelIndex& index) const
{
  QDoubleSpinBox *spin = new QDoubleSpinBox(parent);
  spin->setLocale(sim::locale());
  spin->setValue(index.data(Qt::EditRole).toDouble());
  spin->setDecimals(index.data(SimValuesModel::GetValuePrecision).toInt());
  spin->setSingleStep(1.0);
  spin->setMinimum(index.data(SimValuesModel::GetMinValue).toDouble());
  spin->setMaximum(index.data(SimValuesModel::GetMaxValue).toDouble());
  return spin;
}

void ValueDelegate::setEditorData(QWidget * /*editor*/,
                                  const QModelIndex& /*index*/) const
{
  // NOTE: Inhibit cyclic updates; set value during creation ONLY!
}

void ValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex& index) const
{
  QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(editor);
  spin->interpretText();
  model->setData(index, spin->value(), Qt::EditRole);
}

void ValueDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& /*index*/) const
{
  editor->setGeometry(option.rect);
}
