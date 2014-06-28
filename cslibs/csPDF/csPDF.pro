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

DEFINES += CS_BUILD_PDF_LIB


INCLUDEPATH += ../include
DEPENDPATH  += ../include

LIBS += -lpdf-x64


SOURCES += \
    src/csPdfDocument.cpp \
    src/csPdfPage.cpp \
    src/csPdfContentsNode.cpp \
    src/fz_util.cpp \
    src/csPdfContentsModel.cpp \
    src/csPdfUtil.cpp \
    src/csPdfSearch.cpp \
    src/csPdfSearchResultsModel.cpp \
    src/fz_render.cpp

HEADERS += \
    ../include/csPDF/cspdf_config.h \
    ../include/csPDF/csPdfDocument.h \
    ../include/csPDF/internal/csPdfDocumentImpl.h \
    ../include/csPDF/internal/csPdfPageImpl.h \
    ../include/csPDF/csPdfPage.h \
    ../include/csPDF/internal/fz_util.h \
    ../include/csPDF/csPdfLink.h \
    ../include/csPDF/csPdfContentsNode.h \
    ../include/csPDF/csPdfContentsModel.h \
    ../include/csPDF/csPdfText.h \
    ../include/csPDF/csPdfUtil.h \
    ../include/csPDF/csPdfSearch.h \
    ../include/csPDF/internal/config.h \
    ../include/csPDF/csPdfSearchResult.h \
    ../include/csPDF/csPdfSearchResultsModel.h \
    ../include/csPDF/csPdfTextPage.h \
    ../include/csPDF/internal/fz_render.h
