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
#include <QtWidgets/QMessageBox>

#include "taq/wmainwindow.h"

#include "taq/util.h"

////// private ////////////////////////////////////////////////////////////////

void WMainWindow::single_clear()
{
  ui.single_filenameEdit->clear();

  ui.single_titleEdit->clear();
  ui.single_albumEdit->clear();
  ui.single_artistEdit->clear();
  ui.single_genreEdit->clear();
  ui.single_yearEdit->clear();
  ui.single_commentEdit->clear();
  ui.single_trackEdit->clear();
  ui.single_discEdit->clear();
  ui.single_composerEdit->clear();
  ui.single_conductorEdit->clear();
}

void WMainWindow::single_setTag(const MyTag& mytag)
{
#define SET_FIELD(field) \
  if( !mytag.str_##field.isEmpty() ) \
    ui.single_##field##Edit->setText(mytag.str_##field);

  SET_FIELD(title);
  SET_FIELD(album);
  SET_FIELD(artist);
  SET_FIELD(genre);
  SET_FIELD(year);
  SET_FIELD(comment);
  SET_FIELD(track);
  SET_FIELD(disc);
  SET_FIELD(composer);
  SET_FIELD(conductor);

#undef SET_FIELD
}

MyTag WMainWindow::single_getTag()
{
  MyTag mytag;

#define GET_FIELD(field) \
  mytag.str_##field = ui.single_##field##Edit->text();

  GET_FIELD(title);
  GET_FIELD(album);
  GET_FIELD(artist);
  GET_FIELD(genre);
  GET_FIELD(year);
  GET_FIELD(comment);
  GET_FIELD(track);
  GET_FIELD(disc);
  GET_FIELD(composer);
  GET_FIELD(conductor);

#undef GET_FIELD

  return mytag;
}

////// private slots //////////////////////////////////////////////////////////

void WMainWindow::single_browse()
{
  TagLib::MPEG::File *f = browseMP3File();
  if( f == 0 )
    return;

  single_clear();
  ui.single_filenameEdit->setText(QString::fromUtf8(f->name()));

  MyTag mytag = util_ID3v2TagToMyTag(f->ID3v2Tag());
  QFileInfo fi(QString::fromUtf8(f->name()));
  MyTag remytag = util_RegExpMyTag(fi.fileName(), currentTag());

  enum Processing {
    Expression = 0,
    Tag,
    ExpressionThenTag,
    TagThenExpression
  };

  switch( ui.single_processingCombo->currentIndex() )
    {
    case TagThenExpression:
      single_setTag(mytag);
      single_setTag(remytag);
      break;

    case ExpressionThenTag:
      single_setTag(remytag);
      single_setTag(mytag);
      break;

    case Tag:
      single_setTag(mytag);
      break;

    case Expression:
    default:
      single_setTag(remytag);
      break;
    }

  delete f;
}

void WMainWindow::single_save()
{
  if( !util_TagFile(ui.single_filenameEdit->text(), single_getTag()) )
    QMessageBox::critical(this, tr("Error"),
			  tr("Unable to write file!"),
			  QMessageBox::Ok,
			  QMessageBox::NoButton, QMessageBox::NoButton);
}
