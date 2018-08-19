/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QRegExp>

#include "gui/environmentmodel.h"

////// public ////////////////////////////////////////////////////////////////

EnvironmentModel::EnvironmentModel(csILogger *logger, QObject *parent)
  : QAbstractTableModel(parent)
  , eval::Environment(3, logger)
  , _vars()
{
}

EnvironmentModel::~EnvironmentModel()
{
}

int EnvironmentModel::columnCount(const QModelIndex&) const
{
  return static_cast<int>(dimension());
}

QVariant EnvironmentModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  const int row = index.row();
  const int col = index.column();
  if( role == Qt::DisplayRole ) {
    const Variable& v = _vars.at(row);
    if( col < v.fields.size() ) {
      return v.fields.at(col);
    }
  }
  return QVariant();
}

Qt::ItemFlags EnvironmentModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags f = QAbstractTableModel::flags(index);
  return f;
}

QVariant EnvironmentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if( role == Qt::DisplayRole ) {
    if(        orientation == Qt::Horizontal ) {
      return QString("[%1]").arg(section);
    } else if( orientation == Qt::Vertical ) {
      return _vars.at(section).name;
    }
  }
  return QVariant();
}

int EnvironmentModel::rowCount(const QModelIndex&) const
{
  return _vars.size();
}

////// public slots //////////////////////////////////////////////////////////

void EnvironmentModel::evaluate(const QString& s)
{
  eval::Environment::evaluate(s.toStdString());
}

////// protected /////////////////////////////////////////////////////////////

void EnvironmentModel::on_EnvironmentChange()
{
  beginResetModel();

  _vars.clear();

  std::list<AST::IdentifierType> names = variables().variables();
  for(const AST::IdentifierType& name : names) {
    Variable v(QString::fromStdString(name));

    v.fields = fields(v.name);

    _vars.push_back(v);
  }

  qSort(_vars);

  endResetModel();
}

void EnvironmentModel::on_VariableChange(const AST::IdentifierType& /*name*/)
{
  on_EnvironmentChange();
}

////// private ///////////////////////////////////////////////////////////////

QStringList EnvironmentModel::fields(const QString& name) const
{
  const std::string value = print(name.toStdString());
  return QString::fromStdString(value).split(QRegExp("\\s+"),
                                             QString::SkipEmptyParts);
}
