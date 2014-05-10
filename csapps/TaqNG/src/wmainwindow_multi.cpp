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

#include <QtCore/QStringList>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "taq/wmainwindow.h"

#include "taq/util.h"
#include "taq/mp3filesmodel.h"
#include "taq/progresswatcher.hpp"

////// Private ////////////////////////////////////////////////////////////////

static MyTag _current_tag;

static void _tag_worker(QString& absfilename)
{
  const QFileInfo fi(absfilename);

  const MyTag remytag = util_RegExpMyTag(fi.fileName(), _current_tag);

  util_TagFile(fi.absoluteFilePath(), remytag);
}

////// private ////////////////////////////////////////////////////////////////

////// private slots //////////////////////////////////////////////////////////

void WMainWindow::multi_browse()
{
  QString dir = QFileDialog::getExistingDirectory(this,
						  tr("Choose Directory"),
						  QDir::currentPath());

  if( dir.isNull() ) // cancel
    return;

  ui.multi_directoryEdit->clear();

  QDir::setCurrent(dir);
  ui.multi_directoryEdit->setText(dir);

  QDir d(dir);
  d.setNameFilters(QStringList("*.mp3"));
  d.setFilter(QDir::Files);

  Mp3FilesModel *m =
    dynamic_cast<Mp3FilesModel*>(ui.multi_filesView->model());
  if( m != 0 )
    m->setStringList(d.entryList());
}

void WMainWindow::multi_go()
{
  QString dir = ui.multi_directoryEdit->text();
  QStringList files;
  if( dynamic_cast<Mp3FilesModel*>(ui.multi_filesView->model()) != 0 )
    {
      Mp3FilesModel *m =
	dynamic_cast<Mp3FilesModel*>(ui.multi_filesView->model());
      files = m->activeFiles();
    }

  if( !files.isEmpty() )
    {
      for(int i = 0; i < files.size(); i++)
	{
	  const QFileInfo fi(QDir(dir), files.at(i));
	  files[i] = fi.absoluteFilePath();
	}

      _current_tag = currentTag();

      executeProgressWatcher(this, tr("Tagging"), files, _tag_worker);
    }
}
