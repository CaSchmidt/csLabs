/****************************************************************************
** Copyright (c) 2015, Carsten Schmidt. All rights reserved.
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

#ifndef __CSLIST_H__
#define __CSLIST_H__

#include <csCore/csListData.h>

template<class T>
class csList {
public:
  csList() throw()
    : _count(0)
    , _data(0)
  {
  }

  csList(const csList<T>& other) throw()
    : _count()
    , _data()
  {
    _data = copy(other._data, other._count, _count);
  }

  ~csList() throw()
  {
    free(_data, _count);
  }

  // Information /////////////////////////////////////////////////////////////

  inline int count() const throw()
  {
    return (int)_count;
  }

  inline bool isEmpty() const throw()
  {
    return _data == 0  ||  _count < 1;
  }

  // Assignment //////////////////////////////////////////////////////////////

  void clear() throw()
  {
    free(_data, _count);
  }

  csList<T>& operator=(const csList<T>& other) throw()
  {
    if( this != &other ) {
      free(_data, _count);
      _data = copy(other._data, other._count, _count);
    }
    return *this;
  }

  // Concatenation ///////////////////////////////////////////////////////////

  csList<T>& operator+=(const csList<T>& other) throw()
  {
    DataPtr *old = _data;
    _data = append(old, _count, other._data, other._count, _count);
    if( old != _data ) {
      free(old);
    }
    return *this;
  }

  csList<T>& operator+=(const T& other) throw()
  {
    DataPtr *old = _data;
    _data = append(old, _count, other, _count);
    if( old != _data ) {
      free(old);
    }
    return *this;
  }

  // Access //////////////////////////////////////////////////////////////////

  inline T& operator[](const int index) throw()
  {
    return _data[index]->value;
  }

  inline const T& operator[](const int index) const throw()
  {
    return _data[index]->value;
  }

private:
  typedef csListData<T>* DataPtr;

  static DataPtr *alloc(size_t count) throw()
  {
    return (DataPtr*)csAlloc(count*sizeof(DataPtr));
  }

  static void free(DataPtr* &data, size_t& count) throw()
  {
    if( data != 0 ) {
      for(size_t i = 0; i < count; i++) {
        delete data[i];
      }
      csFree(data);
    }
    count = 0;
    data  = 0;
  }

  static void free(DataPtr *&data) throw()
  {
    size_t count = 0;
    free(data, count);
  }

  static DataPtr *append(const DataPtr *bas, const size_t basCount,
                         const DataPtr *app, const size_t appCount,
                         size_t& count) throw()
  {
    count = basCount;
    if( bas == 0  ||  app == 0  ||  appCount < 1 ) {
      return (DataPtr*)bas;
    }

    DataPtr *data = alloc(basCount+appCount);
    if( data == 0 ) {
      return (DataPtr*)bas;
    }
    count = basCount + appCount;

    for(size_t i = 0; i < basCount; i++) {
      data[i] = bas[i];
    }
    for(size_t i = 0; i < appCount; i++) {
      data[basCount+i] = 0;
      if( app[i] != 0 ) {
        data[basCount+i] = new csListData<T>(app[i]->value);
      }
    }

    return data;
  }

  static DataPtr *append(const DataPtr *bas, const size_t basCount, const T& value,
                         size_t& count) throw()
  {
    count = basCount;

    DataPtr *data = alloc(basCount+1);
    if( data == 0 ) {
      return (DataPtr*)bas;
    }
    count = basCount + 1;

    for(size_t i = 0; i < basCount; i++) {
      data[i] = bas[i];
    }
    data[basCount] = new csListData<T>(value);

    return data;
  }

  static DataPtr *copy(const DataPtr *other, const size_t otherCount,
                size_t& count) throw()
  {
    count = 0;
    if( other == 0  ||  otherCount < 1 ) {
      return 0;
    }

    DataPtr *data = alloc(otherCount);
    if( data == 0 ) {
      return 0;
    }
    count = otherCount;

    for(size_t i = 0; i < count; i++) {
      data[i] = 0;
      if( other[i] != 0 ) {
        data[i] = new csListData<T>(other[i]->value);
      }
    }

    return data;
  }

  size_t   _count;
  DataPtr *_data;
};

#endif // __CSLIST_H__
