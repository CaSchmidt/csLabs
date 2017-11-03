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

#include "SimCore/SimVariable.h"

SimVariable::SimVariable()
  : _initialValue()
  , _name()
  , _type(InvalidDataType)
  , _unit()
{
}

SimVariable::~SimVariable()
{
}

bool SimVariable::isEmpty() const
{
  return _name.isEmpty()  ||
      (_type != DoubleType  &&  _type != SingleType  &&
      _type != IntType  &&  _type != UIntType);
}

double SimVariable::initialValue() const
{
  return _initialValue;
}

void SimVariable::setInitialValue(const double iv)
{
  _initialValue = iv;
}

const QString& SimVariable::name() const
{
  return _name;
}

void SimVariable::setName(const QString& n)
{
  _name = n;
}

SimDataType SimVariable::type() const
{
  return _type;
}

void SimVariable::setType(const SimDataType t)
{
  _type = t;
}

const QString& SimVariable::unit() const
{
  return _unit;
}

void SimVariable::setUnit(const QString& u)
{
  _unit = u;
}
