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
LIBS += -lcsQt$${TARGET_POSTFIX}
LIBS += -lcsPDFium$${TARGET_POSTFIX}
LIBS += -lcsPDFUI$${TARGET_POSTFIX}


SOURCES += \
    src/main.cpp \
    src/wmainwindow.cpp \
    src/wquicksearch.cpp

HEADERS  += \
    include/wmainwindow.h \
    include/wquicksearch.h

FORMS    += \
    forms/wmainwindow.ui \
    forms/wquicksearch.ui
