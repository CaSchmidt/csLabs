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

#include <numeric>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtGui/QColor>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "config.h"

#include "wvalueswindow.h"

QColor loadColor(const QJsonObject& obj)
{
  QColor color;
  if( !obj.isEmpty() ) {
    color.setNamedColor(obj[QStringLiteral("color")].toString());
  }
  return color;
}

void storeColor(QJsonObject& obj, const QColor& color)
{
  obj[QStringLiteral("color")] = color.name(QColor::HexRgb);
}

void loadGeometry(QWidget *widget, const QJsonObject& obj)
{
  if( obj.isEmpty() ) {
    return;
  }

  const int error = std::numeric_limits<int>::lowest();

  const int x = obj[QStringLiteral("x")].toInt(error);
  const int y = obj[QStringLiteral("y")].toInt(error);
  if( x != error  &&  y != error ) {
    widget->move(x, y);
  }

  const int w = obj[QStringLiteral("width")].toInt(error);
  const int h = obj[QStringLiteral("height")].toInt(error);
  if( w != error  &&  h != error ) {
    widget->resize(w, h);
  }
}

QJsonObject storeGeometry(QWidget *widget)
{
  QJsonObject obj;
  obj[QStringLiteral("x")]      = widget->x();
  obj[QStringLiteral("y")]      = widget->y();
  obj[QStringLiteral("width")]  = widget->width();
  obj[QStringLiteral("height")] = widget->height();
  return obj;
}

bool loadConfig(const QString& filename, QMainWindow *mainWindow)
{
  // Load Existing Configuration /////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QFile::ReadOnly) ) {
    return false;
  }

  QJsonParseError error;
  const QJsonDocument cfgDoc = QJsonDocument::fromJson(file.readAll(), &error);
  file.close();
  if( error.error != QJsonParseError::NoError ) {
    return false;
  }

  const QJsonObject cfgObj = cfgDoc.object();
  if( cfgObj.isEmpty() ) {
    return false;
  }

  // WMainWindow /////////////////////////////////////////////////////////////

  const QJsonObject mainObj = cfgObj[QStringLiteral("mainWindow")].toObject();
  if( !mainObj.isEmpty() ) {
    loadGeometry(mainWindow, mainObj[QStringLiteral("geometry")].toObject());
  }

  // WValuesWindow ///////////////////////////////////////////////////////////

  WValuesWindow::loadConfig(cfgObj);

  return true;
}

bool storeConfig(const QString& filename, QMainWindow *mainWindow)
{
  // Load Existing Configuration /////////////////////////////////////////////

  QFile file(filename);
  if( !file.open(QFile::ReadOnly) ) {
    return false;
  }

  QJsonParseError error;
  QJsonDocument cfgDoc = QJsonDocument::fromJson(file.readAll(), &error);
  file.close();
  if( error.error != QJsonParseError::NoError ) {
    return false;
  }

  QJsonObject cfgObj = cfgDoc.object();
  if( cfgObj.isEmpty() ) {
    return false;
  }

  // WMainWindow /////////////////////////////////////////////////////////////

  QJsonObject mainObj;
  mainObj[QStringLiteral("geometry")] = storeGeometry(mainWindow);
  cfgObj[QStringLiteral("mainWindow")] = mainObj;

  // WValuesWindow ///////////////////////////////////////////////////////////

  WValuesWindow::storeConfig(cfgObj);

  // Store New Configuration /////////////////////////////////////////////////

  cfgDoc.setObject(cfgObj);

  if( !file.open(QFile::WriteOnly) ) {
    return false;
  }
  file.write(cfgDoc.toJson());
  file.close();

  return true;
}
