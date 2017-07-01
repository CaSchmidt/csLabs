#-------------------------------------------------
#
# Project created by QtCreator 2017-06-22T16:39:46
#
#-------------------------------------------------

TEMPLATE = app

QT       += core gui widgets

include(../../global.pri)
TARGET = test_plot$${TARGET_POSTFIX}

DESTDIR = ../../bin

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../../csapps/SimTool2/plot/include
DEPENDPATH  += ../../csapps/SimTool2/plot/include

LIBS += -L../../lib -lSimPlot$${TARGET_POSTFIX}

win32::PRE_TARGETDEPS += ../../lib/SimPlot$${TARGET_POSTFIX}.lib


SOURCES += \
    src/main.cpp \
    src/MySignal.cpp

HEADERS  += \
    include/MySignal.h

FORMS    +=
