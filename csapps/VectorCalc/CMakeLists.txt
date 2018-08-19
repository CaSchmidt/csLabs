cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(VectorCalc-dev)

if(WIN32)
  set(BISON_EXECUTABLE
    D:/Development9/Tools/win_flex_bison-2.5.14/win_bison.exe
    CACHE FILEPATH "" FORCE
    )
  set(FLEX_EXECUTABLE
    D:/Development9/Tools/win_flex_bison-2.5.14/win_flex.exe
    CACHE FILEPATH "" FORCE
    )
endif()

add_subdirectory(VectorCalc)

set_target_properties(csUtil PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${VectorCalc_BINARY_DIR}
  )
