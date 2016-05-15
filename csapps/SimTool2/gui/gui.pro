#-------------------------------------------------
#
# Project created by QtCreator 2016-04-19T17:34:15
#
#-------------------------------------------------

QT       += core gui widgets

TEMPLATE = app

include(../../../global.pri)
TARGET = SimTool$${TARGET_POSTFIX}

DESTDIR = ../../../bin

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../../../cslibs/include
DEPENDPATH  +=../../../cslibs/include
LIBS += -L../../../lib -lcsQt$${TARGET_POSTFIX}
win32:PRE_TARGETDEPS += ../../../lib/csQt$${TARGET_POSTFIX}.lib

INCLUDEPATH += ../core/include
DEPENDPATH  += ../core/include
LIBS += -L../../../lib -lSimCore$${TARGET_POSTFIX}
win32:PRE_TARGETDEPS += ../../../lib/SimCore$${TARGET_POSTFIX}.lib


SOURCES += \
    src/global.cpp \
    src/main.cpp \
    src/valuedelegate.cpp \
    src/wlogger.cpp \
    src/wmainwindow.cpp \
    src/wmoduleeditor.cpp \
    src/wmodules.cpp \
    src/wsimconfigbar.cpp \
    src/wvalueswindow.cpp \
    src/wvariableeditor.cpp \
    src/wvariables.cpp

HEADERS  += \
    include/global.h \
    include/valuedelegate.h \
    include/wlogger.h \
    include/wmainwindow.h \
    include/wmoduleeditor.h \
    include/wmodules.h \
    include/wsimconfigbar.h \
    include/wvalueswindow.h \
    include/wvariableeditor.h \
    include/wvariables.h

FORMS    += \
    forms/wmainwindow.ui \
    forms/wvariableeditor.ui \
    forms/wmoduleeditor.ui \
    forms/wvalueswindow.ui \
    forms/wmodules.ui \
    forms/wvariables.ui

RESOURCES += \
    resources/resources.qrc
