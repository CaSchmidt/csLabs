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

#ifndef __SIMDATALOGGER_H__
#define __SIMDATALOGGER_H__

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPair>

#include <SimCore/simcore_config.h>

#include <SimCore/ISimValue.h>
#include <SimCore/SimDataSeries.h>

class SimContext;

typedef QPair<SimValueRef,SimDataSeries>    SimDataLog;
typedef QHash<QString,SimDataLog>           SimDataLogs;
typedef QHash<QString,SimDataLog>::iterator SimDataLogIter;

class SIM_CORE_EXPORT SimDataLogger : public QObject {
  Q_OBJECT
public:
  SimDataLogger(SimContext *simctx);
  ~SimDataLogger();

  void lock();
  void unlock();

  bool addLog(const QString& name);
  SimDataSeries logSeries(const QString& name) const;
  SimDataSeries logTime() const;
  void syncLog(const double time);

public slots:
  void clear();
  void exitState(int state);
  void removeLog(const QString& name);

private:
  SimDataLogs _logs;
  SimDataSeries _logTime;
  QMutex _mutex;
};

#endif // __SIMDATALOGGER_H__
