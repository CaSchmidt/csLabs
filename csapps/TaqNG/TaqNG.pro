#-------------------------------------------------
#
# Project created by QtCreator 2014-05-02T18:00:44
#
#-------------------------------------------------

QT       += core gui widgets concurrent

TARGET = TaqNG
TEMPLATE = app

CONFIG(debug, debug|release): TARGET = $${TARGET}_debug

DESTDIR = ../../bin


INCLUDEPATH += ./include
INCLUDEPATH += ../../cslibs/include

DEPENDPATH += ./include
DEPENDPATH += ../../cslibs/include

LIBS += -L../../lib
CONFIG(debug,   debug|release): LIBS += -lcsQt_debug
CONFIG(release, debug|release): LIBS += -lcsQt
LIBS += -lid3-x64


SOURCES += \
    src/id3v2.cpp \
    src/main.cpp \
    src/mp3filesmodel.cpp \
    src/util.cpp \
    src/wmainwindow.cpp \
    src/wmainwindow_apic.cpp \
    src/wmainwindow_id3v2.cpp \
    src/wmainwindow_multi.cpp \
    src/wmainwindow_single.cpp \
    src/wmainwindow_strip.cpp \
    src/wmainwindow_tag.cpp

HEADERS  += \
    include/taq/id3v2.h \
    include/taq/mp3filesmodel.h \
    include/taq/myframesmodel.h \
    include/taq/mytag.h \
    include/taq/progresswatcher.hpp \
    include/taq/util.h \
    include/taq/wmainwindow.h

FORMS    += \
    forms/wmainwindow.ui
