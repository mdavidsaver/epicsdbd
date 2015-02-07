find_path(EPICS_BASE_DIR include/ellLib.h
  PATHS
   ${EPICS_BASE}
   /usr/lib/epics
   /usr/local/epics/base
   /usr/local/epics
  ENV EPICS_BASE
)
message(STATUS "EPICS_BASE_DOR=${EPICS_BASE_DIR}")
find_path(EPICS_CORE_INCLUDE_DIR ellLib.h
  PATHS ${EPICS_BASE_DIR}/include
  NO_DEFAULT_PATH
)

find_path(EPICS_OS_INCLUDE_DIR osdSock.h
  PATHS
    ${EPICS_BASE_DIR}/include/os/${EPICS_OS}
  NO_DEFAULT_PATH
)

if(EPICS_CORE_INCLUDE_DIR AND EPICS_OS_INCLUDE_DIR)
  list(APPEND EPICS_INCLUDE_DIRS ${EPICS_CORE_INCLUDE_DIR} ${EPICS_OS_INCLUDE_DIR})
endif()

list(APPEND EPICS_FIND_COMPONENTS Com)
message(STATUS "Looking for EPICS Libraries: ${EPICS_FIND_COMPONENTS}")
foreach(COMP IN LISTS EPICS_FIND_COMPONENTS)
  message(STATUS "LIB ${EPICS_BASE_DIR}/lib/${EPICS_HOST_ARCH}/lib${COMP}.so")
  find_library(EPICS_${COMP}_LIBRARY ${COMP}
    PATHS
      ${EPICS_BASE_DIR}/lib
    PATH_SUFFIXES ${EPICS_HOST_ARCH}
    NO_DEFAULT_PATH
  )

  if(EPICS_${COMP}_LIBRARY)
    list(APPEND EPICS_LIBRARIES ${EPICS_${COMP}_LIBRARY})
    set(EPICS_${COMP}_FOUND TRUE)
  endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EPICS
  FOUND_VAR EPICS_FOUND
  REQUIRED_VARS EPICS_CORE_INCLUDE_DIR EPICS_OS_INCLUDE_DIR EPICS_LIBRARIES
  HANDLE_COMPONENTS
)
