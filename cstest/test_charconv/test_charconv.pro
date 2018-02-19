TEMPLATE = app

include(../../global.pri)
TARGET = test_charconvs$${TARGET_POSTFIX}

DESTDIR = ../../bin

QT -= gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH  += ./include
DEPENDPATH  += ../../cslibs/include

LIBS += -L../../lib -lcsUtil$${TARGET_POSTFIX}
win32:PRE_TARGETDEPS += ../../lib/csUtil$${TARGET_POSTFIX}.lib


SOURCES += \
    src/main.cpp

HEADERS +=
