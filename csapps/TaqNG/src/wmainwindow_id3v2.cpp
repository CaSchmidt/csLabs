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

#include <QtWidgets/QMessageBox>

#include <taglib/id3v2tag.h>
#include <taglib/textidentificationframe.h>

#include "taq/wmainwindow.h"

#include "taq/id3v2.h"

////// private ////////////////////////////////////////////////////////////////

void WMainWindow::id3v2_clear()
{
  id3v2_myframes.clear();
  ui.id3v2_filenameEdit->clear();
  id3v2_myframesmodel->requery();
}

////// private slots //////////////////////////////////////////////////////////

void WMainWindow::id3v2_browse()
{
  TagLib::MPEG::File *file = browseMP3File();
  if( file == 0 )
    return;

  TagLib::ID3v2::Tag *tag = file->ID3v2Tag();
  if( tag == 0 )
    {
      QMessageBox::critical(this, tr("Error"),
			    tr("No tagging information available!"),
			    QMessageBox::Ok,
			    QMessageBox::NoButton, QMessageBox::NoButton);
      delete file;
      return;
    }

  id3v2_clear();

  for(TagLib::ID3v2::FrameList::ConstIterator it = tag->frameList().begin();
      it != tag->frameList().end(); it++)
    {
      TagLib::ID3v2::Frame *frame = *it;
      TagLib::ID3v2::TextIdentificationFrame *tframe =
	dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame);
      if( tframe != 0 )
	{
	  MyFrame f;

	  QByteArray b(tframe->frameID().data(), 4);

	  f.id      = QString(b);
	  f.frame   = ID3v2::frames.value(b, QString());
	  f.content = TStringToQString(tframe->toString());

	  id3v2_myframes.append(f);
	}
    }

  ui.id3v2_filenameEdit->setText(QString::fromUtf8(file->name()));
  id3v2_myframesmodel->requery();
  delete file;
}
