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

#include <taglib/id3v2frame.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>

#include "taq/util.h"

MyTag util_ID3v2TagToMyTag(const TagLib::ID3v2::Tag *tag)
{
  if( tag == 0 )
    return MyTag();

  MyTag mytag;

  mytag.str_title   = TStringToQString(tag->title());
  mytag.str_album   = TStringToQString(tag->album());
  mytag.str_artist  = TStringToQString(tag->artist());
  mytag.str_genre   = TStringToQString(tag->genre());
  if( tag->year() > 0 )
    mytag.str_year    = QString::number(tag->year());
  mytag.str_comment = TStringToQString(tag->comment());
  if( tag->track() > 0 )
    mytag.str_track   = QString::number(tag->track());

  TagLib::ID3v2::FrameList discs = tag->frameListMap()["TPOS"];
  if( !discs.isEmpty() )
    mytag.str_disc = TStringToQString(discs.front()->toString());

  TagLib::ID3v2::FrameList composers = tag->frameListMap()["TCOM"];
  if( !composers.isEmpty() )
    mytag.str_composer = TStringToQString(composers.front()->toString());

  TagLib::ID3v2::FrameList conductors = tag->frameListMap()["TPE3"];
  if( !conductors.isEmpty() )
    mytag.str_conductor = TStringToQString(conductors.front()->toString());

  return mytag;
}

MyTag util_RegExpMyTag(const QString& filename, const MyTag& mytag)
{
  MyTag remytag = mytag;

  QRegExp rx = mytag.regexp;
  if( rx.indexIn(filename) < 0 )
    return remytag;

#define CAPTURE_FIELD(field) \
  if( mytag.cap_##field > 0 ) \
    remytag.str_##field = rx.cap(mytag.cap_##field);

  CAPTURE_FIELD(title);
  CAPTURE_FIELD(album);
  CAPTURE_FIELD(artist);
  CAPTURE_FIELD(genre);
  CAPTURE_FIELD(year);
  CAPTURE_FIELD(comment);
  CAPTURE_FIELD(track);
  CAPTURE_FIELD(disc);
  CAPTURE_FIELD(composer);
  CAPTURE_FIELD(conductor);

#undef CAPTURE_FIELD

  return remytag;
}

bool util_TagFile(const QString& fullname, const MyTag& mytag)
{
  TagLib::MPEG::File file(fullname.toUtf8().constData(), false);

  if( !TagLib::File::isWritable(fullname.toUtf8().constData())  ||
      !file.isOpen() )
    return false;

  if( !file.strip(TagLib::MPEG::File::AllTags, true) )
    return false;

  TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);
  if( tag == 0 )
    return false;

  // Set Text Encoding ////////////////////////////////////////////////////////

  TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF16);

  // Write Frames /////////////////////////////////////////////////////////////

  tag->setTitle(QStringToTString(mytag.str_title));
  tag->setAlbum(QStringToTString(mytag.str_album));
  tag->setArtist(QStringToTString(mytag.str_artist));
  // tag->setGenre(QStringToTString(mytag.str_genre)); // Custom Behaviour ////
  tag->setYear(mytag.str_year.toUInt());
  tag->setComment(QStringToTString(mytag.str_comment));
  tag->setTrack(mytag.str_track.toUInt());

  // Begin Custom Frames //////////////////////////////////////////////////////
  //

  // Text Encoding ////////////////////////////////////////////////////////////

  const TagLib::String::Type encoding =
    TagLib::ID3v2::FrameFactory::instance()->defaultTextEncoding();

  TagLib::ID3v2::TextIdentificationFrame *textframe;

  // Genre ////////////////////////////////////////////////////////////////////

  if( !mytag.str_genre.isEmpty() )
    {
      textframe = new TagLib::ID3v2::TextIdentificationFrame("TCON", encoding);
      textframe->setText(QStringToTString(mytag.str_genre));
      tag->addFrame(textframe);
    }

  // Disc /////////////////////////////////////////////////////////////////////

  if( !mytag.str_disc.isEmpty() )
    {
      textframe = new TagLib::ID3v2::TextIdentificationFrame("TPOS", encoding);
      textframe->setText(QStringToTString(mytag.str_disc));
      tag->addFrame(textframe);
    }

  // Composer /////////////////////////////////////////////////////////////////

  if( !mytag.str_composer.isEmpty() )
    {
      textframe = new TagLib::ID3v2::TextIdentificationFrame("TCOM", encoding);
      textframe->setText(QStringToTString(mytag.str_composer));
      tag->addFrame(textframe);
    }

  // Conductor ////////////////////////////////////////////////////////////////

  if( !mytag.str_conductor.isEmpty() )
    {
      textframe = new TagLib::ID3v2::TextIdentificationFrame("TPE3", encoding);
      textframe->setText(QStringToTString(mytag.str_conductor));
      tag->addFrame(textframe);
    }

  //
  // End Custom Frame /////////////////////////////////////////////////////////

  file.save(TagLib::MPEG::File::ID3v2, true);

  return true;
}
