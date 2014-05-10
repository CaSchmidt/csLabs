/****************************************************************************
** Copyright (c) 2005-2014, Carsten Schmidt. All rights reserved.
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

#ifndef __PROGRESSWATCHER_HPP__
#define __PROGRESSWATCHER_HPP__

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QFutureWatcher>
#include <QtWidgets/QProgressDialog>

template<class Sequence, class MapFunction>
void executeProgressWatcher(QWidget *parent, const QString& title,
                            Sequence& sequence,
                            MapFunction function)
{
  QProgressDialog      progress(parent);
  {
    QString t = title;
    const int num = QThread::idealThreadCount();
    if( num > 1 ) {
      t.append(QObject::tr(" (%1 Threads)").arg(num));
    }
    progress.setWindowTitle(t);
  }
  QFutureWatcher<void> watcher;

  QObject::connect(&watcher,  SIGNAL(finished()),
                   &progress, SLOT(reset()));
  QObject::connect(&watcher,  SIGNAL(progressRangeChanged(int, int)),
                   &progress, SLOT(setRange(int, int)));
  QObject::connect(&watcher,  SIGNAL(progressValueChanged(int)),
                   &progress, SLOT(setValue(int)));
  QObject::connect(&progress, SIGNAL(canceled()),
                   &watcher,  SLOT(cancel()));

  watcher.setFuture(QtConcurrent::map(sequence, function));
  progress.exec();
  watcher.waitForFinished();
}

#endif // __PROGRESSWATCHER_HPP__
