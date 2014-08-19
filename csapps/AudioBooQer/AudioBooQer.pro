#-------------------------------------------------
#
# Project created by QtCreator 2014-08-10T10:57:59
#
#-------------------------------------------------

QT       += core gui widgets multimedia concurrent

include(../../global.pri)
TARGET = AudioBooQer$${TARGET_POSTFIX}
TEMPLATE = app

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

DESTDIR = ../../bin


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH += ./include
DEPENDPATH += ../../cslibs/include

LIBS += -L../../lib
LIBS += -lcsQt$${TARGET_POSTFIX}


SOURCES += \
    src/main.cpp \
    src/chapter.cpp \
    src/chaptermodel.cpp \
    src/wmainwindow.cpp \
    src/waudioplayer.cpp \
    src/job.cpp \
    src/wjobinfo.cpp

HEADERS  += \
    include/chapter.h \
    include/chaptermodel.h \
    include/wmainwindow.h \
    include/waudioplayer.h \
    include/job.h \
    include/wjobinfo.h

FORMS    += \
    forms/wmainwindow.ui \
    forms/waudioplayer.ui \
    forms/wjobinfo.ui

RESOURCES +=
