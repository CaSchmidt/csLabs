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

#include <csPlot3D/csSurface.h>

#include <csPlot3D/csCoordinateBox.h>
#include <csPlot3D/csOpenGLUtil.h>

////// public ////////////////////////////////////////////////////////////////

csSurface::csSurface()
  : QOpenGLFunctions()
  , _initRequired(false)
  , _paletteAxis()
  , _palette()
  , _meshInfo()
  , _surfaceData()
  , _surface(0)
  , _colorData()
  , _color(0)
  , _stripData()
  , _strip(0)
  , _meshYData()
  , _meshY(0)
  , _model()
{
  // Initialize (a Quite Simple) Palette /////////////////////////////////////

  _paletteAxis << 0.0f             << 0.5f              << 1.0f;
  _palette     << QColor(Qt::blue) << QColor(Qt::green) << QColor(Qt::red);

  // Trigger Initialization //////////////////////////////////////////////////

  _initRequired = true;
}

csSurface::~csSurface()
{
  delete _surface;
  delete _color;
  delete _strip;
  delete _meshY;
}

void csSurface::draw(QOpenGLShaderProgram& program)
{
  if( _initRequired ) {
    initialize();
  }

  if( _meshInfo.isEmpty() ) {
    return;
  }

  glDisable(GL_CULL_FACE);

  program.setUniformValue("cs_DepthOffset", 0.0f);
  program.setUniformValue("cs_Model", _model);

  _surface->bind();
  const int vertexLoc = program.attributeLocation("cs_Vertex");
  program.enableAttributeArray(vertexLoc);
  program.setAttributeBuffer(vertexLoc, GL_FLOAT, 0, 3);

  _color->bind();
  const int colorLoc = program.attributeLocation("cs_Color");
  program.enableAttributeArray(colorLoc);
  program.setAttributeBuffer(colorLoc, GL_FLOAT, 0, 3);

  _strip->bind();
  const int numStrips       =   _meshInfo.rowCount()   -1;
  const int numVertPerStrip = 2*_meshInfo.columnCount();
  for(int y = 0; y < numStrips; y++) {
    const GLuint  offset  = sizeof(GLuint)*y*numVertPerStrip;
    const GLvoid *indices = (GLvoid*)offset;
    glDrawElements(GL_TRIANGLE_STRIP, numVertPerStrip, GL_UNSIGNED_INT, indices);
  }
  _strip->release();

  program.disableAttributeArray(colorLoc);
  _color->release();

  program.disableAttributeArray(vertexLoc);
  _surface->release();
}

void csSurface::drawMesh(QOpenGLShaderProgram& program)
{
  if( _initRequired ) {
    initialize();
  }

  if( _meshInfo.isEmpty() ) {
    return;
  }

  program.setUniformValue("cs_DepthOffset", csCoordinateBox::DepthOffset);
  program.setUniformValue("cs_Model", _model);

  _surface->bind();
  const int vertexLoc = program.attributeLocation("cs_Vertex");
  program.enableAttributeArray(vertexLoc);
  program.setAttributeBuffer(vertexLoc, GL_FLOAT, 0, 3);

  const int colorLoc = program.attributeLocation("cs_Color");
  program.setAttributeValue(colorLoc, QColor(Qt::black));

  // Along x-Axis
  for(int y = 0; y < _meshInfo.rowCount(); y++) {
    glDrawArrays(GL_LINE_STRIP,
                 y*_meshInfo.columnCount(), _meshInfo.columnCount());
  }

  // Along y-Axis
  _meshY->bind();
  for(int x = 0; x < _meshInfo.columnCount(); x++) {
    const GLuint  offset  = sizeof(GLuint)*x*_meshInfo.rowCount();
    const GLvoid *indices = (GLvoid*)offset;
    glDrawElements(GL_LINE_STRIP, _meshInfo.rowCount(), GL_UNSIGNED_INT, indices);

    const GLenum error = glGetError();
    if( error != 0 ) {
      printf("%d: error=0x%08X\n", x, error);
    }
  }
  _meshY->release();

  program.disableAttributeArray(vertexLoc);
  _surface->release();
}

void csSurface::setData(const QVector<float>& x,
                        const QVector<float>& y,
                        const QVector<float>& z)
{
  // (0) Sanity Check ////////////////////////////////////////////////////////

  if( !_meshInfo.initialize(x, y, z) ) {
    return;
  }

  // (1) Create Surface Data /////////////////////////////////////////////////

  _surfaceData.resize(3 * _meshInfo.vertexCount());
  for(int i = 0; i < _meshInfo.vertexCount(); i++) {
    const int column = _meshInfo.column(i); // x
    const int row    = _meshInfo.row(i);    // y

    _surfaceData[i*3+0] = x[column];
    _surfaceData[i*3+1] = y[row];
    _surfaceData[i*3+2] = z[i];
  }

  // (2) Create Color Data ///////////////////////////////////////////////////

  _colorData.resize(3 * _meshInfo.vertexCount());
  for(int i = 0; i < _meshInfo.vertexCount(); i++) {
    const float z  = _surfaceData[i*3+2];
    const float z0 = _meshInfo.zInterval() == 0.0f
        ? qBound(_paletteAxis.front(), 0.0f, _paletteAxis.back())
        : qBound(_paletteAxis.front(),
                 csInter1D(z,
                           _meshInfo.zMin(), _meshInfo.zMax(),
                           _paletteAxis.front(), _paletteAxis.back()),
                 _paletteAxis.back());

    float r(0.0f), g(0.0f), b(0.0f);
    for(int k = 1; k < _palette.size(); k++) {
      if( z0 <= _paletteAxis[k] ) {
        r = csInter1D(z0,
                      _paletteAxis[k-1], _paletteAxis[k],
            (float)_palette[k-1].redF(), (float)_palette[k].redF());
        g = csInter1D(z0,
                      _paletteAxis[k-1], _paletteAxis[k],
            (float)_palette[k-1].greenF(), (float)_palette[k].greenF());
        b = csInter1D(z0,
                      _paletteAxis[k-1], _paletteAxis[k],
            (float)_palette[k-1].blueF(), (float)_palette[k].blueF());
        break;
      }
    }

    _colorData[i*3+0] = qBound(0.0f, r, 1.0f);
    _colorData[i*3+1] = qBound(0.0f, g, 1.0f);
    _colorData[i*3+2] = qBound(0.0f, b, 1.0f);
  }

  // (3) Create Strip Data ///////////////////////////////////////////////////

  const int numStrips       =   _meshInfo.rowCount()   -1;
  const int numVertPerStrip = 2*_meshInfo.columnCount();
  _stripData.resize(numStrips*numVertPerStrip);

  for(int y = 0; y < numStrips; y++) { // Along y-Axis
    for(int x = 0; x < _meshInfo.columnCount(); x++) { // Along x-Axis
      const int index0 = (y+1)*_meshInfo.columnCount() + x;
      const int index1 =  y   *_meshInfo.columnCount() + x;

      _stripData[y*numVertPerStrip+2*x  ] = index0;
      _stripData[y*numVertPerStrip+2*x+1] = index1;
    }
  }

  // (4) Create Mesh Data (y-Direction) //////////////////////////////////////

  _meshYData.resize(_meshInfo.vertexCount());
  for(int x = 0; x < _meshInfo.columnCount(); x++) {
    for(int y = 0; y < _meshInfo.rowCount(); y++) {
      _meshYData[x*_meshInfo.rowCount()+y] = _meshInfo.index(x, y);
    }
  }

  // (5) Update Model Matrix /////////////////////////////////////////////////

  updateModelMatrix();

  // (6) Trigger Initialization //////////////////////////////////////////////

  _initRequired = true;
}

////// private ///////////////////////////////////////////////////////////////

void csSurface::initialize()
{
  initializeOpenGLFunctions();

  // (0) Sanity Check ////////////////////////////////////////////////////////

  if( _meshInfo.isEmpty() ) {
    return;
  }

  // (1) Surface's Buffer ////////////////////////////////////////////////////

  _surface = csAllocateBuffer(_surface,
                              _surfaceData.constData(),
                              sizeof(GLfloat)*3*_meshInfo.vertexCount());

  // (2) Color's Buffer //////////////////////////////////////////////////////

  _color = csAllocateBuffer(_color,
                            _colorData.constData(),
                            sizeof(GLfloat)*3*_meshInfo.vertexCount());

  // (3) Strip's Buffer //////////////////////////////////////////////////////

  _strip = csAllocateBuffer(_strip,
                            _stripData.constData(),
                            sizeof(GLuint)*_stripData.count(),
                            QOpenGLBuffer::IndexBuffer);

  // (4) Mesh's Buffer (y-Direction) /////////////////////////////////////////

  _meshY = csAllocateBuffer(_meshY,
                            _meshYData.constData(),
                            sizeof(GLuint)*_meshYData.size(),
                            QOpenGLBuffer::IndexBuffer);

  // Done ////////////////////////////////////////////////////////////////////

  _initRequired = false;
}

void csSurface::updateModelMatrix()
{
  const float sx = _meshInfo.xInterval() == 0.0f
      ? 1.0f
      : csCoordinateBox::Size / _meshInfo.xInterval();
  const float sy = _meshInfo.yInterval() == 0.0f
      ? 1.0f
      : csCoordinateBox::Size / _meshInfo.yInterval();
  const float sz = _meshInfo.zInterval() == 0.0f
      ? 1.0f
      : csCoordinateBox::Size / _meshInfo.zInterval();

  _model.setToIdentity();
  _model.translate(-csCoordinateBox::Size/2.0f,    // 3. Center Translation
                   csCoordinateBox::Radius-csCoordinateBox::Size/2.0f,
                   -csCoordinateBox::Size/2.0f);
  _model.scale(sx, sy, sz);                        // 2. Scale to Unity
  _model.translate(-_meshInfo.xMin(),              // 1. Alignment Translation
                   -_meshInfo.yMin(),
                   -_meshInfo.zMin());
}
