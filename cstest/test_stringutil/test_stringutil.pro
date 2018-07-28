TEMPLATE = app

include(../../global.pri)
TARGET = test_stringutil$${TARGET_POSTFIX}

DESTDIR = ../../bin

QT -= core gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/csUtil/include

DEPENDPATH  += ./include
DEPENDPATH  += ../../cslibs/csUtil/include


SOURCES += \
    src/main.cpp

HEADERS += \
    ../../cslibs/csUtil/include/csUtil/csStringUtil.h
