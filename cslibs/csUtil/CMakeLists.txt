cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(csUtil)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_SHARED_LIBS "Build shared library" ON)

### 3rd Party ################################################################

include(3rdparty/double-conversion/dblcnv_files.cmake)
include(3rdparty/icu-ucnv/icu_files.cmake)

### csUtil ###################################################################

set(csUtil_HEADERS
  include/csUtil/csCharConv.h
  include/csUtil/csEndian.h
  include/csUtil/csFileIO.h
  include/csUtil/csFlags.h
  include/csUtil/csFormat.h
  include/csUtil/csILogger.h
  include/csUtil/csLogger.h
  include/csUtil/csPointer.h
  include/csUtil/csRawConverter.h
  include/csUtil/csTextConverter.h
  include/csUtil/csutil_config.h
  include/internal/csutil_internal.h
  )

set(csUtil_SOURCES
  src/csCharConv-fp.cpp
  src/csCharConv.cpp
  src/csFileIO.cpp
  src/csFormat.cpp
  src/csLogger.cpp
  src/csTextConverter.cpp
  )

### Target ###################################################################

add_library(csUtil
  ${csUtil_HEADERS} ${csUtil_SOURCES}
  ${trdparty_dblcnv_FILES}
  ${trdparty_icu_FILES}
  )

target_compile_definitions(csUtil PRIVATE CS_BUILD_UTIL_LIB)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(csUtil PRIVATE CS_BUILD_UTIL_STATIC)
endif()

target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
  target_compile_definitions(csUtil PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ICU
target_compile_definitions(csUtil PRIVATE U_COMMON_IMPLEMENTATION U_STATIC_IMPLEMENTATION)
target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/icu-ucnv/src)
