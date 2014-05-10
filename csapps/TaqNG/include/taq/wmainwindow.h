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

#ifndef __WMAINWINDOW_H__
#define __WMAINWINDOW_H__

#include <QtCore/QList>
#include <QtCore/QVector>

#include <taglib/mpegfile.h>

#include "ui_wmainwindow.h"

#include "taq/mytag.h"
#include "taq/myframesmodel.h"

class WMainWindow : public QMainWindow {
  Q_OBJECT
public:
  WMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
  ~WMainWindow();

  MyTag currentTag();

private:
  Ui::WMainWindow ui;

  // wmainwindow_tag.cpp
  void tag_clear();
  void tag_setMaximum(int max);
  void tag_setEditable(QLineEdit& e, const bool b);
  void tag_setRegExp(QLineEdit& e, const QSpinBox& s);

  // wmainwindow_single.cpp
  void single_clear();
  void single_setTag(const MyTag& mytag);
  MyTag single_getTag();

  // wmainwindow_id3v2.cpp
  void id3v2_clear();

  QString configFileName() const;
  TagLib::MPEG::File *browseMP3File();
  void setCurrentTag(const MyTag& tag);
  void saveFieldToSettings(class QSettings& s,
			   const QString& expr,
			   const QString& key,
			   const int cap, const QString& str);
  bool loadFieldFromSettings(int& cap, QString& str,
			     const QString& expr,
			     const QString& key,
			     const class QSettings& s);

  QList<MyTag> tag_expressions;
  MyTagModel *tag_expressionsmodel;
  QVector<MyFrame> id3v2_myframes;
  MyFramesModel *id3v2_myframesmodel;

protected:
  bool eventFilter(QObject *watched, QEvent *event);

private slots:
  // wmainwindow_tag.cpp
  void tag_changeState(const QString& s);
  void tag_selectExpression(const QModelIndex& index);
  void tag_add();
  void tag_remove();
  // wmainwindow_single.cpp
  void single_browse();
  void single_save();
  // wmainwindow_multi.cpp
  void multi_browse();
  void multi_go();
  // wmainwindow_strip.cpp
  void strip_browse();
  void strip_go();
  // wmainwindow_apic.cpp
  void apic_browsePicture();
  void apic_browse();
  void apic_go();
  // wmainwindow_id3v2.cpp
  void id3v2_browse();
};

#endif // __WMAINWINDOW_H__
