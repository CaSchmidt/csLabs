#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T19:03:49
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib

include(../../../../global.pri)
TARGET = signals$${TARGET_POSTFIX}

DLLDESTDIR = ../../../../bin


INCLUDEPATH += ../../core/include
DEPENDPATH  += ../../core/include

LIBS += -L../../../../lib -lSimCore$${TARGET_POSTFIX}


SOURCES += \
    src/main.cpp

HEADERS +=
