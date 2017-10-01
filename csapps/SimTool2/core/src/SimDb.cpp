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

#include <limits>

#include <QtCore/QMutexLocker>

#include "SimCore/SimDb.h"

#include "SimCore/SimContext.h"
#include "SimCore/SimDataTypeId.h"
#include "SimCore/sim_mod.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  template<typename T>
  class SimValue : public ISimValue {
  public:
    SimValue(const QString& name, const int index, QMutex *mutex,
             SimDbStore<T> *store)
      : ISimValue(name, index, mutex)
      , _store(store)
      , _value()
    {
    }

    ~SimValue()
    {
    }

    double get()
    {
      _mutex->lock();
      _value = _store->operator[](_index);
      _mutex->unlock();
      return _value;
    }

    double getAsync()
    {
      _value = _store->operator[](_index);
      return _value;
    }

    void set() const
    {
      _mutex->lock();
      _store->operator[](_index) = _value;
      _mutex->unlock();
    }

    double value() const
    {
      return _value;
    }

    void setValue(const double value)
    {
      _value = sim::bound(value, type());
    }

    double minValue() const
    {
      return std::numeric_limits<T>::lowest();
    }

    double maxValue() const
    {
      return std::numeric_limits<T>::max();
    }

    SimDataType type() const
    {
      return SimDataTypeId<T>::id();
    }

  private:
    SimDbStore<T> *_store;
    T _value;
  };

  template<typename T>
  class SimVariableXfer : public ISimVariableXfer {
  public:
    SimVariableXfer(T *pointer, SimDbStore<T> *store, const uint32_t dir, const int index)
      : ISimVariableXfer(dir, index)
      , _pointer(pointer)
      , _store(store)
    {
    }

    ~SimVariableXfer()
    {
    }

    void syncInit()
    {
      *_pointer = _store->operator[](_index);
    }

    void syncInput()
    {
      if( _dir & Input ) {
        *_pointer = _store->operator[](_index);
      }
    }

    void syncOutput()
    {
      if( _dir & _outputMask ) {
        _store->operator[](_index) = *_pointer;
      }
    }

    SimDataType type() const
    {
      return SimDataTypeId<T>::id();
    }

  private:
    T *_pointer;
    SimDbStore<T> *_store;
  };

  template<typename T>
  inline SimVariableXferRef use_impl(const QString& name,
                                     T *pointer,
                                     const uint32_t direction,
                                     SimDbStore<T> *store)
  {
    const int dbIndex = store->index(name);
    if( dbIndex < 0 ) {
      return SimVariableXferRef();
    }
    return SimVariableXferRef(new SimVariableXfer<T>(pointer, store, direction, dbIndex));
  }

  template<typename T>
  inline SimValueRef value_impl(const QString& name,
                                QMutex *mutex,
                                SimDbStore<T> *store)
  {
    const int index = store->index(name);
    if( index < 0 ) {
      return SimValueRef();
    }
    return SimValueRef(new SimValue<T>(name, index, mutex, store));
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

SimDb::SimDb(SimContext *simctx)
  : QObject(simctx)
  , _doubleStore()
  , _singleStore()
  , _intStore()
  , _uintStore()
  , _mutex()
{
}

SimDb::~SimDb()
{
}

void SimDb::initialize()
{
  QMutexLocker locker(&_mutex);
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  _doubleStore.initialize(ctx->env.variables());
  _singleStore.initialize(ctx->env.variables());
  _intStore.initialize(ctx->env.variables());
  _uintStore.initialize(ctx->env.variables());
}

void SimDb::lock()
{
  _mutex.lock();
}

void SimDb::unlock()
{
  _mutex.unlock();
}

SimVariableXferRef SimDb::use(const QString& name, double *pointer,
                              const uint32_t direction) const
{
  return priv::use_impl<double>(name, pointer, direction,
                                const_cast<SimDbStore<double>*>(&_doubleStore));
}

SimVariableXferRef SimDb::use(const QString& name, float *pointer,
                              const uint32_t direction) const
{
  return priv::use_impl<float>(name, pointer, direction,
                               const_cast<SimDbStore<float>*>(&_singleStore));
}

SimVariableXferRef SimDb::use(const QString& name, int32_t *pointer,
                              const uint32_t direction) const
{
  return priv::use_impl<int32_t>(name, pointer, direction,
                                 const_cast<SimDbStore<int32_t>*>(&_intStore));
}

SimVariableXferRef SimDb::use(const QString& name, uint32_t *pointer,
                              const uint32_t direction) const
{
  return priv::use_impl<uint32_t>(name, pointer, direction,
                                  const_cast<SimDbStore<uint32_t>*>(&_uintStore));
}

SimValueRef SimDb::value(const QString& name) const
{
  QMutexLocker locker(const_cast<QMutex*>(&_mutex));
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  const SimVariable& var = ctx->env.variable(name);
  if(        var.type() == DoubleType ) {
    return priv::value_impl<double>(name, const_cast<QMutex*>(&_mutex),
                                    const_cast<SimDbStore<double>*>(&_doubleStore));
  } else if( var.type() == SingleType ) {
    return priv::value_impl<float>(name, const_cast<QMutex*>(&_mutex),
                                   const_cast<SimDbStore<float>*>(&_singleStore));
  } else if( var.type() == IntType ) {
    return priv::value_impl<int32_t>(name, const_cast<QMutex*>(&_mutex),
                                     const_cast<SimDbStore<int32_t>*>(&_intStore));
  } else if( var.type() == UIntType ) {
    return priv::value_impl<uint32_t>(name, const_cast<QMutex*>(&_mutex),
                                      const_cast<SimDbStore<uint32_t>*>(&_uintStore));
  }
  return SimValueRef();
}

////// public slots //////////////////////////////////////////////////////////

void SimDb::clear()
{
  QMutexLocker locker(&_mutex);

  _doubleStore.clear();
  _singleStore.clear();
  _intStore.clear();
  _uintStore.clear();
}

void SimDb::insertVariable(const QString& name)
{
  QMutexLocker locker(&_mutex);
  SimContext *ctx = qobject_cast<SimContext*>(parent());

  const SimVariable& var = ctx->env.variable(name);
  if(        var.type() == DoubleType ) {
    _doubleStore.insert(name, sim::bound(var.initialValue(), var.type()));
  } else if( var.type() == SingleType ) {
    _singleStore.insert(name, sim::bound(var.initialValue(), var.type()));
  } else if( var.type() == IntType ) {
    _intStore.insert(name, sim::bound(var.initialValue(), var.type()));
  } else if( var.type() == UIntType ) {
    _uintStore.insert(name, sim::bound(var.initialValue(), var.type()));
  }
}

void SimDb::removeVariable(const QString& name)
{
  QMutexLocker locker(&_mutex);

  _doubleStore.remove(name);
  _singleStore.remove(name);
  _intStore.remove(name);
  _uintStore.remove(name);
}
