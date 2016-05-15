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

#ifndef __SIMENVIRONMENT_H__
#define __SIMENVIRONMENT_H__

#include <QtCore/QHash>
#include <QtCore/QObject>

#include <SimCore/simcore_config.h>
#include <SimCore/SimModule.h>
#include <SimCore/SimVariable.h>

class SimContext;

class SIM_CORE_EXPORT SimEnvironment : public QObject {
  Q_OBJECT
public:
  SimEnvironment(SimContext *simctx);
  ~SimEnvironment();

  void clear();

  bool insertModule(const SimModule& mod);
  bool isModuleActive(const QString& name) const;
  bool isValidModuleFileName(const QString& filename) const;
  QStringList listModules() const;
  SimModules modules() const;
  SimModule removeModule(const QString& name);
  void setModuleActive(const QString& name, const bool active);

  bool insertVariable(const SimVariable& var);
  bool isValidVariableName(const QString& name) const;
  QStringList listVariables() const;
  SimVariable removeVariable(const QString& name);
  const SimVariable& variable(const QString& name) const;
  SimVariables variables() const;

signals:
  void moduleInserted(const QString& name);
  void moduleRemoved(const QString& name);
  void modulesCleared();
  void variableInserted(const QString& name);
  void variableRemoved(const QString& name);
  void variablesCleared();

private:
  static SimModule _module0;
  static SimVariable _variable0;
  QHash<QString,SimModule> _modules;
  QHash<QString,SimVariable> _variables;
};

#endif // __SIMENVIRONMENT_H__
