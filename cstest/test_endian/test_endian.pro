TEMPLATE = app

include(../../global.pri)
TARGET = test_endian$${TARGET_POSTFIX}

DESTDIR = ../../bin

QT -= core gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH  += ./include
DEPENDPATH  += ../../cslibs/include


SOURCES += \
    src/data.cpp \
    src/main.cpp

HEADERS += \
    include/data.h \
    ../../cslibs/include/csUtil/csEndian.h
