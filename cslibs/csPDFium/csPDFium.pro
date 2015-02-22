#-------------------------------------------------
#
# Project created by QtCreator 2013-12-22T17:19:13
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csPDFium$${TARGET_POSTFIX}

QT += core gui

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_PDFIUM_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ../include
DEPENDPATH  += ../include

INCLUDEPATH += ./include
DEPENDPATH  += ./include

LIBS += -lPDFium$${TARGET_ARCH}


SOURCES += \
    src/csPDFium.cpp \
    src/csPDFiumContentsModel.cpp \
    src/csPDFiumContentsNode.cpp \
    src/csPDFiumDocument.cpp \
    src/csPDFiumPage.cpp \
    src/util_text.cpp

HEADERS += \
    ../include/csPDFium/cspdfium_config.h \
    ../include/csPDFium/csPDFium.h \
    ../include/csPDFium/csPDFiumUtil.h \
    ../include/csPDFium/csPDFiumContentsModel.h \
    ../include/csPDFium/csPDFiumContentsNode.h \
    ../include/csPDFium/csPDFiumLink.h \
    ../include/csPDFium/csPDFiumText.h \
    ../include/csPDFium/csPDFiumTextPage.h \
    ../include/csPDFium/csPDFiumPage.h \
    ../include/csPDFium/csPDFiumDocument.h \
    include/internal/csPDFiumDocumentImpl.h \
    include/internal/csPDFiumPageImpl.h \
    include/internal/fpdf_util.h \
    include/internal/config.h
