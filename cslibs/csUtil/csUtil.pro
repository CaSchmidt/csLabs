#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T18:07:27
#
#-------------------------------------------------

TEMPLATE = lib

include(../../global.pri)
TARGET = csUtil$${TARGET_POSTFIX}

QT -= core gui

DESTDIR    = ../../lib
DLLDESTDIR = ../../bin

DEFINES += CS_BUILD_UTIL_LIB

# ICU
DEFINES += U_COMMON_IMPLEMENTATION U_STATIC_IMPLEMENTATION


# Dependencies ###############################################################

INCLUDEPATH += ./include
DEPENDPATH  += ./include

INCLUDEPATH += ../include
DEPENDPATH  += ../include

INCLUDEPATH += ./3rdparty/icu-ucnv/src

LIBS += advapi32.lib


# Project ####################################################################

SOURCES += \
    3rdparty/double-conversion/src/bignum-dtoa.cc \
    3rdparty/double-conversion/src/bignum.cc \
    3rdparty/double-conversion/src/cached-powers.cc \
    3rdparty/double-conversion/src/diy-fp.cc \
    3rdparty/double-conversion/src/double-conversion.cc \
    3rdparty/double-conversion/src/fast-dtoa.cc \
    3rdparty/double-conversion/src/fixed-dtoa.cc \
    3rdparty/double-conversion/src/strtod.cc \
    3rdparty/icu-ucnv/src-dat/icudt61l_dat.c \
    3rdparty/icu-ucnv/src/appendable.cpp \
    3rdparty/icu-ucnv/src/charstr.cpp \
    3rdparty/icu-ucnv/src/cmemory.cpp \
    3rdparty/icu-ucnv/src/cstring.cpp \
    3rdparty/icu-ucnv/src/locid.cpp \
    3rdparty/icu-ucnv/src/locmap.cpp \
    3rdparty/icu-ucnv/src/putil.cpp \
    3rdparty/icu-ucnv/src/resource.cpp \
    3rdparty/icu-ucnv/src/stringpiece.cpp \
    3rdparty/icu-ucnv/src/uarrsort.cpp \
    3rdparty/icu-ucnv/src/ucln_cmn.cpp \
    3rdparty/icu-ucnv/src/ucmndata.cpp \
    3rdparty/icu-ucnv/src/ucnv.cpp \
    3rdparty/icu-ucnv/src/ucnv2022.cpp \
    3rdparty/icu-ucnv/src/ucnv_bld.cpp \
    3rdparty/icu-ucnv/src/ucnv_cb.cpp \
    3rdparty/icu-ucnv/src/ucnv_cnv.cpp \
    3rdparty/icu-ucnv/src/ucnv_ct.cpp \
    3rdparty/icu-ucnv/src/ucnv_err.cpp \
    3rdparty/icu-ucnv/src/ucnv_ext.cpp \
    3rdparty/icu-ucnv/src/ucnv_io.cpp \
    3rdparty/icu-ucnv/src/ucnv_lmb.cpp \
    3rdparty/icu-ucnv/src/ucnv_u16.cpp \
    3rdparty/icu-ucnv/src/ucnv_u32.cpp \
    3rdparty/icu-ucnv/src/ucnv_u7.cpp \
    3rdparty/icu-ucnv/src/ucnv_u8.cpp \
    3rdparty/icu-ucnv/src/ucnvbocu.cpp \
    3rdparty/icu-ucnv/src/ucnvhz.cpp \
    3rdparty/icu-ucnv/src/ucnvisci.cpp \
    3rdparty/icu-ucnv/src/ucnvlat1.cpp \
    3rdparty/icu-ucnv/src/ucnvmbcs.cpp \
    3rdparty/icu-ucnv/src/ucnvscsu.cpp \
    3rdparty/icu-ucnv/src/ucol_swp.cpp \
    3rdparty/icu-ucnv/src/udata.cpp \
    3rdparty/icu-ucnv/src/udatamem.cpp \
    3rdparty/icu-ucnv/src/udataswp.cpp \
    3rdparty/icu-ucnv/src/uenum.cpp \
    3rdparty/icu-ucnv/src/uhash.cpp \
    3rdparty/icu-ucnv/src/uinit.cpp \
    3rdparty/icu-ucnv/src/uinvchar.cpp \
    3rdparty/icu-ucnv/src/uloc.cpp \
    3rdparty/icu-ucnv/src/uloc_keytype.cpp \
    3rdparty/icu-ucnv/src/uloc_tag.cpp \
    3rdparty/icu-ucnv/src/umapfile.cpp \
    3rdparty/icu-ucnv/src/umath.cpp \
    3rdparty/icu-ucnv/src/umutex.cpp \
    3rdparty/icu-ucnv/src/unistr.cpp \
    3rdparty/icu-ucnv/src/uobject.cpp \
    3rdparty/icu-ucnv/src/uresbund.cpp \
    3rdparty/icu-ucnv/src/uresdata.cpp \
    3rdparty/icu-ucnv/src/ustr_cnv.cpp \
    3rdparty/icu-ucnv/src/ustrenum.cpp \
    3rdparty/icu-ucnv/src/ustrfmt.cpp \
    3rdparty/icu-ucnv/src/ustring.cpp \
    3rdparty/icu-ucnv/src/ustrtrns.cpp \
    3rdparty/icu-ucnv/src/utf_impl.cpp \
    3rdparty/icu-ucnv/src/utrace.cpp \
    3rdparty/icu-ucnv/src/utrie2.cpp \
    3rdparty/icu-ucnv/src/utypes.cpp \
    3rdparty/icu-ucnv/src/uvector.cpp \
    3rdparty/icu-ucnv/src/wintz.cpp \
    src/csCharConv.cpp \
    src/csCharConv-fp.cpp \
    src/csFormat.cpp \
    src/csLogger.cpp \
    src/csTextConverter.cpp \
    src/csFileIO.cpp

HEADERS += \
    ../include/csUtil/csCharConv.h \
    ../include/csUtil/csEndian.h \
    ../include/csUtil/csFlags.h \
    ../include/csUtil/csPointer.h \
    ../include/csUtil/csutil_config.h \
    ../include/csUtil/csFormat.h \
    ../include/csUtil/csILogger.h \
    ../include/csUtil/csLogger.h \
    ../include/csUtil/csRawConverter.h \
    ../include/csUtil/csTextConverter.h \
    ../include/csUtil/csFileIO.h
