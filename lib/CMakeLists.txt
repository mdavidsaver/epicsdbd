include_directories(
  ${EPICS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Werror)
endif()

add_library(dbdast STATIC
  cfstream.cpp
  dbdlexer.cpp
  dbdparser.cpp
  util.cpp
  ast1/dbdast.cpp
  ast2/astlib.c
  ast2/build.cpp
)

target_link_libraries(dbdast ${EPICS_Com_LIBRARY})
