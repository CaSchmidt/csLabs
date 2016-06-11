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

#ifndef __SIMDBSTORE_H__
#define __SIMDBSTORE_H__

#include <QtCore/QHash>
#include <QtCore/QVector>

#include <SimCore/SimDataTypeId.h>
#include <SimCore/SimUtil.h>
#include <SimCore/SimVariable.h>

template<typename T>
class SimDbStore {
public:
  SimDbStore()
    : _index()
    , _table()
  {
  }

  ~SimDbStore()
  {
  }

  void initialize(const SimVariables& vars)
  {
    const SimDataType typeId = SimDataTypeId<T>::id();

    foreach(const SimVariable& var, vars) {
      if( var.type() != typeId ) {
        continue;
      }
      const int dbIndex = index(var.name());
      if( dbIndex < 0 ) {
        continue;
      }
      _table[dbIndex] = sim::bound(var.initialValue(), typeId);
    }
  }

  void clear()
  {
    _index.clear();
    _table.clear();
  }

  inline int index(const QString& name) const
  {
    return _index.value(name, -1);
  }

  bool insert(const QString& name, const T initialValue)
  {
    if( _index.contains(name) ) {
      return true;
    }
    const int pos = _table.size();
    _table.append(initialValue);
    if( pos+1 != _table.size() ) {
      return false;
    }
    _index.insert(name, pos);
    return _index.contains(name);
  }

  bool remove(const QString& name)
  {
    return _index.remove(name) > 0;
  }

  inline T& operator[](const int index)
  {
    return _table[index];
  }

  inline const T& operator[](const int index) const
  {
    return _table[index];
  }

private:
  QHash<QString,int> _index;
  QVector<T> _table;
};

#endif // __SIMDBSTORE_H__
