project("dbdparser" C CXX)

cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Werror)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(cmdlexer
  cmdlexer.cpp
  dbdlexer.cpp
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
