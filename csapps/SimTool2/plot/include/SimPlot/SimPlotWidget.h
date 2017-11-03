/****************************************************************************
** Copyright (c) 2017, Carsten Schmidt. All rights reserved.
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

#ifndef __SIMPLOTWIDGET_H__
#define __SIMPLOTWIDGET_H__

#include <QtWidgets/QWidget>

#include <SimPlot/SimPlot.h>
#include <SimPlot/SimPlotSeriesHandle.h>

class QMenu;

class ISimPlotSeriesData;
class IPlotImplementation;

class SIM_PLOT_EXPORT SimPlotWidget : public QWidget {
  Q_OBJECT
public:
  SimPlotWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~SimPlotWidget();

  SimPlotSeriesHandle handle(const QString& name) const;
  SimPlotSeriesHandle insert(ISimPlotSeriesData *data, const QColor& color = QColor());

  SimPlot::DrawFlags drawFlags() const;
  void setDrawFlags(const SimPlot::DrawFlags flags);
  void setDrawFlag(const SimPlot::DrawFlag flag, const bool on);

  void setTitleX(const QString& title);

public slots:
  void setActiveSeries(const QString& seriesName);
  void exportToClipboard();
  void panning();
  void horizontalZoom();
  void verticalZoom();
  void rectangularZoom();
  void reset();

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void enterEvent(QEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void leaveEvent(QEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
  void resizeEvent(QResizeEvent *event);

private:
  enum PanZoom {
    RectangularZoom = 0,
    HorizontalZoom,
    VerticalZoom,
    Panning
  };

  QAction *createAction(const QString& text, const QKeySequence& shortcut);
  void initializeContextMenu();
  void initializeCursor();

  IPlotImplementation *_impl;
  QMenu *_contextMenu;
  PanZoom _panZoom;
  QPoint _dragStart;
  QRect _zoomRect;
};

#endif // __SIMPLOTWIDGET_H__
