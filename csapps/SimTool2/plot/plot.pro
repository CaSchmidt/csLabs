#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T16:42:11
#
#-------------------------------------------------

QT += core gui widgets

TEMPLATE = lib

include(../../../global.pri)
TARGET = SimPlot$${TARGET_POSTFIX}

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../bin

DEFINES += SIM_BUILD_PLOT_LIB
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ./include
DEPENDPATH  += ./include


SOURCES += \
    src/AxisLabel.cpp \
    src/IAxisElement.cpp \
    src/IPlotElement.cpp \
    src/IPlotImplementation.cpp \
    src/ISimPlotSeriesData.cpp \
    src/ITitleElement.cpp \
    src/Mapping.cpp \
    src/Scale.cpp \
    src/Scope.cpp \
    src/ScopeRow.cpp \
    src/Series.cpp \
    src/SeriesStore.cpp \
    src/SimPlotWidget.cpp \
    src/SimRange.cpp \
    src/SimTheme.cpp \
    src/SinglePlotImpl.cpp \
    src/XAxis.cpp \
    src/XTitle.cpp \
    src/YAxis.cpp \
    src/YTitle.cpp

HEADERS += \
    include/SimPlot/ISimPlotSeriesData.h \
    include/SimPlot/simplot_config.h \
    include/SimPlot/SimPlotWidget.h \
    include/SimPlot/SimRange.h \
    include/SimPlot/SimTheme.h \
    include/internal/AxisLabel.h \
    include/internal/IAxisElement.h \
    include/internal/IPlotElement.h \
    include/internal/IPlotImplementation.h \
    include/internal/ITitleElement.h \
    include/internal/Layout.h \
    include/internal/Mapping.h \
    include/internal/Scale.h \
    include/internal/Scope.h \
    include/internal/ScopeRow.h \
    include/internal/Series.h \
    include/internal/SeriesStore.h \
    include/internal/SinglePlotImpl.h \
    include/internal/XAxis.h \
    include/internal/XTitle.h \
    include/internal/YAxis.h \
    include/internal/YTitle.h
