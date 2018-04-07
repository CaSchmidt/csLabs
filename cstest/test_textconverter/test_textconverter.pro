TEMPLATE = app

include(../../global.pri)
TARGET = test_textconverter$${TARGET_POSTFIX}

DESTDIR = ../../bin

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


# Dependencies ###############################################################

INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../../cslibs/include
DEPENDPATH  += ../../cslibs/include

LIBS += -L../../lib -lcsUtil$${TARGET_POSTFIX}
win32:PRE_TARGETDEPS += ../../lib/csUtil$${TARGET_POSTFIX}.lib


# Project ####################################################################

SOURCES += \
    src/main.cpp
