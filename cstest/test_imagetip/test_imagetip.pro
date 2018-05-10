#-------------------------------------------------
#
# Project created by QtCreator 2017-06-22T16:39:46
#
#-------------------------------------------------

TEMPLATE = app

QT       += core gui widgets

include(../../global.pri)
TARGET = test_imagetip$${TARGET_POSTFIX}

DESTDIR = ../../bin

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../../cslibs/include
DEPENDPATH  += ../../cslibs/include

LIBS += -L../../lib -lcsQt$${TARGET_POSTFIX}

win32::PRE_TARGETDEPS += ../../lib/csQt$${TARGET_POSTFIX}.lib


SOURCES += \
    src/main.cpp \
    src/wmainwindow.cpp

HEADERS  += \
    include/wmainwindow.h

FORMS    += \
    forms/wmainwindow.ui
