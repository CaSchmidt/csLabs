/****************************************************************************
** Copyright (c) 2014, Carsten Schmidt. All rights reserved.
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

#include <QtCore/QUrl>
#include <QtMultimedia/QMediaPlaylist>

#include "waudioplayer.h"
#include "ui_waudioplayer.h"

////// public ////////////////////////////////////////////////////////////////

WAudioPlayer::WAudioPlayer(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WAudioPlayer)
  , _player(0)
  , _playlist(0)
{
  ui->setupUi(this);

  // Media Player ////////////////////////////////////////////////////////////

  _player = new QMediaPlayer(this);
  connect(_player, SIGNAL(stateChanged(QMediaPlayer::State)),
          SLOT(changeButtonState(QMediaPlayer::State)));
  connect(_player, SIGNAL(currentMediaChanged(const QMediaContent&)),
          SLOT(changeMediaInfo(const QMediaContent&)));

  // Playlist ////////////////////////////////////////////////////////////////

  _playlist = new QMediaPlaylist(this);
  _playlist->setPlaybackMode(QMediaPlaylist::Sequential);
  _player->setPlaylist(_playlist);

  // Buttons /////////////////////////////////////////////////////////////////

  connect(ui->nextButton, SIGNAL(clicked()), SLOT(next()));
  connect(ui->stopButton, SIGNAL(clicked()), SLOT(stop()));
  connect(ui->playButton, SIGNAL(clicked()), SLOT(play()));
  connect(ui->previousButton, SIGNAL(clicked()), SLOT(previous()));
}

WAudioPlayer::~WAudioPlayer()
{
  delete ui;
}

void WAudioPlayer::setFiles(const QStringList& files)
{
  reset();

  foreach(const QString& file, files) {
    _playlist->addMedia(QUrl::fromLocalFile(file));
  }
}

////// public slots //////////////////////////////////////////////////////////

void WAudioPlayer::next()
{
  _playlist->next();
}

void WAudioPlayer::play()
{
  if( _player->state() == QMediaPlayer::PlayingState ) {
    _player->pause();
  } else {
    _player->play();
  }
}

void WAudioPlayer::previous()
{
  _playlist->previous();
}

void WAudioPlayer::stop()
{
  _player->stop();
}

void WAudioPlayer::reset()
{
  _player->stop();
  _playlist->clear();
}

////// private slots /////////////////////////////////////////////////////////

void WAudioPlayer::changeButtonState(QMediaPlayer::State state)
{
#if 1
  if( state == QMediaPlayer::PlayingState ) {
    ui->playButton->setText(tr("Pause"));
  } else {
    ui->playButton->setText(tr("Play"));
  }
#else
  QIcon icon;
  if( state == QMediaPlayer::PlayingState ) {
    icon.addFile(":/audioplayer/pause.png", QSize(), QIcon::Normal, QIcon::Off);
  } else {
    icon.addFile(":/audioplayer/play.png", QSize(), QIcon::Normal, QIcon::Off);
  }
  ui->playButton->setIcon(icon);
  ui->playButton->setIconSize(QSize(75, 75));
#endif

  if( state == QMediaPlayer::StoppedState ) {
    emit fileNameChanged(QString());
  } else {
    changeMediaInfo(_player->currentMedia());
  }
}

void WAudioPlayer::changeMediaInfo(const QMediaContent& content)
{
  if( _player->state() == QMediaPlayer::StoppedState ) {
    emit fileNameChanged(QString());
  } else {
    emit fileNameChanged(content.canonicalUrl().toLocalFile());
  }
}
