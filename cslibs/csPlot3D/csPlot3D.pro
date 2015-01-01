#-------------------------------------------------
#
# Project created by QtCreator 2015-01-01T16:09:01
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csPlot3D$${TARGET_POSTFIX}

QT += core gui widgets

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_PLOT3D_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ../include
DEPENDPATH  += ../include


SOURCES += \
    src/csAxisLabel.cpp \
    src/csCoordinateBox.cpp \
    src/csOpenGLUtil.cpp \
    src/csPlot3D.cpp \
    src/csSurface.cpp

HEADERS += \
    ../include/csPlot3D/csAxisLabel.h \
    ../include/csPlot3D/csCoordinateBox.h \
    ../include/csPlot3D/csEdge.h \
    ../include/csPlot3D/csMath.h \
    ../include/csPlot3D/csMeshInfo.h \
    ../include/csPlot3D/csOpenGLUtil.h \
    ../include/csPlot3D/csPlot3D.h \
    ../include/csPlot3D/csSurface.h \
    ../include/csPlot3D/csPlot3DUtil.h \
    ../include/csPlot3D/csplot3d_config.h

RESOURCES += \
    resources/csPlot3D.qrc

DISTFILES += \
    resources/shader/csplot3d_f.glsl \
    resources/shader/csplot3d_v.glsl
