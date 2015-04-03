#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T16:12:03
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csCore$${TARGET_POSTFIX}

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_CORE_LIB

win32:QMAKE_LFLAGS += /NODEFAULTLIB /ENTRY:DllMain


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../include
DEPENDPATH  += ../include

win32:LIBS += kernel32.lib oleaut32.lib


SOURCES += \
    src/csLimits.cpp \
    src/csString.cpp \
    src/csStringLib.cpp \
    src/csStringList.cpp

win32 {
SOURCES += \
    src/csAlloc_win32.cpp \
    src/csMain_win32.cpp
}

HEADERS += \
    include/internal/util_string.h \
    ../include/csCore/csAlloc.h \
    ../include/csCore/csChar.h \
    ../include/csCore/cscore_config.h \
    ../include/csCore/csLimits.h \
    ../include/csCore/csList.h \
    ../include/csCore/csListData.h \
    ../include/csCore/csString.h \
    ../include/csCore/csStringLib.h \
    ../include/csCore/csStringList.h \
    ../include/csCore/csUtil.h
