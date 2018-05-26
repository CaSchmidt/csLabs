cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(csUtil)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

# Static Build?
option(BUILD_SHARED_LIBS "Build shared library" ON)

if(NOT BUILD_SHARED_LIBS)
  set(CS_BUILD_UTIL_STATIC ON)
endif()

# config.h
configure_file(include/config/csutil_config.h.in
  csUtil/csutil_config.h @ONLY
  )

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
  include/csUtil/csNamespace.h
  include/csUtil/csPointer.h
  include/csUtil/csRawConverter.h
  include/csUtil/csTextConverter.h
  include/internal/csutil_internal.h
  )

set(csUtil_SOURCES
  src/csCharConv-fp.cpp
  src/csCharConv.cpp
  src/csFileIO.cpp
  src/csFormat.cpp
  src/csILogger.cpp
  src/csLogger.cpp
  src/csTextConverter.cpp
  )

### Target ###################################################################

add_library(csUtil
  ${csUtil_HEADERS} ${csUtil_SOURCES}
  ${trdparty_dblcnv_FILES}
  ${trdparty_icu_FILES}
  )

format_output_name(csUtil "csUtil")

target_compile_definitions(csUtil PRIVATE CS_BUILD_UTIL_LIB)

target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
  target_compile_definitions(csUtil PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# ICU
target_compile_definitions(csUtil PRIVATE U_COMMON_IMPLEMENTATION U_STATIC_IMPLEMENTATION)
target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/icu-ucnv/src)

install(TARGETS csUtil
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

install(DIRECTORY
  include/ DESTINATION include
  PATTERN "config" EXCLUDE
  PATTERN "internal" EXCLUDE
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil/csutil_config.h DESTINATION include/csUtil
  )
