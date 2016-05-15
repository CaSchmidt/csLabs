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

#ifndef __SIMMODULERUNNER_H__
#define __SIMMODULERUNNER_H__

#include <QtCore/QHash>
#include <QtCore/QLibrary>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

#include <SimCore/sim_mod.h>
#include <SimCore/ISimVariableXfer.h>

class SimContext;

class SimModuleRunner : public QObject {
  Q_OBJECT
public:
  ~SimModuleRunner();

  QString moduleName() const;

  bool isXfer(void *pointer) const;

  void syncInits();
  void syncInputs();
  void syncOutputs();

  // Begin sim_mod.h Interface ///////////////////////////////////////////////

  void init(int argc, char **argv);
  void start();
  void step(double dt);
  void stop();

  void use(const QString& name, double *pointer, const uint32_t direction);
  void use(const QString& name, float *pointer, const uint32_t direction);
  void use(const QString& name, int32_t *pointer, const uint32_t direction);
  void use(const QString& name, uint32_t *pointer, const uint32_t direction);

  void on(void *ptr);
  void off(void *ptr);

  void print(const uint32_t lvl, const QString& str) const;

  // End sim_mod.h Interface /////////////////////////////////////////////////

  static QSharedPointer<SimModuleRunner> create(const QString& filename,
                                                SimContext *simctx);

private:
  SimModuleRunner(const QString& filename, SimContext *simctx);

  SimModuleRunner(const SimModuleRunner&);
  SimModuleRunner& operator=(const SimModuleRunner&);

  QLibrary _lib;
  sim_init_func  _sim_init;
  sim_start_func _sim_start;
  sim_step_func  _sim_step;
  sim_stop_func  _sim_stop;
  QHash<void*,SimVariableXferRef> _xfers;
};

typedef QSharedPointer<SimModuleRunner> SimModuleRunnerRef;

#endif // __SIMMODULERUNNER_H__
