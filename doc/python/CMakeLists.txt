CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

# On windows, it is required that the paths to the dll dependencies (e.g. zlib)
# are contained in the CMAKE_LIBRARY_PATH variable (especially with Visual
# Studio). Some warnings will be output when configuring the whole project if it
# is not the case. Otherwise, the Python API documentation will not be generated
# as the import of the talipot modules will fail.

# Turn CMAKE_LIBRARY_PATH list into a string in order to pass all its contents
# to the CMake command generating the doc, only the first list entry is
# transferred otherwise.
STRING(REPLACE ";" "," CMAKE_LIBRARY_PATH_STR "${CMAKE_LIBRARY_PATH}")

# Ensure that libraries paths are in MSYS format when using MSYS Makefiles,
# otherwise the documentation fails to build
IF("${CMAKE_GENERATOR}" MATCHES ".*MSYS.*")
  TALIPOT_WINDOWS_TO_MSYS_PATH(${CMAKE_LIBRARY_PATH_STR} CMAKE_LIBRARY_PATH_STR)
ENDIF("${CMAKE_GENERATOR}" MATCHES ".*MSYS.*")

ADD_CUSTOM_TARGET(
  pythonBindingsDoc ALL
  ${CMAKE_COMMAND}
  -DSPHINX_EXECUTABLE=${SPHINX_EXECUTABLE}
  -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
  -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
  -DBIN_DIR=${CMAKE_CURRENT_BINARY_DIR}
  -DTALIPOT_SOURCE_DIR=${CMAKE_SOURCE_DIR}
  -DTALIPOT_BUILD_DIR=${CMAKE_BINARY_DIR}
  -DSYSTEM_SIP=${SYSTEM_SIP}
  -DSIP_VERSION=${SIP_VERSION_STR}
  -DQT_BIN_DIR=${QT_BINARY_DIR}
  -DAPPLE=${APPLE}
  -DWIN32=${WIN32}
  -DCMAKE_LIBRARY_PATH=${CMAKE_LIBRARY_PATH_STR}
  -P
  ${CMAKE_CURRENT_SOURCE_DIR}/genSphinxDoc.cmake
  DEPENDS ${TalipotPythonModuleName} ${TALIPOT_PLUGIN_TARGETS}
  VERBATIM)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "doc;doctrees")

INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/talipot-python)
