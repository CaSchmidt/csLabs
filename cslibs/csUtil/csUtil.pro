#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T18:07:27
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csUtil$${TARGET_POSTFIX}

QT -= gui

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_UTIL_LIB


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../include
DEPENDPATH  += ../include


SOURCES += \
    src/csCharConv.cpp

HEADERS += \
    ../include/csUtil/csCharConv.h \
    ../include/csUtil/csConvert.h \
    ../include/csUtil/csEndian.h \
    ../include/csUtil/csFlags.h \
    ../include/csUtil/csPointer.h \
    ../include/csUtil/csutil_config.h
