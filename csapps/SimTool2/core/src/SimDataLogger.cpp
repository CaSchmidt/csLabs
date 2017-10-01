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

#include "SimCore/SimDataLogger.h"

#include "SimCore/priv/SimDataSeriesPriv.h"
#include "SimCore/SimContext.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  SimDataSeries createLogTime(const int depth, const double dt)
  {
    SimDataSeries time(new SimDataSeriesPriv());
    if( time.isNull() ) {
      return SimDataSeries();
    }
    if( !time.initialize(depth, 0) ) {
      return SimDataSeries();
    }
    const double T0 = -(double)time.size()*dt;
    for(int i = 0; i < time.size(); i++) {
      time.append(T0 + (double)i*dt);
    }
    return time;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

SimDataLogger::SimDataLogger(SimContext *simctx)
  : QObject(simctx)
  , _logs()
  , _logTime()
  , _mutex()
{
}

SimDataLogger::~SimDataLogger()
{
}

void SimDataLogger::lock()
{
  _mutex.lock();
}

void SimDataLogger::unlock()
{
  _mutex.unlock();
}

bool SimDataLogger::addLog(const QString& name)
{
  QMutexLocker locker(const_cast<QMutex*>(&_mutex));
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  if( _logs.contains(name) ) {
    return true;
  }

  const SimVariable& var = ctx->env.variable(name);
  if( var.isEmpty() ) {
    return false;
  }

  if( _logTime.isNull() ) {
    _logTime = priv::createLogTime(ctx->cfg.logDepth, ctx->cfg.step);
  }
  if( _logTime.isNull() ) {
    return false;
  }

  SimDataLog log(ctx->db.value(name), SimDataSeries(new SimDataSeriesPriv()));
  if( log.first.isNull()   ||  log.second.isNull()  ||
      !log.second.initialize(_logTime.depth(), var.initialValue()) ) {
    if( _logs.isEmpty() ) {
      _logTime.clear();
    }
    return false;
  }
  _logs.insert(name, log);

  return _logs.contains(name);
}

SimDataSeries SimDataLogger::logSeries(const QString& name) const
{
  SimDataLog log = _logs.value(name, SimDataLog());
  return log.second.isNull() ? SimDataSeries() : log.second.copy();
}

SimDataSeries SimDataLogger::logTime() const
{
  return _logTime.isNull() ? SimDataSeries() : _logTime.copy();
}

void SimDataLogger::syncLog(const double time)
{
  QMutexLocker locker(&_mutex);
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  if( _logTime.isNull()  || _logs.isEmpty() ) {
    return;
  }

  _logTime.append(time);

  ctx->db.lock();
  for(SimDataLogIter it = _logs.begin(); it != _logs.end(); it++) {
    it->second.append(it->first->getAsync());
  }
  ctx->db.unlock();
}

////// public slots //////////////////////////////////////////////////////////

void SimDataLogger::clear()
{
  QMutexLocker locker(&_mutex);

  if( !_logTime.isNull() ) {
    _logTime.clear();
  }
  _logs.clear();
}

void SimDataLogger::exitState(int state)
{
  if( state == IdleState ) {
    QMutexLocker locker(&_mutex);
    SimContext *ctx = qobject_cast<SimContext*>(parent());

    if( _logTime.isNull()  ||  _logs.isEmpty() ) {
      return;
    }

    _logTime = priv::createLogTime(ctx->cfg.logDepth, ctx->cfg.step);
    if( _logTime.isNull() ) {
      _logs.clear();
      return;
    }

    foreach(const QString& name, _logs.keys()) {
      const SimVariable& var = ctx->env.variable(name);
      if( var.isEmpty() ) {
        _logs.remove(name);
        continue;
      }

      if( !_logs[name].second.initialize(_logTime.depth(), var.initialValue()) ) {
        _logs.remove(name);
        continue;
      }
    }

    if( _logs.isEmpty() ) {
      _logTime.clear();
    }
  } // IdleState
}

void SimDataLogger::removeLog(const QString& name)
{
  QMutexLocker locker(const_cast<QMutex*>(&_mutex));

  _logs.remove(name);
  if( _logs.isEmpty()  &&  !_logTime.isNull() ) {
    _logTime.clear();
  }
}
