project("dbdparser" C CXX)

cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Werror)
endif()

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
