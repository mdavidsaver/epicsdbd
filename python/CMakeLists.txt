include_directories(
  ${PYTHON_INCLUDE_DIRS}
)

set_source_files_properties(../dbdlexparse.i
  PROPERTIES
    CPLUSPLUS ON
)
SWIG_ADD_MODULE(dbdlexparse python
  ../dbdlexparse.i
  ${PROJECT_SOURCE_DIR}/dbdlexer.cpp
)
