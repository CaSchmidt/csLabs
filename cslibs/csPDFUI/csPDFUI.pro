#-------------------------------------------------
#
# Project created by QtCreator 2014-06-29T10:44:45
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csPDFUI$${TARGET_POSTFIX}

QT       += core gui widgets

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_PDFUI_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ../include
DEPENDPATH  += ../include

LIBS += -L../../lib
LIBS += -lcsPDFium$${TARGET_POSTFIX} -lcsPDFSearch$${TARGET_POSTFIX} -lcsQt$${TARGET_POSTFIX}


SOURCES += \
    src/csPdfUiDocumentView.cpp \
    src/csPdfUiSearchWidget.cpp \
    src/csPdfUiTocWidget.cpp

HEADERS += \
    ../include/csPDFUI/cspdfui_config.h \
    ../include/csPDFUI/csPdfUiDocumentView.h \
    ../include/csPDFUI/csPdfUiSearchWidget.h \
    ../include/csPDFUI/csPdfUiTocWidget.h

FORMS += \
    forms/csPdfUiSearchWidget.ui \
    forms/csPdfUiTocWidget.ui
