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

find_package(EPICS REQUIRED)

add_subdirectory(lib)
add_subdirectory(cli)
