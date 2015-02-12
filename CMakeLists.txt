project("dbdparser" C CXX)

cmake_minimum_required(VERSION 2.8)

# Local modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE DEBUG)
endif()

find_package(EPICS REQUIRED)

add_subdirectory(lib)
add_subdirectory(cli)
