#-------------------------------------------------
#
# Project created by QtCreator 2013-12-22T17:19:13
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csPDF$${TARGET_POSTFIX}

QT += core gui concurrent

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_PDF_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ../include
DEPENDPATH  += ../include

INCLUDEPATH += ./include
DEPENDPATH  += ./include

LIBS += -lpdf$${TARGET_ARCH}


SOURCES += \
    src/csPdfDocument.cpp \
    src/csPdfPage.cpp \
    src/csPdfContentsNode.cpp \
    src/fz_util.cpp \
    src/csPdfContentsModel.cpp \
    src/fz_render.cpp \
    src/fz_pathext.cpp

HEADERS += \
    ../include/csPDF/cspdf_config.h \
    ../include/csPDF/csPdfDocument.h \
    ../include/csPDF/csPdfPage.h \
    ../include/csPDF/csPdfLink.h \
    ../include/csPDF/csPdfContentsNode.h \
    ../include/csPDF/csPdfContentsModel.h \
    ../include/csPDF/csPdfText.h \
    ../include/csPDF/csPdfUtil.h \
    ../include/csPDF/csPdfTextPage.h \
    include/internal/config.h \
    include/internal/csPdfDocumentImpl.h \
    include/internal/csPdfPageImpl.h \
    include/internal/fz_pathext.h \
    include/internal/fz_render.h \
    include/internal/fz_util.h
