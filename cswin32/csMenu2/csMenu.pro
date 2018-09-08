#-------------------------------------------------
#
# Project created by QtCreator 2015-03-27T17:02:18
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csMenu$${TARGET_POSTFIX}

QT       -= core gui widgets

DLLDESTDIR = ../../bin


DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ./include
INCLUDEPATH += ./resource

DEPENDPATH += ./include
DEPENDPATH += ./resource

LIBS += advapi32.lib gdi32.lib mpr.lib ole32.lib shell32.lib shlwapi.lib user32.lib


SOURCES += \
    src/main.cpp \
    src/csMenu.cpp \
    src/csMenuFactory.cpp \
    src/clipboard.cpp \
    src/menu.cpp \
    src/command.cpp \
    src/reg.cpp \
    src/util.cpp \
    src/csCore2/csFileUtil_win32.cpp \
    src/csCore2/csProcess_win32.cpp \
    src/worker/worker.cpp \
    src/worker/batch_work.cpp \
    src/worker/sequential_work.cpp \
    src/worker/parallel_work.cpp

HEADERS += \
    include/csMenu.h \
    include/csMenuFactory.h \
    include/clipboard.h \
    include/menu.h \
    include/command.h \
    include/reg.h \
    include/util.hpp \
    resource/resource.h \
    include/csCore2/csWString.h \
    include/csCore2/csFileUtil.h \
    include/csCore2/csProcess.h \
    include/worker/worker.h

DEF_FILE = csMenu.def

RC_FILE = ./resource/csMenu.rc

DISTFILES += \
    csMenu.def \
    resource/csMenu.rc
