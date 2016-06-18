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

#ifndef __SIMCONTEXT_H__
#define __SIMCONTEXT_H__

#include <QtCore/QObject>

#include <SimCore/simcore_config.h>
#include <SimCore/SimConfig.h>
#include <SimCore/SimControl.h>
#include <SimCore/SimDataLogger.h>
#include <SimCore/SimDb.h>
#include <SimCore/SimEnvironment.h>
#include <SimCore/SimSimulator.h>
#include <SimCore/ISimLogger.h>

class SIM_CORE_EXPORT SimContext : public QObject, public ISimLogger {
  Q_OBJECT
public:
  SimContext(QObject *parent);
  ~SimContext();

  bool open(const QString& filename);
  bool save(const QString& filename) const;

  SimConfig cfg;
  SimControl ctrl;
  SimDb db;
  SimEnvironment env;
  SimDataLogger logger;
  SimSimulator sim;

public slots:
  void logError(const QString& text);
  void logError(int lineno, const QString& text);

  void logWarning(const QString& text);
  void logWarning(int lineno, const QString& text);

  void logText(const QString& text);

  void setConfig(const SimConfig& config);

signals:
  void configChanged(const SimConfig&);

  void errorLogged(const QString& text);
  void errorLogged(int lineno, const QString& text);

  void warningLogged(const QString& text);
  void warningLogged(int lineno, const QString& text);

  void textLogged(const QString& text);
};

#endif // __SIMCONTEXT_H__
