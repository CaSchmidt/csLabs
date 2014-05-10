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

#include "taq/id3v2.h"

namespace ID3v2 {

  ////// Global ///////////////////////////////////////////////////////////////

  const ID3v2Frames frames;

  ////// public ///////////////////////////////////////////////////////////////

  ID3v2Frames::ID3v2Frames()
    : QMap<QByteArray, QString>()
  {
    // 4.2.1. Identification frames ///////////////////////////////////////////

    insert(QByteArray("TIT1"), QString("Content group description"));
    insert(QByteArray("TIT2"), QString("Title/Songname/Content description"));
    insert(QByteArray("TIT3"), QString("Subtitle/Description refinement"));
    insert(QByteArray("TALB"), QString("Album/Movie/Show title"));
    insert(QByteArray("TOAL"), QString("Original album/movie/show title"));
    insert(QByteArray("TRCK"), QString("Track number/Position in set"));
    insert(QByteArray("TPOS"), QString("Part of a set"));
    insert(QByteArray("TSST"), QString("Set subtitle"));
    insert(QByteArray("TSRC"), QString("ISRC"));

    // 4.2.2. Involved persons frames /////////////////////////////////////////

    insert(QByteArray("TPE1"),
	   QString("Lead artist/Lead performer/Soloist/Performing group"));
    insert(QByteArray("TPE2"), QString("Band/Orchestra/Accompaniment"));
    insert(QByteArray("TPE3"), QString("Conductor"));
    insert(QByteArray("TPE4"),
	   QString("Interpreted, remixed, or otherwise modified by"));
    insert(QByteArray("TOPE"), QString("Original artist/performer"));
    insert(QByteArray("TEXT"), QString("Lyricist/Text writer"));
    insert(QByteArray("TOLY"), QString("Original lyricist/text writer"));
    insert(QByteArray("TCOM"), QString("Composer"));
    insert(QByteArray("TMCL"), QString("Musician credits list"));
    insert(QByteArray("TIPL"), QString("Involved people list"));
    insert(QByteArray("TENC"), QString("Encoded by"));

    // 4.2.3. Derived and subjective properties frames ////////////////////////

    insert(QByteArray("TBPM"), QString("BPM"));
    insert(QByteArray("TLEN"), QString("Length"));
    insert(QByteArray("TKEY"), QString("Initial key"));
    insert(QByteArray("TLAN"), QString("Language"));
    insert(QByteArray("TCON"), QString("Content type"));
    insert(QByteArray("TFLT"), QString("File type"));
    insert(QByteArray("TMED"), QString("Media type"));
    insert(QByteArray("TMOO"), QString("Mood"));

    // 4.2.4. Rights and license frames ///////////////////////////////////////

    insert(QByteArray("TCOP"), QString("Copyright message"));
    insert(QByteArray("TPRO"), QString("Produced notice"));
    insert(QByteArray("TPUB"), QString("Publisher"));
    insert(QByteArray("TOWN"), QString("File owner/licensee"));
    insert(QByteArray("TRSN"), QString("Internet radio station name"));
    insert(QByteArray("TRSO"), QString("Internet radio station owner"));

    // 4.2.5. Other text frames ///////////////////////////////////////////////

    insert(QByteArray("TOFN"), QString("Original filename"));
    insert(QByteArray("TDLY"), QString("Playlist delay"));
    insert(QByteArray("TDEN"), QString("Encoding time"));
    insert(QByteArray("TDOR"), QString("Original release time"));
    insert(QByteArray("TDRC"), QString("Recording time"));
    insert(QByteArray("TDRL"), QString("Release time"));
    insert(QByteArray("TDTG"), QString("Tagging time"));
    insert(QByteArray("TSSE"),
	   QString("Software/Hardware and settings used for encoding"));
    insert(QByteArray("TSOA"), QString("Album sort order"));
    insert(QByteArray("TSOP"), QString("Performer sort order"));
    insert(QByteArray("TSOT"), QString("Title sort order"));
  }

  ID3v2Frames::~ID3v2Frames()
  {
  }

}; // namespace ID3v2
