#-------------------------------------------------
#
# Project created by QtCreator 2015-03-27T17:02:18
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csMenu$${TARGET_POSTFIX}

QT       -= core gui widgets

QMAKE_CFLAGS_DEBUG   += /MTd
QMAKE_CFLAGS_RELEASE += /MT

QMAKE_CXXFLAGS_DEBUG   += /MTd
QMAKE_CXXFLAGS_RELEASE += /MT

DESTDIR = ../../bin


INCLUDEPATH += ./include
INCLUDEPATH += ./resource
INCLUDEPATH += ../../cslibs/include

DEPENDPATH += ./include
DEPENDPATH += ./resource
DEPENDPATH += ../../cslibs/include

LIBS += -L../../lib -lcsCore$${TARGET_POSTFIX}
LIBS += advapi32.lib gdi32.lib mpr.lib ole32.lib shell32.lib shlwapi.lib user32.lib


SOURCES += \
    src/main.cpp \
    src/csMenu.cpp \
    src/csMenuFactory.cpp \
    src/clipboard.cpp \
    src/menu.cpp \
    src/command.cpp \
    src/reg.cpp \
    src/util.cpp

HEADERS += \
    include/csMenu.h \
    include/csMenuFactory.h \
    include/clipboard.h \
    include/menu.h \
    include/command.h \
    include/reg.h \
    include/util.hpp \
    resource/resource.h

DEF_FILE = csMenu.def

RC_FILE = ./resource/csMenu.rc

DISTFILES += \
    csMenu.def \
    resource/csMenu.rc
