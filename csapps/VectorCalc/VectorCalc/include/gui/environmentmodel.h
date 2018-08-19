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

#ifndef ENVIRONMENTMODEL_H
#define ENVIRONMENTMODEL_H

#include <QtCore/QAbstractTableModel>

#include "eval/environment.h"

struct Variable {
  Variable(const QString& _name = QString(),
           const QStringList& _fields = QStringList())
    : name(_name)
    , fields(_fields)
  {
  }

  QString name;
  QStringList fields;
};

using Variables = QList<Variable>;

inline bool operator<(const QString& s, const Variable& v)
{
  return s < v.name;
}

inline bool operator<(const Variable& v, const QString& s)
{
  return v.name < s;
}

inline bool operator<(const Variable& a, const Variable& b)
{
  return a.name < b.name;
}

class csILogger;

class EnvironmentModel : public QAbstractTableModel, public eval::Environment {
  Q_OBJECT
public:
  EnvironmentModel(csILogger *logger, QObject *parent);
  ~EnvironmentModel();

  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex& index, int role) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  int rowCount(const QModelIndex&) const;

public slots:
  void evaluate(const QString& s);

protected:
  void on_EnvironmentChange();
  void on_VariableChange(const AST::IdentifierType& name);

private:
  QStringList fields(const QString& name) const;

  Variables _vars;
};

#endif // ENVIRONMENTMODEL_H
