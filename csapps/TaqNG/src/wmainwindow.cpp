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

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QHelpEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <taglib/attachedpictureframe.h>
#include <taglib/audioproperties.h>
#include <taglib/id3v2tag.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>

#include <csQt/csImageTip.h>

#include "taq/wmainwindow.h"

#include "taq/mp3filesmodel.h"

////// public /////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags), tag_expressionsmodel(0), id3v2_myframesmodel(0)
{
  ui.setupUi(this);

  // Main Menu ////////////////////////////////////////////////////////////////

  QObject::connect(ui.quitAction, SIGNAL(triggered()),
		   this, SLOT(close()));

  // Tag Scanner //////////////////////////////////////////////////////////////

  tag_expressionsmodel = new MyTagModel(tag_expressions,
					ui.tag_expressionList);
  tag_expressionsmodel->setObjectName("WMainWindow::tag_expressionsmodel");
  ui.tag_expressionList->setModel(tag_expressionsmodel);

  tag_clear();

  ui.tag_yearEdit->setValidator(new QIntValidator(0, 9999, ui.tag_yearEdit));
  ui.tag_trackEdit->setValidator(new QIntValidator(0, 99, ui.tag_trackEdit));
  ui.tag_discEdit->setValidator(new QIntValidator(0, 99, ui.tag_discEdit));

  QObject::connect(ui.tag_expressionEdit, SIGNAL(textChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));

  QObject::connect(ui.tag_titleSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_albumSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_artistSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_genreSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_yearSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_commentSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_trackSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_discSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_composerSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));
  QObject::connect(ui.tag_conductorSpin, SIGNAL(valueChanged(const QString&)),
		   this, SLOT(tag_changeState(const QString&)));

  QObject::connect(ui.tag_expressionList,
		   SIGNAL(activated(const QModelIndex&)),
		   this,
		   SLOT(tag_selectExpression(const QModelIndex&)));

  QObject::connect(ui.tag_addButton, SIGNAL(clicked()),
		   this, SLOT(tag_add()));
  QObject::connect(ui.tag_removeButton, SIGNAL(clicked()),
		   this, SLOT(tag_remove()));

  // Single File Mode /////////////////////////////////////////////////////////

  QObject::connect(ui.single_browseButton, SIGNAL(clicked()),
		   this, SLOT(single_browse()));
  QObject::connect(ui.single_saveButton, SIGNAL(clicked()),
		   this, SLOT(single_save()));

  // Multiple Files Mode //////////////////////////////////////////////////////

  ui.multi_filesView->setModel(new Mp3FilesModel(QStringList(), true, this));

  QObject::connect(ui.multi_browseButton, SIGNAL(clicked()),
		   this, SLOT(multi_browse()));
  QObject::connect(ui.multi_goButton, SIGNAL(clicked()),
		   this, SLOT(multi_go()));

  // Strip Frames /////////////////////////////////////////////////////////////

  ui.strip_filesView->setModel(new Mp3FilesModel(QStringList(), false, this));

  QObject::connect(ui.strip_browseButton, SIGNAL(clicked()),
		   this, SLOT(strip_browse()));
  QObject::connect(ui.strip_goButton, SIGNAL(clicked()),
		   this, SLOT(strip_go()));

  // Attach Picture ///////////////////////////////////////////////////////////

  ui.apic_filesView->setModel(new Mp3FilesModel(QStringList(), false, this));

  connect(ui.apic_browsePictureButton, SIGNAL(clicked()),
	  this, SLOT(apic_browsePicture()));
  connect(ui.apic_browseButton, SIGNAL(clicked()),
	  this, SLOT(apic_browse()));
  connect(ui.apic_goButton, SIGNAL(clicked()),
	  this, SLOT(apic_go()));

  {
    QStringList types;
    types                      <<
      tr("Other")              << // 0x00
      tr("FileIcon")           << // 0x01
      tr("OtherFileIcon")      << // 0x02
      tr("FrontCover")         << // 0x03
      tr("BackCover")          << // 0x04
      tr("LeafletPage")        << // 0x05
      tr("Media")              << // 0x06
      tr("LeadArtist")         << // 0x07
      tr("Artist")             << // 0x08
      tr("Conductor")          << // 0x09
      tr("Band")               << // 0x0A
      tr("Composer")           << // 0x0B
      tr("Lyricist")           << // 0x0C
      tr("RecordingLocation")  << // 0x0D
      tr("DuringRecording")    << // 0x0E
      tr("DuringPerformance")  << // 0x0F
      tr("MovieScreenCapture") << // 0x10
      tr("ColouredFish")       << // 0x11
      tr("Illustration")       << // 0x12
      tr("BandLogo")           << // 0x13
      tr("PublisherLogo");        // 0x14

    ui.apic_typeCombo->addItems(types);

    ui.apic_typeCombo->setCurrentIndex(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
  }



  // ID3v2 Frames /////////////////////////////////////////////////////////////

  id3v2_myframesmodel = new MyFramesModel(id3v2_myframes,
					  ui.id3v2_framesTable);
  id3v2_myframesmodel->setObjectName("WMainWindow::id3v2_myframesmodel");
  ui.id3v2_framesTable->setModel(id3v2_myframesmodel);

  id3v2_clear();

  QObject::connect(ui.id3v2_browseButton, SIGNAL(clicked()),
		   this, SLOT(id3v2_browse()));

  // Load Settings ////////////////////////////////////////////////////////////

  QSettings settings(configFileName(), QSettings::IniFormat, this);
  settings.setFallbacksEnabled(false);

  int i(0);
  while( settings.contains(QString("expression%1/name").arg(i)) )
    {
      MyTag tag;

      tag.name = settings.value(QString("expression%1/name").arg(i)).toString();
      tag.regexp.setPattern(settings.value(QString("expression%1/regexp").arg(i)).toString());

#define LOAD_FIELD(field) \
      loadFieldFromSettings(tag.cap_##field, \
			    tag.str_##field, \
			    QString("expression%1").arg(i), \
                            #field, \
			    settings);

      LOAD_FIELD(title);
      LOAD_FIELD(album);
      LOAD_FIELD(artist);
      LOAD_FIELD(genre);
      LOAD_FIELD(year);
      LOAD_FIELD(comment);
      LOAD_FIELD(track);
      LOAD_FIELD(disc);
      LOAD_FIELD(composer);
      LOAD_FIELD(conductor);

#undef LOAD_FIELD

      tag_expressions.append(tag);
      i++;
    }

  tag_expressionsmodel->requery();

  // Event Filter ////////////////////////////////////////////////////////////

  ui.apic_pictureEdit->installEventFilter(this);
}

WMainWindow::~WMainWindow()
{
  // Save Settings ////////////////////////////////////////////////////////////

  QSettings settings(configFileName(), QSettings::IniFormat, this);
  settings.setFallbacksEnabled(false);
  settings.clear();

  for(int i = 0; i < tag_expressions.count(); i++)
    {
      settings.setValue(QString("expression%1/name").arg(i),
			tag_expressions[i].name);
      settings.setValue(QString("expression%1/regexp").arg(i),
			tag_expressions[i].regexp.pattern());

#define SAVE_FIELD(field) \
      saveFieldToSettings(settings, \
			  QString("expression%1").arg(i), \
			  #field, \
			  tag_expressions[i].cap_##field, \
			  tag_expressions[i].str_##field)

      SAVE_FIELD(title);
      SAVE_FIELD(album);
      SAVE_FIELD(artist);
      SAVE_FIELD(genre);
      SAVE_FIELD(year);
      SAVE_FIELD(comment);
      SAVE_FIELD(track);
      SAVE_FIELD(disc);
      SAVE_FIELD(composer);
      SAVE_FIELD(conductor);

#undef SAVE_FIELD
    }

  settings.sync();
}

////// Utility ////////////////////////////////////////////////////////////////

QString WMainWindow::configFileName() const
{
  const QString dir = QApplication::instance()->applicationDirPath();

  return QFileInfo(dir, "taq.ini").absoluteFilePath();
}

TagLib::MPEG::File *WMainWindow::browseMP3File()
{
  QString s = QFileDialog::getOpenFileName(this,
					   tr("Select MP3 File"),
					   QDir::currentPath(),
					   tr("MP3 Files (*.mp3)"));

  if( s.isNull() ) // cancel
    return 0;

  QDir::setCurrent(QFileInfo(s).path());

  TagLib::MPEG::File *file =
    new TagLib::MPEG::File(s.toUtf8().constData(), true,
			   TagLib::AudioProperties::Accurate);
  if( !file->isValid() )
    {
      QMessageBox::critical(this, tr("Error"), tr("Invalid MP3 File!"),
			    QMessageBox::Ok,
			    QMessageBox::NoButton, QMessageBox::NoButton);
      delete file;

      return 0;
    }

  return file;
}

MyTag WMainWindow::currentTag()
{
  MyTag tag;

  tag.name = ui.tag_nameEdit->text();

  tag.regexp.setPattern(ui.tag_expressionEdit->text());
  tag.regexp.setCaseSensitivity(Qt::CaseSensitive);
  tag.regexp.setPatternSyntax(QRegExp::RegExp);

  tag.cap_title     = ui.tag_titleSpin->value();
  tag.cap_album     = ui.tag_albumSpin->value();
  tag.cap_artist    = ui.tag_artistSpin->value();
  tag.cap_genre     = ui.tag_genreSpin->value();
  tag.cap_year      = ui.tag_yearSpin->value();
  tag.cap_comment   = ui.tag_commentSpin->value();
  tag.cap_track     = ui.tag_trackSpin->value();
  tag.cap_disc      = ui.tag_discSpin->value();
  tag.cap_composer  = ui.tag_composerSpin->value();
  tag.cap_conductor = ui.tag_conductorSpin->value();

  tag.str_title     = ui.tag_titleEdit->text().simplified();
  tag.str_album     = ui.tag_albumEdit->text().simplified();
  tag.str_artist    = ui.tag_artistEdit->text().simplified();
  tag.str_genre     = ui.tag_genreEdit->text().simplified();
  tag.str_year      = ui.tag_yearEdit->text().simplified();
  tag.str_comment   = ui.tag_commentEdit->text().simplified();
  tag.str_track     = ui.tag_trackEdit->text().simplified();
  tag.str_disc      = ui.tag_discEdit->text().simplified();
  tag.str_composer  = ui.tag_composerEdit->text().simplified();
  tag.str_conductor = ui.tag_conductorEdit->text().simplified();

  return tag;
}

void WMainWindow::setCurrentTag(const MyTag& tag)
{
  tag_clear();

  ui.tag_nameEdit->setText(tag.name);

  ui.tag_expressionEdit->setText(tag.regexp.pattern());

  ui.tag_titleEdit->setText(tag.str_title);
  ui.tag_albumEdit->setText(tag.str_album);
  ui.tag_artistEdit->setText(tag.str_artist);
  ui.tag_genreEdit->setText(tag.str_genre);
  ui.tag_yearEdit->setText(tag.str_year);
  ui.tag_commentEdit->setText(tag.str_comment);
  ui.tag_trackEdit->setText(tag.str_track);
  ui.tag_discEdit->setText(tag.str_disc);
  ui.tag_composerEdit->setText(tag.str_composer);
  ui.tag_conductorEdit->setText(tag.str_conductor);

  ui.tag_titleSpin->setValue(tag.cap_title);
  ui.tag_albumSpin->setValue(tag.cap_album);
  ui.tag_artistSpin->setValue(tag.cap_artist);
  ui.tag_genreSpin->setValue(tag.cap_genre);
  ui.tag_yearSpin->setValue(tag.cap_year);
  ui.tag_commentSpin->setValue(tag.cap_comment);
  ui.tag_trackSpin->setValue(tag.cap_track);
  ui.tag_discSpin->setValue(tag.cap_disc);
  ui.tag_composerSpin->setValue(tag.cap_composer);
  ui.tag_conductorSpin->setValue(tag.cap_conductor);
}

void WMainWindow::saveFieldToSettings(class QSettings& s,
				      const QString& expr,
				      const QString& key,
				      const int cap, const QString& str)
{
  s.setValue(expr + "/cap_" + key, cap);
  s.setValue(expr + "/str_" + key, str);
}

bool WMainWindow::loadFieldFromSettings(int& cap, QString& str,
					const QString& expr,
					const QString& key,
					const class QSettings& s)
{
  if( !s.contains(expr + "/cap_" + key) ||
      !s.contains(expr + "/str_" + key) )
    return false;

  cap = s.value(expr + "/cap_" + key).toInt();
  str = s.value(expr + "/str_" + key).toString();

  return true;
}

////// protected /////////////////////////////////////////////////////////////

bool WMainWindow::eventFilter(QObject *watched, QEvent *event)
{
  QHelpEvent *hev = dynamic_cast<QHelpEvent*>(event);
  if( hev != 0  &&  !ui.apic_pictureEdit->text().isEmpty() ) {
    QImage image;
    image.load(ui.apic_pictureEdit->text());
    if( !image.isNull() ) {
      csImageTip::showImage(hev->globalPos(), image,
                            this, csImageTip::DrawBorder);
      return true;
    }
  }

  return QMainWindow::eventFilter(watched, event);
}
