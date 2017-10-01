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

#include <QtCore/QCoreApplication>

#include "SimCore/SimModuleRunner.h"

#include "SimCore/SimContext.h"
#include "SimCore/SimDataTypeId.h"
#include "SimCore/SimModule.h"
#include "SimCore/SimUtil.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  template<typename T>
  SimVariableXferRef use_impl(const QString& name,
                              T *pointer,
                              const uint32_t direction,
                              SimModuleRunner *runner)
  {
    SimContext *ctx = qobject_cast<SimContext*>(runner->parent());

    if( ctx->sim.activeState() != InitState ) {
      ctx->logError(QCoreApplication::translate("SimModuleRunner",
                                                "%1: sim_use() can only be called from state INIT!")
                    .arg(runner->moduleName()));
      return SimVariableXferRef();
    }

    if( runner->isXfer(pointer) ) {
      ctx->logError(QCoreApplication::translate("SimModuleRunner",
                                                "%1: Named address \"%2\" is already in use!")
                    .arg(runner->moduleName())
                    .arg(name));
      return SimVariableXferRef();
    }

    SimVariableXferRef xfer = ctx->db.use(name, pointer, direction);
    if( xfer.isNull() ) {
      ctx->logError(QCoreApplication::translate("SimModuleRunner",
                                                "%1: Reference to unknown variable \"%2\" of type %3!")
                    .arg(runner->moduleName())
                    .arg(name)
                    .arg(sim::toString(SimDataTypeId<T>::id()).toUpper()));
      return SimVariableXferRef();
    }

    return xfer;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

SimModuleRunner::~SimModuleRunner()
{
  if( _lib.isLoaded() ) {
    _lib.unload();
  }
}

QString SimModuleRunner::moduleName() const
{
  return SimModule::moduleName(_lib.fileName());
}

bool SimModuleRunner::isXfer(void *pointer) const
{
  return _xfers.contains(pointer);
}

void SimModuleRunner::syncInits()
{
  foreach(const SimVariableXferRef& xfer, _xfers.values()) {
    xfer->syncInit();
  }
}

void SimModuleRunner::syncInputs()
{
  foreach(const SimVariableXferRef& xfer, _xfers.values()) {
    xfer->syncInput();
  }
}

void SimModuleRunner::syncOutputs()
{
  foreach(const SimVariableXferRef& xfer, _xfers.values()) {
    xfer->syncOutput();
  }
}

void SimModuleRunner::init(int argc, char **argv)
{
  _sim_init(argc, argv, this);
}

void SimModuleRunner::start()
{
  _sim_start(this);
}

void SimModuleRunner::step(double dt)
{
  _sim_step(dt, this);
}

void SimModuleRunner::stop()
{
  _sim_stop(this);
}

void SimModuleRunner::use(const QString& name, double *pointer, const uint32_t direction)
{
  SimVariableXferRef xfer = priv::use_impl<double>(name, pointer, direction, this);
  if( !xfer.isNull() ) {
    _xfers.insert(static_cast<void*>(pointer), xfer);
  }
}

void SimModuleRunner::use(const QString& name, float *pointer, const uint32_t direction)
{
  SimVariableXferRef xfer = priv::use_impl<float>(name, pointer, direction, this);
  if( !xfer.isNull() ) {
    _xfers.insert(static_cast<void*>(pointer), xfer);
  }
}

void SimModuleRunner::use(const QString& name, int32_t *pointer, const uint32_t direction)
{
  SimVariableXferRef xfer = priv::use_impl<int32_t>(name, pointer, direction, this);
  if( !xfer.isNull() ) {
    _xfers.insert(static_cast<void*>(pointer), xfer);
  }
}

void SimModuleRunner::use(const QString& name, uint32_t *pointer, const uint32_t direction)
{
  SimVariableXferRef xfer = priv::use_impl<uint32_t>(name, pointer, direction, this);
  if( !xfer.isNull() ) {
    _xfers.insert(static_cast<void*>(pointer), xfer);
  }
}

void SimModuleRunner::on(void *ptr)
{
  SimVariableXferRef xfer = _xfers.value(ptr);
  if( !xfer.isNull() ) {
    xfer->on();
  }
}

void SimModuleRunner::off(void *ptr)
{
  SimVariableXferRef xfer = _xfers.value(ptr);
  if( !xfer.isNull() ) {
    xfer->off();
  }
}

void SimModuleRunner::print(const uint32_t lvl, const QString& str) const
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  if(        lvl == Error ) {
    ctx->logError(str);
  } else if( lvl == Warning ) {
    ctx->logWarning(str);
  } else {
    ctx->logText(str);
  }
}

QSharedPointer<SimModuleRunner> SimModuleRunner::create(const QString& filename,
                                                        SimContext *simctx)
{
  SimModuleRunnerRef ref(new SimModuleRunner(filename, simctx));
  if( ref.isNull() ) {
    return SimModuleRunnerRef();
  }

  if( ref->_sim_init == 0  ||  ref->_sim_start == 0  ||
      ref->_sim_step == 0  ||  ref->_sim_stop  == 0 ) {
    return SimModuleRunnerRef();
  }

  return ref;
}

////// private ///////////////////////////////////////////////////////////////

SimModuleRunner::SimModuleRunner(const QString& filename, SimContext *simctx)
  : QObject(simctx)
  , _lib(filename)
  , _sim_init(0)
  , _sim_start(0)
  , _sim_step(0)
  , _sim_stop(0)
  , _xfers()
{
  if( _lib.load() ) {
    _sim_init  =  (sim_init_func)_lib.resolve("sim_init");
    _sim_start = (sim_start_func)_lib.resolve("sim_start");
    _sim_step  =  (sim_step_func)_lib.resolve("sim_step");
    _sim_stop  =  (sim_stop_func)_lib.resolve("sim_stop");
  }
}
