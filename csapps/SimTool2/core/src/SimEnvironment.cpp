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

#include <QtCore/QRegExp>

#include "SimCore/SimEnvironment.h"

#include "SimCore/SimContext.h"

////// private ///////////////////////////////////////////////////////////////

SimModule SimEnvironment::_module0;
SimVariable SimEnvironment::_variable0;

////// public ////////////////////////////////////////////////////////////////

SimEnvironment::SimEnvironment(SimContext *simctx)
  : QObject(simctx)
  , _modules()
  , _variables()
{
}

SimEnvironment::~SimEnvironment()
{
}

void SimEnvironment::clear()
{
  _variables.clear();
  emit variablesCleared();

  _modules.clear();
  emit modulesCleared();
}

bool SimEnvironment::insertModule(const SimModule& mod)
{
  if( !isValidModuleFileName(mod.fileName()) ) {
    return false;
  }
  _modules.insert(mod.name(), mod);
  emit moduleInserted(mod.name());
  return true;
}

bool SimEnvironment::isModuleActive(const QString& name) const
{
  if( !_modules.contains(name) ) {
    return false;
  }
  return _modules[name].isActive();
}

bool SimEnvironment::isValidModuleFileName(const QString& filename) const
{
  if( filename.isEmpty()  ||  _modules.contains(SimModule::moduleName(filename)) ) {
    return false;
  }
  return true;
}

QStringList SimEnvironment::listModules() const
{
  QStringList l = _modules.keys();
  qSort(l);
  return l;
}

SimModules SimEnvironment::modules() const
{
  return _modules.values();
}

SimModule SimEnvironment::removeModule(const QString& name)
{
  const SimModule mod = _modules.take(name);
  emit moduleRemoved(name);
  return mod;
}

void SimEnvironment::setModuleActive(const QString& name, const bool active)
{
  if( _modules.contains(name) ) {
    _modules[name].setActive(active);
  }
}

bool SimEnvironment::insertVariable(const SimVariable& var)
{
  if( var.isEmpty()  ||  !isValidVariableName(var.name()) ) {
    return false;
  }
  _variables.insert(var.name(), var);
  emit variableInserted(var.name());
  return true;
}

bool SimEnvironment::isValidVariableName(const QString& name) const
{
  if( name.isEmpty()  ||  _variables.contains(name) ) {
    return false;
  }
  const QRegExp rx(QString::fromLatin1("[_a-zA-Z][_a-zA-Z0-9]*"   // C-identifier
                                       "(?:\\[[_a-zA-Z0-9]+\\])*" // array indices
                                       "(?:\\."                   // structure nesting
                                       "[_a-zA-Z][_a-zA-Z0-9]*"   // C-identifier
                                       "(?:\\[[_a-zA-Z0-9]+\\])*" // array indices
                                       ")*"                       // end nesting
                                       ));
  return rx.exactMatch(name);
}

QStringList SimEnvironment::listVariables() const
{
  QStringList l = _variables.keys();
  qSort(l);
  return l;
}

SimVariable SimEnvironment::removeVariable(const QString& name)
{
  const SimVariable var = _variables.take(name);
  emit variableRemoved(name);
  return var;
}

const SimVariable& SimEnvironment::variable(const QString& name) const
{
  if( !_variables.contains(name) ) {
    return _variable0;
  }
  return _variables.find(name).value();
}

SimVariables SimEnvironment::variables() const
{
  return _variables.values();
}
