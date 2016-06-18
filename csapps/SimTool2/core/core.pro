#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T16:42:11
#
#-------------------------------------------------

QT       -= gui

TEMPLATE = lib

include(../../../global.pri)
TARGET = SimCore$${TARGET_POSTFIX}

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../bin

DEFINES += SIM_BUILD_CORE_LIB
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII


INCLUDEPATH += ./include
DEPENDPATH  += ./include


SOURCES += \
    src/ISimValue.cpp \
    src/ISimVariableXfer.cpp \
    src/sim_mod.cpp \
    src/SimContext.cpp \
    src/SimControl.cpp \
    src/SimDb.cpp \
    src/SimEnvironment.cpp \
    src/SimModule.cpp \
    src/SimModuleRunner.cpp \
    src/SimModulesModel.cpp \
    src/SimSimulator.cpp \
    src/SimUtil.cpp \
    src/SimValuesModel.cpp \
    src/SimVariable.cpp \
    src/SimVariablesModel.cpp \
    src/SimDataSeries.cpp \
    src/SimDataLogger.cpp

HEADERS += \
    include/SimCore/ISimLogger.h \
    include/SimCore/ISimValue.h \
    include/SimCore/ISimVariableXfer.h \
    include/SimCore/Sim.h \
    include/SimCore/sim_mod.h \
    include/SimCore/SimConfig.h \
    include/SimCore/SimContext.h \
    include/SimCore/SimControl.h \
    include/SimCore/simcore_config.h \
    include/SimCore/SimDataTypeId.h \
    include/SimCore/SimDb.h \
    include/SimCore/SimDbStore.h \
    include/SimCore/SimEnvironment.h \
    include/SimCore/SimModule.h \
    include/SimCore/SimModuleRunner.h \
    include/SimCore/SimModulesModel.h \
    include/SimCore/SimSimulator.h \
    include/SimCore/SimUtil.h \
    include/SimCore/SimValuesModel.h \
    include/SimCore/SimVariable.h \
    include/SimCore/SimVariablesModel.h \
    include/SimCore/SimDataSeries.h \
    include/SimCore/priv/SimDataSeriesPriv.h \
    include/SimCore/SimDataLogger.h
