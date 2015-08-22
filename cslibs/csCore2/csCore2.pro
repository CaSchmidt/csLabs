#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T16:12:03
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csCore2$${TARGET_POSTFIX}

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_CORE2_LIB


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../include
DEPENDPATH  += ../include

win32:LIBS += shell32.lib


SOURCES += \
    src/csAlphaNum.cpp \
    src/csChar.cpp \
    src/csLimits.cpp \
    src/csString.cpp \
    src/csStringLib.cpp \
    src/csStringList.cpp

win32 {
SOURCES += \
    src/csFile_win32.cpp \
    src/csProcess_win32.cpp
}

HEADERS += \
    ../include/csCore2/csAlphaNum.h \
    ../include/csCore2/csChar.h \
    ../include/csCore2/cscore2_config.h \
    ../include/csCore2/cscore2_features.h \
    ../include/csCore2/csLimits.h \
    ../include/csCore2/csString.h \
    ../include/csCore2/csStringLib.h \
    ../include/csCore2/csStringList.h \
    ../include/csCore2/csUtil.h \
    ../include/csCore2/csFile.h \
    ../include/csCore2/csProcess.h
