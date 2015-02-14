#-------------------------------------------------
#
# Project created by QtCreator 2013-12-22T17:19:13
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csPDFSearch$${TARGET_POSTFIX}

QT += core

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_PDFSEARCH_LIB QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ../include
DEPENDPATH  += ../include

INCLUDEPATH += ./include
DEPENDPATH  += ./include

LIBS += -L../../lib
LIBS += -lcsPDF$${TARGET_POSTFIX}


SOURCES += \
    src/csPdfSearch.cpp \
    src/csPdfSearchResultsModel.cpp \
    src/csPdfSearchUtil.cpp

HEADERS += \
    ../include/csPDFSearch/csPdfSearch.h \
    ../include/csPDFSearch/cspdfsearch_config.h \
    ../include/csPDFSearch/csPdfSearchResult.h \
    ../include/csPDFSearch/csPdfSearchResultsModel.h \
    ../include/csPDFSearch/csPdfSearchUtil.h \
    include/internal/config.h
