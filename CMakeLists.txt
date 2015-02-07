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

#find_package(PythonInterp 2.7)
find_package(PythonLibs 2.7.3 EXACT)

find_package(SWIG 2.0)
if(SWIG_FOUND)
  include(${SWIG_USE_FILE})
endif()

add_subdirectory(lib)
add_subdirectory(cli)

#if(SWIG_FOUND AND PYTHONLIBS_FOUND)
#  add_subdirectory(python)
#endif()
