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
    3rdparty/double-conversion/double-conversion.cc \
    3rdparty/double-conversion/bignum-dtoa.cc \
    3rdparty/double-conversion/fast-dtoa.cc \
    3rdparty/double-conversion/fixed-dtoa.cc \
    3rdparty/double-conversion/strtod.cc \
    3rdparty/double-conversion/bignum.cc \
    3rdparty/double-conversion/cached-powers.cc \
    3rdparty/double-conversion/diy-fp.cc \
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
