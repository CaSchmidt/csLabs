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

#include <QtGui/QColor>

#include <csPlot3D/csCoordinateBox.h>

#include <csPlot3D/csOpenGLUtil.h>

////// Macros ////////////////////////////////////////////////////////////////

#define TFR  0
#define TFL  1
#define TRR  2
#define TRL  3
#define BFL  4
#define BFR  5
#define BRL  6
#define BRR  7

////// public static /////////////////////////////////////////////////////////

const float csCoordinateBox::DepthOffset = -0.005f;
const float csCoordinateBox::Radius      = 1.8f*csCoordinateBox::Size/2.0f;
const float csCoordinateBox::Size        = 2.0f;

////// public ////////////////////////////////////////////////////////////////

csCoordinateBox::csCoordinateBox()
  : QOpenGLFunctions()
  , _initRequired(false)
  , _boxData()
  , _box(0)
  , _stripData()
  , _strip(0)
  , _axisData()
  , _axis(0)
{
  // (1) Vertex Data /////////////////////////////////////////////////////////

  _boxData.resize(3*8);

  // Top
  // FR
  _boxData[0*3+0] = Size / 2.0f;
  _boxData[0*3+1] = Radius - Size / 2.0f;
  _boxData[0*3+2] = Size / 2.0f;
  // FL
  _boxData[1*3+0] = -Size / 2.0f;
  _boxData[1*3+1] = Radius - Size / 2.0f;
  _boxData[1*3+2] = Size / 2.0f;
  // RR
  _boxData[2*3+0] = Size / 2.0f;
  _boxData[2*3+1] = Radius + Size / 2.0f;
  _boxData[2*3+2] = Size / 2.0f;
  // RL
  _boxData[3*3+0] = -Size / 2.0f;
  _boxData[3*3+1] = Radius + Size / 2.0f;
  _boxData[3*3+2] = Size / 2.0f;

  // Bottom
  // FL
  _boxData[4*3+0] = -Size / 2.0f;
  _boxData[4*3+1] = Radius - Size / 2.0f;
  _boxData[4*3+2] = -Size / 2.0f;
  // FR
  _boxData[5*3+0] = Size / 2.0f;
  _boxData[5*3+1] = Radius - Size / 2.0f;
  _boxData[5*3+2] = -Size / 2.0f;
  // RL
  _boxData[6*3+0] = -Size / 2.0f;
  _boxData[6*3+1] = Radius + Size / 2.0f;
  _boxData[6*3+2] = -Size / 2.0f;
  // RR
  _boxData[7*3+0] = Size / 2.0f;
  _boxData[7*3+1] = Radius + Size / 2.0f;
  _boxData[7*3+2] = -Size / 2.0f;

  // (2) Strip Data //////////////////////////////////////////////////////////

  _stripData.resize(10);

  _stripData[0] = TRL;
  _stripData[1] = BRL;
  _stripData[2] = TRR;
  _stripData[3] = BRR;
  _stripData[4] = TFR;
  _stripData[5] = BFR;
  _stripData[6] = TFL;
  _stripData[7] = BFL;
  _stripData[8] = TRL;
  _stripData[9] = BRL;

  // (3) Axis Data ///////////////////////////////////////////////////////////

  _axisData.resize(16);

  // Top
  _axisData[0] = TFL;
  _axisData[1] = TFR;
  _axisData[2] = TRR;
  _axisData[3] = TRL;
  // Bottom
  _axisData[4] = BFL;
  _axisData[5] = BFR;
  _axisData[6] = BRR;
  _axisData[7] = BRL;
  // Sides
  _axisData[ 8] = TFL;
  _axisData[ 9] = BFL;
  _axisData[10] = TFR;
  _axisData[11] = BFR;
  _axisData[12] = TRR;
  _axisData[13] = BRR;
  _axisData[14] = TRL;
  _axisData[15] = BRL;

  // (4) Trigger Initialization //////////////////////////////////////////////

  _initRequired = true;
}

csCoordinateBox::~csCoordinateBox()
{
  delete _box;
  delete _strip;
  delete _axis;
}

void csCoordinateBox::draw(QOpenGLShaderProgram& program)
{
  if( _initRequired ) {
    initialize();
  }

  glEnable(GL_CULL_FACE);

  program.setUniformValue("cs_Model", QMatrix4x4());

  _box->bind();
  const int vertexLoc = program.attributeLocation("cs_Vertex");
  program.enableAttributeArray(vertexLoc);
  program.setAttributeBuffer(vertexLoc, GL_FLOAT, 0, 3);

  const int colorLoc = program.attributeLocation("cs_Color");

  // Axis ////////////////////////////////////////////////////////////////////

  program.setUniformValue("cs_DepthOffset", DepthOffset);

  program.setAttributeValue(colorLoc, QColor(Qt::black));

  _axis->bind();
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint)*4));
  glDrawElements(GL_LINES,     8, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint)*8));
  _axis->release();

  // Box /////////////////////////////////////////////////////////////////////

  program.setUniformValue("cs_DepthOffset", 0.0f);

  program.setAttributeValue(colorLoc, QColor(Qt::white));

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Top
  glDrawArrays(GL_TRIANGLE_STRIP, 4, 4); // Bottom

  _strip->bind();
  glDrawElements(GL_TRIANGLE_STRIP, 10, GL_UNSIGNED_INT, 0); // Sides
  _strip->release();

  program.disableAttributeArray(vertexLoc);
  _box->release();
}

csEdges csCoordinateBox::edges() const
{
  csEdges list;

  list.push_back(csEdge(csEdge::XAxis,
                        &_boxData.constData()[TFL*3],
                        &_boxData.constData()[TFR*3],
                        QVector3D(0, 0, 1), QVector3D(0, -1, 0)));
  list.push_back(csEdge(csEdge::XAxis,
                        &_boxData.constData()[TRL*3],
                        &_boxData.constData()[TRR*3],
                        QVector3D(0, 1, 0), QVector3D(0, 0, 1)));
  list.push_back(csEdge(csEdge::XAxis,
                        &_boxData.constData()[BFL*3],
                        &_boxData.constData()[BFR*3],
                        QVector3D(0, -1, 0), QVector3D(0, 0, -1)));
  list.push_back(csEdge(csEdge::XAxis,
                        &_boxData.constData()[BRL*3],
                        &_boxData.constData()[BRR*3],
                        QVector3D(0, 0, -1), QVector3D(0, 1, 0)));

  list.push_back(csEdge(csEdge::YAxis,
                        &_boxData.constData()[TFL*3],
                        &_boxData.constData()[TRL*3],
                        QVector3D(-1, 0, 0), QVector3D(0, 0, 1)));
  list.push_back(csEdge(csEdge::YAxis,
                        &_boxData.constData()[TFR*3],
                        &_boxData.constData()[TRR*3],
                        QVector3D(0, 0, 1), QVector3D(1, 0, 0)));
  list.push_back(csEdge(csEdge::YAxis,
                        &_boxData.constData()[BFL*3],
                        &_boxData.constData()[BRL*3],
                        QVector3D(0, 0, -1), QVector3D(-1, 0, 0)));
  list.push_back(csEdge(csEdge::YAxis,
                        &_boxData.constData()[BFR*3],
                        &_boxData.constData()[BRR*3],
                        QVector3D(1, 0, 0), QVector3D(0, 0, -1)));

  list.push_back(csEdge(csEdge::ZAxis,
                        &_boxData.constData()[BFL*3],
                        &_boxData.constData()[TFL*3],
                        QVector3D(-1, 0, 0), QVector3D(0, -1, 0)));
  list.push_back(csEdge(csEdge::ZAxis,
                        &_boxData.constData()[BRL*3],
                        &_boxData.constData()[TRL*3],
                        QVector3D(0, 1, 0), QVector3D(-1, 0, 0)));
  list.push_back(csEdge(csEdge::ZAxis,
                        &_boxData.constData()[BRR*3],
                        &_boxData.constData()[TRR*3],
                        QVector3D(1, 0, 0), QVector3D(0, 1, 0)));
  list.push_back(csEdge(csEdge::ZAxis,
                        &_boxData.constData()[BFR*3],
                        &_boxData.constData()[TFR*3],
                        QVector3D(0, -1, 0), QVector3D(1, 0, 0)));

  return list;
}

////// private ///////////////////////////////////////////////////////////////

void csCoordinateBox::initialize()
{
  initializeOpenGLFunctions();

  // (1) Vertex Array Object /////////////////////////////////////////////////

  _box = csAllocateBuffer(_box,
                          _boxData.constData(), sizeof(GLfloat)*3*8);

  // (2) Index Array Object //////////////////////////////////////////////////

  _strip = csAllocateBuffer(_strip,
                            _stripData.constData(), sizeof(GLuint)*10,
                            QOpenGLBuffer::IndexBuffer);

  // (3) Axis ////////////////////////////////////////////////////////////////

  _axis = csAllocateBuffer(_axis,
                           _axisData.constData(), sizeof(GLuint)*16,
                           QOpenGLBuffer::IndexBuffer);

  // Done ////////////////////////////////////////////////////////////////////

  _initRequired = false;
}
