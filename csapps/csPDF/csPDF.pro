#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T19:15:36
#
#-------------------------------------------------

QT += core gui widgets

TARGET = csPDF
TEMPLATE = app

CONFIG(debug, debug|release): TARGET = $${TARGET}_debug
# CONFIG += console

DESTDIR = ../../bin


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH += ./include
DEPENDPATH += ../../cslibs/include

LIBS += -L../../lib
CONFIG(debug, debug|release):   LIBS += -lcsPDF_debug
CONFIG(release, debug|release): LIBS += -lcsPDF
CONFIG(debug, debug|release):   LIBS += -lcsQt_debug
CONFIG(release, debug|release): LIBS += -lcsQt


SOURCES += \
    src/main.cpp \
    src/wmainwindow.cpp \
    src/wpdfview.cpp \
    src/wquicksearch.cpp \
    src/wpdfsearch.cpp \
    src/wpdfcontents.cpp

HEADERS  += \
    include/wmainwindow.h \
    include/wpdfview.h \
    include/wquicksearch.h \
    include/wpdfsearch.h \
    include/wpdfcontents.h

FORMS    += \
    forms/wmainwindow.ui \
    forms/wquicksearch.ui \
    forms/wpdfsearch.ui \
    forms/wpdfcontents.ui
