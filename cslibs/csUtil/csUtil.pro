#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T18:07:27
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csUtil$${TARGET_POSTFIX}

QT -= core gui

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_UTIL_LIB


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../include
DEPENDPATH  += ../include


SOURCES += \
    src/csCharConv.cpp \
    3rdparty/double-conversion/src/double-conversion.cc \
    3rdparty/double-conversion/src/bignum-dtoa.cc \
    3rdparty/double-conversion/src/fast-dtoa.cc \
    3rdparty/double-conversion/src/fixed-dtoa.cc \
    3rdparty/double-conversion/src/strtod.cc \
    3rdparty/double-conversion/src/bignum.cc \
    3rdparty/double-conversion/src/cached-powers.cc \
    3rdparty/double-conversion/src/diy-fp.cc \
    src/csCharConv-fp.cpp \
    src/csFormat.cpp \
    src/csLogger.cpp

HEADERS += \
    ../include/csUtil/csCharConv.h \
    ../include/csUtil/csEndian.h \
    ../include/csUtil/csFlags.h \
    ../include/csUtil/csPointer.h \
    ../include/csUtil/csutil_config.h \
    ../include/csUtil/csFormat.h \
    ../include/csUtil/csILogger.h \
    ../include/csUtil/csLogger.h \
    ../include/csUtil/csRawConverter.h
