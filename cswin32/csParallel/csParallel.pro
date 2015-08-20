#-------------------------------------------------
#
# Project created by QtCreator 2015-08-20T18:50:16
#
#-------------------------------------------------

TEMPLATE = app

include(../../global.pri)
TARGET = csParallel$${TARGET_POSTFIX}

QT       -= core gui widgets

CONFIG   -= console
CONFIG   -= app_bundle

QMAKE_CFLAGS_DEBUG   += /MTd
QMAKE_CFLAGS_RELEASE += /MT

QMAKE_CXXFLAGS_DEBUG   += /MTd
QMAKE_CXXFLAGS_RELEASE += /MT

QMAKE_LFLAGS += /ENTRY:wWinMain

DESTDIR = ../../bin


INCLUDEPATH += ../../cslibs/include
INCLUDEPATH += ./include
INCLUDEPATH += ./resource

DEPENDPATH += ../../cslibs/include
DEPENDPATH += ./include
DEPENDPATH += ./resource

LIBS += -L../../lib
LIBS += -lcsCore2$${TARGET_POSTFIX}
LIBS += comctl32.lib user32.lib


SOURCES += \
    src/env.cpp \
    src/gui.cpp \
    src/main.cpp

HEADERS += \
    include/env.h \
    include/gui.h \
    resource/resource.h

RC_FILE += \
    resource/csParallel.rc
