TEMPLATE = app

include(../../global.pri)
TARGET = test_convert$${TARGET_POSTFIX}

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
    src/main.cpp

HEADERS += \
    ../../cslibs/include/csUtil/csConvert.h
