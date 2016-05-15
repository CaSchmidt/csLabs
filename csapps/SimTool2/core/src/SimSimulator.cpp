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

#include "SimCore/SimSimulator.h"

#include "SimCore/SimContext.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CONFIG_OFFLINE_BLOCK  10

////// public ////////////////////////////////////////////////////////////////

SimSimulator::SimSimulator(SimContext *simctx)
  : QObject(simctx)
  , _offCntStep()
  , _offNumSteps()
  , _rtTimer()
  , _rtTimerId(0) // cf. qtbase/src/corelib/kernel/qbasictimer.cpp
  , _runners()
  , _state(InvalidOperationState)
{
}

SimSimulator::~SimSimulator()
{
}

SimOperationState SimSimulator::activeState() const
{
  return _state;
}

////// public slots //////////////////////////////////////////////////////////

void SimSimulator::enterState(int state)
{
  if(        state == IdleState ) {
    _state = IdleState;
    idle();
  } else if( state == InitState ) {
    _state = InitState;
    init();
  } else if( state == PauseState ) {
    _state = PauseState;
    pause();
  } else if( state == StartState ) {
    _state = StartState;
    start();
  } else if( state == StepState ) {
    _state = StepState;
    step();
  } else if( state == StopState ) {
    _state = StopState;
    stop();
  }
}

void SimSimulator::exitState(int state)
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());
  if( state == StepState  &&  ctx->cfg.mode == RealTimeMode ) {
    _rtTimer.stop();
    _rtTimerId = 0;
  }
}

////// protected /////////////////////////////////////////////////////////////

void SimSimulator::timerEvent(QTimerEvent *event)
{
  if( _state == StepState  &&  event->timerId() == _rtTimerId ) {
    stepModules();
  }
  event->accept();
}

////// private slots /////////////////////////////////////////////////////////

void SimSimulator::offlineStep()
{
  if( _state != StepState ) {
    return;
  }
  for(int i = 0; i < CONFIG_OFFLINE_BLOCK  &&  _offCntStep < _offNumSteps;
      i++, _offCntStep++) {
    stepModules();
  }
  if( _offCntStep == _offNumSteps ) {
    emit stopRequested();
  } else {
    QMetaObject::invokeMethod(this, "offlineStep", Qt::QueuedConnection);
  }
}

////// private ///////////////////////////////////////////////////////////////

void SimSimulator::idle()
{
  printf("%s: idle\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::init()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  // (1) DB //////////////////////////////////////////////////////////////////

  ctx->db.initialize();

  // (2) Modules /////////////////////////////////////////////////////////////

  ctx->db.lock();
  foreach(const SimModule& mod, ctx->env.modules()) {
    // (2.1) Ignore Inactive Modules /////////////////////////////////////////
    if( !mod.isActive() ) {
      continue;
    }
    // (2.2) Load Module /////////////////////////////////////////////////////
    SimModuleRunnerRef runner = SimModuleRunner::create(mod.fileName(), ctx);
    if( runner.isNull() ) {
      ctx->logError(tr("Unable to load module \"%1\"!")
                    .arg(mod.name()));
      continue;
    }
    // (2.3) Store Module ////////////////////////////////////////////////////
    _runners.push_back(runner);
    // (2.4) Initialize Module ///////////////////////////////////////////////
    runner->init(0, 0); // TODO: arguments
  }
  ctx->db.unlock();

  // (3) Simulation //////////////////////////////////////////////////////////

  _offCntStep = _offNumSteps = _rtTimerId = 0;

  if( ctx->cfg.mode == OfflineMode ) {
    _offNumSteps = (int)(ctx->cfg.duration/ctx->cfg.step) + 1;
    _offCntStep = 0;
  }

  printf("%s: init\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::pause()
{
  printf("%s: pause\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::start()
{
  syncInputs();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->start();
  }
  syncOutputs();

  printf("%s: start\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::step()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  if(        ctx->cfg.mode == OfflineMode ) {
    QMetaObject::invokeMethod(this, "offlineStep", Qt::QueuedConnection);
  } else if( ctx->cfg.mode == RealTimeMode ) {
    _rtTimer.start(ctx->cfg.step*1000.0, this);
    _rtTimerId = _rtTimer.timerId();
  }

  printf("%s: step\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::stepModules()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());
  syncInputs();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->step(ctx->cfg.step);
  }
  syncOutputs();
}

void SimSimulator::stop()
{
  // (1) Stop Modules ////////////////////////////////////////////////////////

  syncInputs();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->stop();
  }
  syncOutputs();

  // (2) Release Modules /////////////////////////////////////////////////////

  _runners.clear();

  printf("%s: stop\n", __FUNCSIG__); fflush(stdout); // TODO
}

void SimSimulator::syncInputs()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());
  ctx->db.lock();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->syncInputs();
  }
  ctx->db.unlock();
}

void SimSimulator::syncOutputs()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());
  ctx->db.lock();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->syncOutputs();
  }
  ctx->db.unlock();
}
