/****************************************************************************
** Copyright (c) 2014-2015, Carsten Schmidt. All rights reserved.
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

#ifndef __CSPLOT3D_H__
#define __CSPLOT3D_H__

#include <QtGui/QColor>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QStaticText>
#include <QtWidgets/QOpenGLWidget>

#include <csPlot3D/csplot3d_config.h>
#include <csPlot3D/csCoordinateBox.h>
#include <csPlot3D/csSurface.h>

class CS_PLOT3D_EXPORT csPlot3D : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
public:
  csPlot3D(QWidget *parent = 0, Qt::WindowFlags f = 0);
  ~csPlot3D();

  void setData(const QVector<float>& x,
               const QVector<float>& y,
               const QVector<float>& z);

  void setXAxisLabels(const QString& minLabel, const QString& maxLabel);
  void setYAxisLabels(const QString& minLabel, const QString& maxLabel);
  void setZAxisLabels(const QString& minLabel, const QString& maxLabel);

protected:
  virtual void initializeGL();
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void paintGL();
  virtual void resizeGL(int w, int h);

private:
  void drawAxis(QPainter& painter, const csEdge& edge,
                const QStaticText& minLabel, const QStaticText& maxLabel) const;
  QPoint project(const QVector4D& p) const;
  void setNearestEdge(csEdge& nearest, const csEdge& incoming,
                      const csEdge::Axis axis);
  void updateViewMatrix();

  QOpenGLShaderProgram *_program;
  QOpenGLShaderProgram *_surfaceShader;
  QMatrix4x4            _projection;
  QMatrix4x4            _view;
  float                 _rotZ;
  float                 _rotX;
  QPoint                _posOld;
  QVector4D             _origin;
  csEdges               _axisEdges;
  csCoordinateBox      *_coordBox;
  csSurface            *_surface;
  QStaticText           _xMinLabel;
  QStaticText           _xMaxLabel;
  QStaticText           _yMinLabel;
  QStaticText           _yMaxLabel;
  QStaticText           _zMinLabel;
  QStaticText           _zMaxLabel;
};

#endif // __CSPLOT3D_H__
