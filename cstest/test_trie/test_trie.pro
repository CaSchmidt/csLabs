TEMPLATE = app

include(../../global.pri)
TARGET = test_trie$${TARGET_POSTFIX}

DESTDIR = ../../bin

QT += core

CONFIG += console


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH  += ./include
DEPENDPATH  += ../../cslibs/include

LIBS += -L../../lib -lcsQt$${TARGET_POSTFIX}
win32:PRE_TARGETDEPS += ../../lib/csQt$${TARGET_POSTFIX}.lib


SOURCES += \
    src/main.cpp

HEADERS +=
