TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./include
DEPENDPATH  += ./include

SOURCES += \
  src/main.cpp

HEADERS += \
  include/FundamentalAllocator.h
