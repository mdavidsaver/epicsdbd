project("dbdparser" C CXX)

cmake_minimum_required(VERSION 2.8)

# Local modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Werror)
endif()

find_package(EPICS REQUIRED)

include_directories(
  ${EPICS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(cmdlexer
  cmdlexer.cpp
  dbdlexer.cpp
  cfstream.cpp
)

add_executable(cmdparser
  cmdparser.cpp
  dbdlexer.cpp
  dbdparser.cpp
  util.cpp
)

add_executable(cmdast1
  ast1/cmdast.cpp
  dbdlexer.cpp
  dbdparser.cpp
  ast1/dbdast.cpp
  util.cpp
)

add_executable(cmdast2
  ast2/dbexpand.c
  dbdlexer.cpp
  dbdparser.cpp
  ast2/astlib.c
  ast2/build.cpp
  cfstream.cpp
  util.cpp
)

target_link_libraries(cmdast2 ${EPICS_Com_LIBRARY})
