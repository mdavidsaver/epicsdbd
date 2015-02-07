include_directories(
  ${EPICS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ast1
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ast2
  ${CMAKE_CURRENT_SOURCE_DIR}
)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Werror)
endif()

add_executable(cmdlexer
  cmdlexer.cpp
)

add_executable(cmdparser
  cmdparser.cpp
)

add_executable(cmdast1
  cmdast1.cpp
)

add_executable(cmdast2
  cmdast2.c
)

target_link_libraries(cmdlexer dbdast)
target_link_libraries(cmdparser dbdast)
target_link_libraries(cmdast1 dbdast)
target_link_libraries(cmdast2 dbdast)
