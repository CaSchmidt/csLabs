/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
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

#include "gui/wlogger.h"

////// public ////////////////////////////////////////////////////////////////

WLogger::WLogger(QWidget *parent)
  : QTextBrowser(parent)
  , csILogger()
{
}

WLogger::~WLogger()
{
}

void WLogger::logText(const char *s) const
{
  WLogger *l = const_cast<WLogger*>(this);
  l->addText(QString::fromUtf8(s));
}

void WLogger::logText(const std::string& s) const
{
  logText(s.data());
}

void WLogger::logWarning(const char *s) const
{
  WLogger *l = const_cast<WLogger*>(this);
  l->addWarning(QString::fromUtf8(s));
}

void WLogger::logWarning(const std::string& s) const
{
  logWarning(s.data());
}

void WLogger::logWarning(const int no, const char *s) const
{
  WLogger *l = const_cast<WLogger*>(this);
  l->addWarning(no, QString::fromUtf8(s));
}

void WLogger::logWarning(const int no, const std::string& s) const
{
  logWarning(no, s.data());
}

void WLogger::logError(const char *s) const
{
  WLogger *l = const_cast<WLogger*>(this);
  l->addError(QString::fromUtf8(s));
}

void WLogger::logError(const std::string& s) const
{
  logError(s.data());
}

void WLogger::logError(const int no, const char *s) const
{
  WLogger *l = const_cast<WLogger*>(this);
  l->addError(no, QString::fromUtf8(s));
}

void WLogger::logError(const int no, const std::string& s) const
{
  logError(no, s.data());
}

////// private ///////////////////////////////////////////////////////////////

void WLogger::addError(const QString& s)
{
  setTextColor(Qt::red);
  append(QString("ERROR: %1").arg(s));
}

void WLogger::addError(const int no, const QString& s)
{
  setTextColor(Qt::red);
  append(QString("ERROR:%1: %2").arg(no).arg(s));
}

void WLogger::addText(const QString& s)
{
  setTextColor(Qt::black);
  append(s);
}

void WLogger::addWarning(const QString& s)
{
  setTextColor(Qt::blue);
  append(QString("WARNING: %1").arg(s));
}

void WLogger::addWarning(const int no, const QString& s)
{
  setTextColor(Qt::blue);
  append(QString("WARNING:%1: %2").arg(no).arg(s));
}
