/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QFile>
#include <QtCore/QProcess>

#include <csQt/csQtUtil.h>

#include "job.h"

////// Public ////////////////////////////////////////////////////////////////

QByteArray executeJob(const Job& job)
{
  QProcess sox;
  QProcess lame;

  // (1) SoX /////////////////////////////////////////////////////////////////

  sox.setProgram(job.soxExe);

  // Command Line: sox <input1> ... <inputN> -t wav -
#if defined(Q_OS_WIN)
  QString sox_args;
  foreach(const QString& input, job.inputFiles) {
    sox_args += _L1C('"');
    sox_args += input;
    sox_args += _L1C('"');
    sox_args += _L1C(' ');
  }
  sox_args += _L1("-t wav -"); // NOTE: Trailing <SPACE> Above!
  sox.setNativeArguments(sox_args);
#else
  QStringList sox_args;
  sox_args << job.inputFiles << _L1("-t") << _L1("wav") << _L1("-");
  sox.setArguments(sox_args);
#endif

  // (2) LAME ////////////////////////////////////////////////////////////////

  lame.setProgram(job.lameExe);

  // Command Line: lame <options> -S - <output>
#if defined(Q_OS_WIN)
  QString lame_args;
  lame_args += job.lameOptions;
  lame_args += _L1(" -S - ");
  lame_args += _L1C('"');
  lame_args += job.outputFile;
  lame_args += _L1C('"');
  lame.setNativeArguments(lame_args);
#else
  QStringList lame_args;
  lame_args << job.lameOptions << _L1("-S") << _L1("-") << job.outputFile;
  lame.setArguments(lame_args);
#endif

  // (3) Execute /////////////////////////////////////////////////////////////

  sox.setStandardOutputProcess(&lame);

  sox.start();
  lame.start();

  sox.waitForFinished(-1);
  lame.waitForFinished(-1);

  QByteArray result;
  result += sox.readAllStandardError();
  result += '\n';
  result += lame.readAllStandardError();

  if( job.renameInput ) {
    foreach(const QString& input, job.inputFiles) {
      QFile::rename(input, input+_L1(".done"));
    }
  }

  return result;
}
