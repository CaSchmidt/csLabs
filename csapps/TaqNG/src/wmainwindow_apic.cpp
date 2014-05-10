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

#include <QtCore/QMutex>
#include <QtGui/QImageReader>
#include <QtWidgets/QFileDialog>

#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2tag.h>

#include "taq/wmainwindow.h"

#include "taq/mp3filesmodel.h"
#include "taq/progresswatcher.hpp"
#include "taq/util.h"

////// Private ////////////////////////////////////////////////////////////////

struct AttachedPicture {
  QString    mimeType;
  int        pictureType;
  QString    description;
  QByteArray picture;

  AttachedPicture()
    : mimeType(), pictureType(), description(), picture()
  {
  }
};

// static QMutex mutex;
static AttachedPicture apicData;

static void _apic_worker(QString& filename)
{
  TagLib::MPEG::File file(filename.toUtf8().constData(), false);

  // printf("%s: 1) file\n", qPrintable(filename));

  if( !TagLib::File::isWritable(filename.toUtf8().constData())  ||
      !file.isOpen() ) {
    return;
  }

  // printf("%s: 2) isOpen\n", qPrintable(filename));

  TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);
  if( tag == 0 ) {
    return;
  }

  // printf("%s: 3) tag\n", qPrintable(filename));

  TagLib::ID3v2::AttachedPictureFrame *f =
      new TagLib::ID3v2::AttachedPictureFrame();

  // printf("%s: 4) PicFrame\n", qPrintable(filename));

  f->setMimeType(QStringToTString(apicData.mimeType));
  f->setType(TagLib::ID3v2::AttachedPictureFrame::Type(apicData.pictureType));
  if( !apicData.description.isEmpty() ) {
    f->setDescription(QStringToTString(apicData.description));
  }
  f->setPicture(TagLib::ByteVector(apicData.picture.constData(),
                                   apicData.picture.size()));

  // printf("%s: 5) setPicture\n", qPrintable(filename));

  tag->addFrame(f);

  // printf("%s: 6) addFrame\n", qPrintable(filename));

  // mutex.lock();
  file.save(TagLib::MPEG::File::ID3v2, true);
  // mutex.unlock();

  // printf("%s: 7) save\n", qPrintable(filename));
}

bool isValidPicture(const QString& filename)
{
  if( !QFile::exists(filename) ) {
    return false;
  }

  const QString format(QImageReader::imageFormat(filename));

  QStringList id3v2formats;
  id3v2formats << "jpg" << "jpeg" << "png";

  if( !id3v2formats.contains(format, Qt::CaseInsensitive) ) {
    return false;
  }

  return true;
}

QString mimeTypeForPicture(const QString& filename)
{
  const QString format(QImageReader::imageFormat(filename));

  if( format == "jpg"  ||  format == "jpeg" ) {
    return QString("image/jpeg");
  } else if( format == "png" ) {
    return QString("image/png");
  }

  return QString();
}

////// private slots //////////////////////////////////////////////////////////

void WMainWindow::apic_browsePicture()
{
  const QString filename =
      QFileDialog::getOpenFileName(this, tr("Choose Picture"),
                                   QDir::currentPath(),
                                   tr("Pictures (*.jpg *.jpeg *.png)"));
  if( filename.isEmpty() ) {
    return;
  }

  if( !isValidPicture(filename) ) {
    return;
  }

  ui.apic_pictureEdit->clear();

  QDir::setCurrent(QFileInfo(filename).absolutePath());
  ui.apic_pictureEdit->setText(filename);
}

void WMainWindow::apic_browse()
{
  QString dir = QFileDialog::getExistingDirectory(this,
                                                  tr("Choose Directory"),
                                                  QDir::currentPath());

  if( dir.isNull() ) { // cancel
    return;
  }

  ui.apic_directoryEdit->clear();

  QDir::setCurrent(dir);
  ui.apic_directoryEdit->setText(dir);

  QDir d(dir);
  d.setNameFilters(QStringList("*.mp3"));
  d.setFilter(QDir::Files);

  Mp3FilesModel *m =
      dynamic_cast<Mp3FilesModel*>(ui.apic_filesView->model());
  if( m != 0 ) {
    m->setStringList(d.entryList());
  }
}

void WMainWindow::apic_go()
{
  const QString picfilename = ui.apic_pictureEdit->text();
  if( !isValidPicture(picfilename) ) {
    return;
  }

  /*
   * Begin attached picture data
   */
  {
    apicData = AttachedPicture();

    apicData.mimeType = mimeTypeForPicture(picfilename);
    if( apicData.mimeType.isEmpty() ) {
      return;
    }

    if( ui.apic_typeCombo->currentIndex() < 0  ||
        ui.apic_typeCombo->currentIndex() > TagLib::ID3v2::AttachedPictureFrame::PublisherLogo ) {
      return;
    }
    apicData.pictureType = ui.apic_typeCombo->currentIndex();

    if( !ui.apic_descriptionEdit->text().isEmpty() ) {
      apicData.description = ui.apic_descriptionEdit->text();
    }

    {
      QFile picfile(picfilename);
      if( !picfile.open(QIODevice::ReadOnly) ) {
        return;
      }

      apicData.picture = picfile.readAll();

      picfile.close();

      if( apicData.picture.isEmpty() ) {
        return;
      }
    }
  }
  /*
   * End attached picture data
   */

  QString dir = ui.apic_directoryEdit->text();
  QStringList files;
  if( dynamic_cast<Mp3FilesModel*>(ui.apic_filesView->model()) != 0 ) {
    Mp3FilesModel *m =
        dynamic_cast<Mp3FilesModel*>(ui.apic_filesView->model());
    files = m->activeFiles();
  }

  if( !files.isEmpty() ) {
    for(int i = 0; i < files.size(); i++) {
      const QFileInfo fi(QDir(dir), files.at(i));
      files[i] = fi.absoluteFilePath();
    }

    executeProgressWatcher(this, tr("Attaching"), files, _apic_worker);
  }

  apicData = AttachedPicture();
}
