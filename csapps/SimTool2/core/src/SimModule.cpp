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

#include <QtCore/QFileInfo>

#include "SimCore/SimModule.h"

#include "SimCore/SimModuleRunner.h"

////// public ////////////////////////////////////////////////////////////////

SimModule::SimModule()
  : _active(true)
  , _arguments()
  , _filename()
{
}

SimModule::~SimModule()
{
}

QString SimModule::name() const
{
  return moduleName(_filename);
}

bool SimModule::isActive() const
{
  return _active;
}

void SimModule::setActive(const bool active)
{
  _active = active;
}

const QString& SimModule::arguments() const
{
  return _arguments;
}

void SimModule::setArguments(const QString& args)
{
  if( isValidArguments(args) ) {
    _arguments = args;
  } else {
    _arguments.clear();
  }
}

const QString& SimModule::fileName() const
{
  return _filename;
}

void SimModule::setFileName(const QString& filename)
{
  _filename = filename;
}

bool SimModule::isValidArguments(const QString& args)
{
  return ( args.count(QChar::fromLatin1('"')) & 1 ) == 0;
}

bool SimModule::isValidModule(const QString& filename)
{
  if( filename.isEmpty()  ||
      SimModuleRunner::create(filename, 0).isNull() ) {
    return false;
  }
  return true;
}

QString SimModule::moduleName(const QString& filename)
{
  return QFileInfo(filename).completeBaseName();
}
