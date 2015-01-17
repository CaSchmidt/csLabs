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

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include <csPlot3D/csPlot3D.h>

#include <csPlot3D/csAxisLabel.h>
#include <csPlot3D/csPlot3DUtil.h>

////// public ////////////////////////////////////////////////////////////////

csPlot3D::csPlot3D(QWidget *parent, Qt::WindowFlags f)
  : QOpenGLWidget(parent, f)
  , QOpenGLFunctions()
  , _program(0)
  , _projection()
  , _view()
  , _rotZ(-45.0f)
  , _rotX(-22.5f)
  , _posOld()
  , _origin()
  , _axisEdges()
  , _coordBox(0)
  , _surface(0)
  , _xMinLabel()
  , _xMaxLabel()
  , _yMinLabel()
  , _yMaxLabel()
  , _zMinLabel()
  , _zMaxLabel()
{
  setCursor(Qt::CrossCursor);
  setMouseTracking(true);

  // Initialize View Matrix //////////////////////////////////////////////////

  updateViewMatrix();

  // Initialize Labels ///////////////////////////////////////////////////////

  csSetLabelText(_xMinLabel, _L1("xMin"));
  csSetLabelText(_xMaxLabel, _L1("xMax"));
  csSetLabelText(_yMinLabel, _L1("yMin"));
  csSetLabelText(_yMaxLabel, _L1("yMax"));
  csSetLabelText(_zMinLabel, _L1("zMin"));
  csSetLabelText(_zMaxLabel, _L1("zMax"));

  // Create 3D Assets ////////////////////////////////////////////////////////

  _coordBox  = new csCoordinateBox();
  _axisEdges = _coordBox->edges();

  _surface = new csSurface();
}

csPlot3D::~csPlot3D()
{
  makeCurrent();
  delete _program;
  delete _coordBox;
  delete _surface;
}

void csPlot3D::setData(const QVector<float>& x,
                       const QVector<float>& y,
                       const QVector<float>& z)
{
  _surface->setData(x, y, z);
}

void csPlot3D::setXAxisLabels(const QString& minLabel, const QString& maxLabel)
{
  csSetLabelText(_xMinLabel, minLabel);
  csSetLabelText(_xMaxLabel, maxLabel);
}

void csPlot3D::setYAxisLabels(const QString& minLabel, const QString& maxLabel)
{
  csSetLabelText(_yMinLabel, minLabel);
  csSetLabelText(_yMaxLabel, maxLabel);
}

void csPlot3D::setZAxisLabels(const QString& minLabel, const QString& maxLabel)
{
  csSetLabelText(_zMinLabel, minLabel);
  csSetLabelText(_zMaxLabel, maxLabel);
}

////// protected /////////////////////////////////////////////////////////////

void csPlot3D::initializeGL()
{
  initializeOpenGLFunctions();

  // GLSL Shader Program /////////////////////////////////////////////////////

  _program = new QOpenGLShaderProgram(this);
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                    _L1(":/shader/csplot3d_v.glsl"));
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                    _L1(":/shader/csplot3d_f.glsl"));
  _program->link();

#if 0
  printf("VENDOR     = %s\n", glGetString(GL_VENDOR));
  printf("VERSION    = %s\n", glGetString(GL_VERSION));
  printf("SL VERSION = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

void csPlot3D::mouseMoveEvent(QMouseEvent *event)
{
  if( event->buttons().testFlag(Qt::LeftButton) ) {
    _rotZ += -1.0f*(event->pos().x() - _posOld.x());
    _rotX += -1.0f*(event->pos().y() - _posOld.y());
    _posOld = event->pos();

    updateViewMatrix();
    update();
  }
  QOpenGLWidget::mouseMoveEvent(event);
}

void csPlot3D::mousePressEvent(QMouseEvent *event)
{
  if( event->buttons().testFlag(Qt::LeftButton) ) {
    _posOld = event->pos();
  }
  QOpenGLWidget::mousePressEvent(event);
}

void csPlot3D::paintGL()
{
  QPainter p(this);
  p.beginNativePainting(); // Begin OpenGL

  // Visible Surface Determination ///////////////////////////////////////////

  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Clear Depth & Color Buffer //////////////////////////////////////////////

  const QColor cc(Qt::gray);
  glClearColor(cc.redF(), cc.greenF(), cc.blueF(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render 3D Scene /////////////////////////////////////////////////////////

  _program->bind();

  _program->setUniformValue("cs_Projection", _projection);
  _program->setUniformValue("cs_View", _view);

  _coordBox->draw(*_program);
  _surface->drawMesh(*_program);
  _surface->draw(*_program);

  _program->release();

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  p.endNativePainting(); // End OpenGL

  csEdge xAxis, yAxis, zAxis;
  foreach(const csEdge& e, _axisEdges) {
    setNearestEdge(xAxis, e, csEdge::XAxis);
    setNearestEdge(yAxis, e, csEdge::YAxis);
    setNearestEdge(zAxis, e, csEdge::ZAxis);
  }

  drawAxis(p, xAxis, _xMinLabel, _xMaxLabel);
  drawAxis(p, yAxis, _yMinLabel, _yMaxLabel);
  drawAxis(p, zAxis, _zMinLabel, _zMaxLabel);
}

void csPlot3D::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);

  // (1) Reset ///////////////////////////////////////////////////////////////

  _projection.setToIdentity();

  // (2) Projection //////////////////////////////////////////////////////////

  const float w0 = w >= h
      ? float(w) / float(h)
      : 1.0;
  const float h0 = w >= h
      ? 1.0
      : float(h) / float(w);

  const float scale = csCoordinateBox::Size/2.0f;

  const float left   = -w0*scale;
  const float right  =  w0*scale;
  const float bottom = -h0*scale;
  const float top    =  h0*scale;
  const float zNear  =  w0*scale;
  const float zFar   =  zNear + 2.0f*csCoordinateBox::Radius;

  _projection.frustum(left, right, bottom, top, zNear, zFar); // glFrustum()

  // (3) OpenGL Camera Coordinates ///////////////////////////////////////////

  _projection.translate(0.0f, 0.0f, -zNear);    // 2. Translation
  _projection.rotate(-90.0f, 1.0f, 0.0f, 0.0f); // 1. Frame Rotation Tx(90)!

  // (4) Origin of Frustum (in Camera/View Coordinates) //////////////////////

  _origin = QVector4D(0.0f, -zNear, 0.0f, 1.0f);
}

////// private ///////////////////////////////////////////////////////////////

void csPlot3D::drawAxis(QPainter& painter, const csEdge& edge,
                        const QStaticText& minLabel, const QStaticText& maxLabel) const
{
  if( edge.isEmpty() ) {
    return;
  }

  const QPoint p1  = project(edge.p1());
  const QPoint p2  = project(edge.p2());
  const QPoint mid = project(edge.mid());

  const bool isLeft = mid.x() < width() / 2
      ? true
      : false;
  const bool isTop = mid.y() < height() / 2
      ? true
      : false;

  const QPoint minPos = csPlaceAxisLabel(p1, p2, minLabel, isTop, isLeft);
  const QPoint maxPos = csPlaceAxisLabel(p2, p1, maxLabel, isTop, isLeft);
  const QRect  minRect(minPos.x(), minPos.y(),
                       minLabel.size().width(), minLabel.size().height());
  const QRect  maxRect(maxPos.x(), maxPos.y(),
                       maxLabel.size().width(), maxLabel.size().height());

  const QBrush black(Qt::black, Qt::SolidPattern);
  const QPen     pen(black, 0.0, Qt::SolidLine);
  painter.setPen(pen);

  if( !minLabel.text().isEmpty() ) {
    painter.drawStaticText(minPos, minLabel);
  }
  if( !maxLabel.text().isEmpty()  &&  !minRect.intersects(maxRect) ) {
    painter.drawStaticText(maxPos, maxLabel);
  }
}

QPoint csPlot3D::project(const QVector4D& p) const
{
  const float sw = float(width() ) / 2.0f;
  const float sh = float(height()) / 2.0f;
  const QMatrix4x4 mvp = _projection*_view;
  const QVector4D   pp = mvp.map(p);

  return QPoint( pp.x()/pp.w()*sw + sw,
                -pp.y()/pp.w()*sh + sh);
}

void csPlot3D::setNearestEdge(csEdge& nearest, const csEdge& incoming,
                              const csEdge::Axis axis)
{
  if( !incoming.isSilhouette(_view, _origin)  ||  incoming.axis() != axis ) {
    return;
  }
  if( !nearest.isEmpty()  &&  incoming.mid(_view).y() >= nearest.mid(_view).y() ) {
    return;
  }

  nearest = incoming;
}

void csPlot3D::updateViewMatrix()
{
  _view.setToIdentity();
  _view.translate(0.0f,  csCoordinateBox::Radius, 0.0f); // 4. Translation
  _view.rotate(-_rotX, 1.0f, 0.0f, 0.0f);                // 3. Frame Rotation
  _view.rotate(-_rotZ, 0.0f, 0.0f, 1.0f);                // 2. Frame Rotation
  _view.translate(0.0f, -csCoordinateBox::Radius, 0.0f); // 1. Translation
}
