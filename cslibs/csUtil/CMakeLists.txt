cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(csUtil)

# Custom CMake utilities
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FormatOutputName)

# Static Build?
option(BUILD_SHARED_LIBS "Build shared library" ON)

if(NOT BUILD_SHARED_LIBS)
  set(CS_BUILD_UTIL_STATIC ON)
endif()

# config.h
configure_file(include/config/csutil_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/include/csUtil/csutil_config.h @ONLY
  )

# C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

### 3rd Party ################################################################

add_subdirectory(3rdparty/double-conversion)
add_subdirectory(3rdparty/icu-ucnv)

### csUtil ###################################################################

list(APPEND csUtil_HEADERS
  include/csUtil/csCharConv.h
  include/csUtil/csCharUtil.h
  include/csUtil/csEndian.h
  include/csUtil/csFileIO.h
  include/csUtil/csFlags.h
  include/csUtil/csFormat.h
  include/csUtil/csILogger.h
  include/csUtil/csLogger.h
  include/csUtil/csNamespace.h
  include/csUtil/csPointer.h
  include/csUtil/csRawConverter.h
  include/csUtil/csStringUtil.h
  include/csUtil/csTextConverter.h
  include/csUtil/csTypeTraits.h
  include/internal/csutil_internal.h
  )

list(APPEND csUtil_SOURCES
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
  $<TARGET_OBJECTS:dblcnv>
  $<TARGET_OBJECTS:icu>
  )

format_output_name(csUtil "csUtil")

target_compile_definitions(csUtil PRIVATE CS_BUILD_UTIL_LIB)
target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(csUtil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
  ${CMAKE_BINARY_DIR}/include/csUtil/csutil_config.h DESTINATION include/csUtil
  )
