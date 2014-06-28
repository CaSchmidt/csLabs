#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T19:15:36
#
#-------------------------------------------------

TEMPLATE = app

include(../../global.pri)
TARGET = csPDF$${TARGET_POSTFIX}

QT += core gui widgets

DESTDIR = ../../bin


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH += ./include
DEPENDPATH += ../../cslibs/include

LIBS += -L../../lib
LIBS += -lcsQt$${TARGET_POSTFIX} -lcsPDF$${TARGET_POSTFIX}


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
