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

#include <cstring>

#include <QtGui/QColor>

#include <csPlot3D/csSurface.h>

#include <csPlot3D/csCoordinateBox.h>
#include <csPlot3D/csOpenGLUtil.h>

////// Macros ////////////////////////////////////////////////////////////////

#define TMU_COLORMAP  0

////// public ////////////////////////////////////////////////////////////////

csSurface::csSurface()
  : QOpenGLFunctions()
  , _initRequired(false)
  , _paletteAxis()
  , _palette()
  , _meshInfo()
  , _surfaceData()
  , _surface(0)
  , _stripData()
  , _strip(0)
  , _meshYData()
  , _meshY(0)
  , _model()
  , _colorImage()
  , _colorTexture(0)
{
  // Initialize (a Quite Simple) Palette /////////////////////////////////////

  _paletteAxis << 0.0f             << 0.5f              << 1.0f;
  _palette     << QColor(Qt::blue) << QColor(Qt::green) << QColor(Qt::red);
  updateColorImage();

  // Trigger Initialization //////////////////////////////////////////////////

  _initRequired = true;
}

csSurface::~csSurface()
{
  delete _surface;
  delete _strip;
  delete _meshY;
  delete _colorTexture;
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

  program.setUniformValue("cs_Model", _model);
  program.setUniformValue("cs_zMin", _meshInfo.zMin());
  program.setUniformValue("cs_zInterval", _meshInfo.zInterval());
  program.setUniformValue("cs_ColorMap", TMU_COLORMAP);

  _surface->bind();
  const int vertexLoc = program.attributeLocation("cs_Vertex");
  program.enableAttributeArray(vertexLoc);
  program.setAttributeBuffer(vertexLoc, GL_FLOAT, 0, 3);

  _colorTexture->bind(TMU_COLORMAP, QOpenGLTexture::ResetTextureUnit);

  _strip->bind();
  const int numStrips       =   _meshInfo.rowCount()   -1;
  const int numVertPerStrip = 2*_meshInfo.columnCount();
  for(int y = 0; y < numStrips; y++) {
    const GLuint  offset  = sizeof(GLuint)*y*numVertPerStrip;
    const GLvoid *indices = (GLvoid*)offset;
    glDrawElements(GL_TRIANGLE_STRIP, numVertPerStrip, GL_UNSIGNED_INT, indices);
  }
  _strip->release();

  _colorTexture->release();

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

  // (2) Create Strip Data ///////////////////////////////////////////////////

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

  // (3) Create Mesh Data (y-Direction) //////////////////////////////////////

  _meshYData.resize(_meshInfo.vertexCount());
  for(int x = 0; x < _meshInfo.columnCount(); x++) {
    for(int y = 0; y < _meshInfo.rowCount(); y++) {
      _meshYData[x*_meshInfo.rowCount()+y] = _meshInfo.index(x, y);
    }
  }

  // (4) Update Model Matrix /////////////////////////////////////////////////

  updateModelMatrix();

  // (5) Trigger Initialization //////////////////////////////////////////////

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

  // (2) Strip's Buffer //////////////////////////////////////////////////////

  _strip = csAllocateBuffer(_strip,
                            _stripData.constData(),
                            sizeof(GLuint)*_stripData.count(),
                            QOpenGLBuffer::IndexBuffer);

  // (3) Mesh's Buffer (y-Direction) /////////////////////////////////////////

  _meshY = csAllocateBuffer(_meshY,
                            _meshYData.constData(),
                            sizeof(GLuint)*_meshYData.size(),
                            QOpenGLBuffer::IndexBuffer);

  // (4) Color Map (Texture) /////////////////////////////////////////////////

  if( _colorTexture == 0 ) {
    _colorTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
  }
  if( !_colorTexture->isCreated() ) {
    _colorTexture->create();
  }
  _colorTexture->setMinificationFilter(QOpenGLTexture::Linear);
  _colorTexture->setMagnificationFilter(QOpenGLTexture::Linear);
  _colorTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
  _colorTexture->setData(_colorImage, QOpenGLTexture::DontGenerateMipMaps);

  // Done ////////////////////////////////////////////////////////////////////

  _initRequired = false;
}

void csSurface::updateColorImage()
{
  const int    SIZE = 128;
  const qreal from  = _paletteAxis.front();
  const qreal   to  = _paletteAxis.back();
  const qreal delta = qAbs(to-from) / (qreal(SIZE-1));

  _colorImage = QImage(SIZE, SIZE, QImage::Format_RGBA8888);
  _colorImage.fill(qRgba(0, 0, 0, 255));

  for(int x = 0; x < SIZE; x++) {
    const qreal pos = qBound(from, from+qreal(x)*delta, to);

    int r(0), g(0), b(0);
    for(int i = 0; i < _paletteAxis.size()-1; i++) {
      const qreal pos0 = _paletteAxis[i];
      const qreal pos1 = _paletteAxis[i+1];

      if( pos0 <= pos  &&  pos <= pos1 ) {
        const QColor& c0 = _palette[i];
        const QColor& c1 = _palette[i+1];

        r = qBound(0,
                   int(255.0f*csInter1D(pos, pos0, pos1, c0.redF(), c1.redF())),
                   255);
        g = qBound(0,
                   int(255.0f*csInter1D(pos, pos0, pos1, c0.greenF(), c1.greenF())),
                   255);
        b = qBound(0,
                   int(255.0f*csInter1D(pos, pos0, pos1, c0.blueF(), c1.blueF())),
                   255);
      }
    }

    _colorImage.setPixel(x, 0, qRgba(r, g, b, 255));
  }

  for(int y = 1; y < SIZE; y++) {
    memcpy(_colorImage.scanLine(y), _colorImage.constScanLine(0),
           _colorImage.bytesPerLine());
  }
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
