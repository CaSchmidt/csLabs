cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(VectorCalc)

### Modules ##################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

### Dependencies #############################################################

find_package(BISON 3.0 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

find_package(Eigen3 3.3 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
find_package(Qt5Widgets 5.6 REQUIRED)

add_subdirectory(../../../cslibs/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil
  )

### VectorCalc ###############################################################

BISON_TARGET(AST_Parser compiler/ast.y
  ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ast.tab.h
  )
FLEX_TARGET(AST_Scanner compiler/ast.l
  ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp
  )
ADD_FLEX_BISON_DEPENDENCY(AST_Scanner AST_Parser)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ast.tab.h PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ast_parser.cpp PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ast_scanner.cpp PROPERTY SKIP_AUTOGEN ON)

set(VectorCalc_HEADERS
  include/ast/ast.h
  include/ast/ast_basictypes.h
  include/ast/parser.h
  include/ast/printer.h
  include/eval/arithmetic.h
  include/eval/engine.h
  include/eval/environment.h
  include/eval/exception.h
  include/eval/function.h
  include/eval/logical.h
  include/eval/relational.h
  include/eval/result.h
  include/eval/unary.h
  include/eval/variables.h
  include/gui/environmentmodel.h
  include/gui/wlogger.h
  include/gui/wmainwindow.h
  include/internal/ast/parser_p.h
  )

set(VectorCalc_SOURCES
  src/ast/ast_aux.cpp
  src/ast/ast_binary.cpp
  src/ast/ast_node.cpp
  src/ast/ast_parser_p.cpp
  src/ast/ast_primary.cpp
  src/ast/ast_unary.cpp
  src/ast/parser.cpp
  src/ast/printer.cpp
  src/eval/arithmetic.cpp
  src/eval/engine.cpp
  src/eval/environment.cpp
  src/eval/exception.cpp
  src/eval/function.cpp
  src/eval/logical.cpp
  src/eval/relational.cpp
  src/eval/result.cpp
  src/eval/unary.cpp
  src/eval/variables.cpp
  src/gui/environmentmodel.cpp
  src/gui/wlogger.cpp
  src/gui/wmainwindow.cpp
  src/main.cpp
  )

### Target ###################################################################

add_executable(VectorCalc
  ${VectorCalc_HEADERS} ${VectorCalc_SOURCES}
  ${BISON_AST_Parser_OUTPUTS} ${FLEX_AST_Scanner_OUTPUTS}
  resources/resources.qrc
  )

set_property(TARGET VectorCalc PROPERTY CXX_STANDARD 14)
set_property(TARGET VectorCalc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET VectorCalc PROPERTY CXX_EXTENSIONS OFF)

if(WIN32)
  set_target_properties(VectorCalc PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_link_libraries(VectorCalc csUtil Qt5::Widgets)

format_output_name(VectorCalc "VectorCalc")

target_include_directories(VectorCalc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(VectorCalc PRIVATE ${csUtil_SOURCE_DIR}/include)
target_include_directories(VectorCalc PRIVATE ${csUtil_BINARY_DIR}/include)

install(TARGETS VectorCalc
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
