#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T16:12:03
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csQt$${TARGET_POSTFIX}

QT += core gui widgets

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_QT_LIB


INCLUDEPATH += ../include
DEPENDPATH  += ../include


SOURCES += \
    src/csTableCopier.cpp \
    src/csItemSearcher.cpp \
    src/csImageTip.cpp \
    src/csFontResizer.cpp \
    src/csColumnResizer.cpp \
    src/csQtUtil.cpp \
    src/csSubstringHighlighter.cpp \
    src/csHighlightingDelegate.cpp \
    src/csTrie.cpp

HEADERS += \
    ../include/csQt/csTableCopier.h \
    ../include/csQt/csItemSearcher.h \
    ../include/csQt/csImageTip.h \
    ../include/csQt/csFontResizer.h \
    ../include/csQt/csColumnResizer.h \
    ../include/csQt/csqt_config.h \
    ../include/csQt/csQtUtil.h \
    ../include/csQt/csSubstringHighlighter.h \
    ../include/csQt/csHighlightingDelegate.h \
    ../include/csQt/csTrie.h \
    ../include/csQt/csNamespace.h
