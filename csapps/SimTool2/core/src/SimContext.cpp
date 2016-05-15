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

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SimCore/SimContext.h"

////// public ////////////////////////////////////////////////////////////////

SimContext::SimContext(QObject *parent)
  : QObject(parent)
  , cfg()
  , ctrl(this)
  , db(this)
  , env(this)
  , sim(this)
{
  qRegisterMetaType<SimConfig>("SimConfig");

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(&ctrl, &SimControl::stateEntered,
          &sim, &SimSimulator::enterState);
  connect(&ctrl, &SimControl::stateExited,
          &sim, &SimSimulator::exitState);

  connect(&env, &SimEnvironment::variablesCleared,
          &db, &SimDb::clear);
  connect(&env, &SimEnvironment::variableInserted,
          &db, &SimDb::insertVariable);
  connect(&env, &SimEnvironment::variableRemoved,
          &db, &SimDb::removeVariable);

  connect(&sim, &SimSimulator::stopRequested,
          &ctrl, &SimControl::stopRequested);
}

SimContext::~SimContext()
{
}

bool SimContext::open(const QString& filename)
{
  // (1) Input ///////////////////////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QIODevice::ReadOnly) ) {
    return false;
  }

  QJsonParseError error;
  const QJsonDocument jsonFile = QJsonDocument::fromJson(file.readAll(), &error);
  file.close();
  if( error.error != QJsonParseError::NoError ) {
    logError(error.errorString());
    return false;
  }

  const QJsonObject ctxObj = jsonFile.object();

  // (2) Configuration ///////////////////////////////////////////////////////

  const QJsonObject cfgObj = ctxObj[QStringLiteral("configuration")].toObject();
  cfg.mode = sim::toOperationMode(cfgObj[QStringLiteral("mode")].toString());
  cfg.step = cfgObj[QStringLiteral("step")].toDouble();
  cfg.duration = cfgObj[QStringLiteral("duration")].toDouble();
  emit configChanged(cfg);

  // (3) Reset Environment ///////////////////////////////////////////////////

  env.clear();

  // (4) Variables ///////////////////////////////////////////////////////////

  const QJsonArray varsObj = ctxObj[QStringLiteral("variables")].toArray();
  foreach(const QJsonValue& valObj, varsObj) {
    const QJsonObject varObj = valObj.toObject();
    SimVariable var;
    var.setName(varObj[QStringLiteral("name")].toString());
    var.setType(sim::toDataType(varObj[QStringLiteral("type")].toString()));
    var.setUnit(varObj[QStringLiteral("unit")].toString());
    var.setInitialValue(sim::bound(varObj[QStringLiteral("init")].toDouble(),
                        var.type()));
    if( !env.insertVariable(var) ) {
      logError(tr("Unable to insert variable \"%1\"!").arg(var.name()));
    }
  }

  // (5) Modules /////////////////////////////////////////////////////////////

  // Done ////////////////////////////////////////////////////////////////////

  return true;
}

bool SimContext::save(const QString& filename) const
{
  QJsonObject ctxObj;

  // (1) Configuration ///////////////////////////////////////////////////////

  QJsonObject cfgObj;
  cfgObj[QStringLiteral("mode")] = sim::toString(cfg.mode);
  cfgObj[QStringLiteral("step")] = cfg.step;
  cfgObj[QStringLiteral("duration")] = cfg.duration;
  ctxObj[QStringLiteral("configuration")] = cfgObj;

  // (2) Variables ///////////////////////////////////////////////////////////

  QJsonArray varsObj;
  foreach(const SimVariable& var, env.variables()) {
    QJsonObject varObj;
    varObj[QStringLiteral("name")] = var.name();
    varObj[QStringLiteral("type")] = sim::toString(var.type());
    varObj[QStringLiteral("unit")] = var.unit();
    varObj[QStringLiteral("init")] = sim::bound(var.initialValue(), var.type());
    varsObj.append(varObj);
  }
  ctxObj[QStringLiteral("variables")] = varsObj;

  // (3) Modules /////////////////////////////////////////////////////////////

  // (4) Output //////////////////////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QIODevice::WriteOnly) ) {
    return false;
  }

  const QJsonDocument jsonFile(ctxObj);
  file.write(jsonFile.toJson());
  file.close();

  // Done ////////////////////////////////////////////////////////////////////

  return true;
}

////// public slots //////////////////////////////////////////////////////////

void SimContext::logError(const QString& text)
{
  emit errorLogged(text);
}

void SimContext::logError(int lineno, const QString& text)
{
  emit errorLogged(lineno, text);
}

void SimContext::logWarning(const QString& text)
{
  emit warningLogged(text);
}

void SimContext::logWarning(int lineno, const QString& text)
{
  emit warningLogged(lineno, text);
}

void SimContext::logText(const QString& text)
{
  emit textLogged(text);
}

void SimContext::setConfig(const SimConfig& config)
{
  cfg = config;
}
