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

#include "taq/wmainwindow.h"

////// private ////////////////////////////////////////////////////////////////

void WMainWindow::tag_clear()
{
  ui.tag_nameEdit->clear();
  ui.tag_expressionEdit->clear();

  tag_setMaximum(0);

  tag_setEditable(*ui.tag_titleEdit, true);
  tag_setEditable(*ui.tag_albumEdit, true);
  tag_setEditable(*ui.tag_artistEdit, true);
  tag_setEditable(*ui.tag_genreEdit, true);
  tag_setEditable(*ui.tag_yearEdit, true);
  tag_setEditable(*ui.tag_commentEdit, true);
  tag_setEditable(*ui.tag_trackEdit, true);
  tag_setEditable(*ui.tag_discEdit, true);
  tag_setEditable(*ui.tag_composerEdit, true);
  tag_setEditable(*ui.tag_conductorEdit, true);
}

void WMainWindow::tag_setMaximum(int max)
{
  ui.tag_titleSpin->setMaximum(max);
  ui.tag_albumSpin->setMaximum(max);
  ui.tag_artistSpin->setMaximum(max);
  ui.tag_genreSpin->setMaximum(max);
  ui.tag_yearSpin->setMaximum(max);
  ui.tag_commentSpin->setMaximum(max);
  ui.tag_trackSpin->setMaximum(max);
  ui.tag_discSpin->setMaximum(max);
  ui.tag_composerSpin->setMaximum(max);
  ui.tag_conductorSpin->setMaximum(max);
}

void WMainWindow::tag_setEditable(QLineEdit& e, const bool b)
{
  if( b )
    {
      e.clear();
      e.setEnabled(true);
    }
  else
    {
      e.clear();
      e.setEnabled(false);
    }
}

void WMainWindow::tag_setRegExp(QLineEdit& e, const QSpinBox& s)
{
  if( s.value() != 0 )
    tag_setEditable(e, false);
  else
    tag_setEditable(e, true);
}

////// private slots //////////////////////////////////////////////////////////

void WMainWindow::tag_changeState(const QString& /*s*/)
{
  QRegExp rx(ui.tag_expressionEdit->text(),
	     Qt::CaseSensitive, QRegExp::RegExp);

  if( !rx.isEmpty()  &&  rx.isValid() )
    tag_setMaximum(rx.captureCount());
  else if( rx.isEmpty() )
    tag_setMaximum(0);

  tag_setRegExp(*ui.tag_titleEdit, *ui.tag_titleSpin);
  tag_setRegExp(*ui.tag_albumEdit, *ui.tag_albumSpin);
  tag_setRegExp(*ui.tag_artistEdit, *ui.tag_artistSpin);
  tag_setRegExp(*ui.tag_genreEdit, *ui.tag_genreSpin);
  tag_setRegExp(*ui.tag_yearEdit, *ui.tag_yearSpin);
  tag_setRegExp(*ui.tag_commentEdit, *ui.tag_commentSpin);
  tag_setRegExp(*ui.tag_trackEdit, *ui.tag_trackSpin);
  tag_setRegExp(*ui.tag_discEdit, *ui.tag_discSpin);
  tag_setRegExp(*ui.tag_composerEdit, *ui.tag_composerSpin);
  tag_setRegExp(*ui.tag_conductorEdit, *ui.tag_conductorSpin);
}

void WMainWindow::tag_selectExpression(const QModelIndex& index)
{
  if( !index.isValid()  ||  index.row() >= tag_expressions.count() )
    return;

  setCurrentTag(tag_expressions[index.row()]);
}

void WMainWindow::tag_add()
{
  MyTag tag = currentTag();

  if( tag.regexp.isEmpty()  ||  !tag.regexp.isValid()  ||  tag.name.isEmpty() )
    {
      QMessageBox::critical(this, tr("Error"),
			    tr("Invalid name or expression!"),
			    QMessageBox::Ok,
			    QMessageBox::NoButton, QMessageBox::NoButton);
      return;
    }

  for(int i = 0; i < tag_expressions.count(); i++)
    {
      if( tag.name == tag_expressions[i].name )
	{
	  tag_expressions[i] = tag;
	  tag_expressionsmodel->requery();
	  return;
	}
    }

  tag_expressions.append(tag);
  tag_expressionsmodel->requery();
}

void WMainWindow::tag_remove()
{
  if( !ui.tag_expressionList->currentIndex().isValid() ||
      ui.tag_expressionList->currentIndex().row() >= tag_expressions.count() )
    return;

  tag_expressions.removeAt(ui.tag_expressionList->currentIndex().row());

  tag_expressionsmodel->requery();
}
