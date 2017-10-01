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

#include <QtCore/QByteArray>
#include <QtCore/QStringList>

#include "SimCore/SimSimulator.h"

#include "SimCore/SimContext.h"
#include "SimCore/SimModule.h"

////// Macros ////////////////////////////////////////////////////////////////

#define CONFIG_OFFLINE_BLOCK  10

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  QByteArray parseArguments(int& argc, const QString& argv0, const QString& argStr)
  {
    argc = 0;
    if( argv0.isEmpty()  ||  !SimModule::isValidArguments(argStr) ) {
      return QByteArray();
    }

    // (1) Parse Argument String into StringList /////////////////////////////

    QStringList args;
    {
      QString arg;
      bool quote = false;

      for(int i = 0; i < argStr.size(); i++) {
        const QChar ch = argStr[i];

        if( ch == QChar::fromLatin1('"') ) {
          quote = quote  ?  false : true;
        } else if( ch.isSpace()  &&  quote ) {
          arg.push_back(ch);
        } else if( ch.isSpace() ) {
          if( !arg.isEmpty() ) {
            args.push_back(arg);
            arg.clear();
          }
        } else {
          arg.push_back(ch);
        }
      } // foreach character

      if( !arg.isEmpty() ) {
        args.push_back(arg);
        arg.clear();
      }
    } // args

    // (2) Argument Count ////////////////////////////////////////////////////

    argc = args.size()+1;

    // (3) Construct **argv //////////////////////////////////////////////////

    // (3.1) Size ////////////////////////////////////////////////////////////

    int size;
    {
      size  = (argc+1)*sizeof(char*);
      size += argv0.toUtf8().size()+1;
      for(int i = 0; i < args.size(); i++) {
        size += args[i].toUtf8().size()+1;
      }
    }

    // (3.2) Data ////////////////////////////////////////////////////////////

    QByteArray result(size, 0);
    {
      char **argv = (char**)result.data();
      char  *data = &result.data()[(argc+1)*sizeof(char*)];

      *argv++ = data;
      qstrcpy(data, argv0.toUtf8().constData());
      data += argv0.toUtf8().size()+1;

      for(int i = 0; i < args.size(); i++) {
        *argv++ = data;
        qstrcpy(data, args[i].toUtf8().constData());
        data += args[i].toUtf8().size()+1;
      }
    }

    // Done! /////////////////////////////////////////////////////////////////

    return result;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

SimSimulator::SimSimulator(SimContext *simctx)
  : QObject(simctx)
  , _logTimeStamp()
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
  if( state == StepState  &&  ctx->cfg.mode() == RealTimeMode ) {
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
    int argc;
    QByteArray argv = priv::parseArguments(argc, mod.fileName(), mod.arguments());
    runner->init(argc, (char**)argv.data());
  }
  ctx->db.unlock();

  // (3) Simulation //////////////////////////////////////////////////////////

  _offCntStep = _offNumSteps = _rtTimerId = 0;

  if( ctx->cfg.mode() == OfflineMode ) {
    _offNumSteps = ctx->cfg.durationS()*1000/ctx->cfg.stepMS();
    _offCntStep = 0;
  }
}

void SimSimulator::pause()
{
}

void SimSimulator::start()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  syncInits();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->start();
  }
  syncOutputs();

  _logTimeStamp = 0;
  ctx->logger.syncLog(_logTimeStamp);
}

void SimSimulator::step()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  if(        ctx->cfg.mode() == OfflineMode ) {
    QMetaObject::invokeMethod(this, "offlineStep", Qt::QueuedConnection);
  } else if( ctx->cfg.mode() == RealTimeMode ) {
    _rtTimer.start(ctx->cfg.stepMS(), this);
    _rtTimerId = _rtTimer.timerId();
  }
}

void SimSimulator::stepModules()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  syncInputs();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->step(ctx->cfg.step());
  }
  syncOutputs();

  _logTimeStamp += ctx->cfg.step();
  ctx->logger.syncLog(_logTimeStamp);
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
}

void SimSimulator::syncInits()
{
  SimContext *ctx = qobject_cast<SimContext*>(parent());
  ctx->db.lock();
  foreach(const SimModuleRunnerRef& runner, _runners) {
    runner->syncInits();
  }
  ctx->db.unlock();
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
