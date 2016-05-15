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

#ifndef __ISIMVALUE_H__
#define __ISIMVALUE_H__

#include <QtCore/QSharedPointer>

#include <SimCore/Sim.h>

class SimDb;

class ISimValue {
public:
  virtual ~ISimValue();

  const QString& name() const;

  void invalidate();
  bool isValid() const;

  int precision() const;
  void setPrecision(int precision);

  QString display() const;

  virtual void get() = 0;
  virtual void getNonBlocking() = 0;
  virtual void set() const = 0;

  virtual double value() const = 0;
  virtual void setValue(const double value) = 0;

  virtual double minValue() const = 0;
  virtual double maxValue() const = 0;

  virtual SimDataType type() const = 0;

protected:
  ISimValue(const QString& name, SimDb *db, const int dbIndex);

  SimDb *_db;
  int _dbIndex;
  QString _name;
  int _precision;

private:
  ISimValue(const ISimValue&);
  ISimValue& operator=(const ISimValue&);
};

typedef QSharedPointer<ISimValue> SimValueRef;

#endif // __ISIMVALUE_H__
