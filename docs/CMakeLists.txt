set(DOXYGEN_HTML_OUTPUT "${CMAKE_SOURCE_DIR}/docs/doxygen")
set(DOXYGEN_PROJECT_NAME "DAQMonitor")
set(DOXYGEN_BUILTIN_STL_SUPPORT "YES")
set(DOXYGEN_EXCLUDE_PATTERNS "*.cpp")

doxygen_add_docs(
    docs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    COMMENT "Generate documentation"
)