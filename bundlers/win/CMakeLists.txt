IF(MSVC)
  SET(SUFFIX "msvc")
ELSEIF(MINGW AND CLANG)
  SET(SUFFIX "clang")
ELSEIF(MINGW)
  SET(SUFFIX "mingw")
ENDIF(MSVC)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Talipot.nsi.in
               ${CMAKE_CURRENT_BINARY_DIR}/Talipot.nsi @ONLY)

IF(MSVC)
  IF(OPENMP_FOUND)
    SET(CMAKE_INSTALL_OPENMP_LIBRARIES TRUE)
  ENDIF(OPENMP_FOUND)

  INCLUDE(InstallRequiredSystemLibraries)
ENDIF(MSVC)

# initialize the list of bundle libs (second arg of FIXUP_BUNDLE), updated in
# plugins/view/GeographicView, and finally used in
# software/talipot/CMakeLists.txt
SET_PROPERTY(GLOBAL PROPERTY FIXUP_BUNDLE_LIBS "")

GET_FILENAME_COMPONENT(NSIS_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\NSIS]"
                       ABSOLUTE)
IF(NOT EXISTS ${NSIS_PATH})
  GET_FILENAME_COMPONENT(
    NSIS_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\NSIS]" ABSOLUTE)
ENDIF(NOT EXISTS ${NSIS_PATH})

IF(NOT EXISTS ${NSIS_PATH})
  SET(NSIS_PATH "C:\\Program Files (x86)\\NSIS")
ENDIF(NOT EXISTS ${NSIS_PATH})

IF(EXISTS ${NSIS_PATH})
  STRING(REPLACE "/" "\\" NSIS_PATH "${NSIS_PATH}")
  STRING(REPLACE "/" "\\" BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
  STRING(REPLACE "/" "\\" TLP_DIR "${CMAKE_INSTALL_PREFIX}")
  STRING(REPLACE "/" "\\" QTX_PLUGINS_DIR "${QT_PLUGINS_DIR}")

  IF(TalipotBuildIsRelease OR MSVC)
    SET(DEBUG_MODE "FALSE")
  ELSE(TalipotBuildIsRelease OR MSVC)
    SET(DEBUG_MODE "TRUE")
  ENDIF(TalipotBuildIsRelease OR MSVC)

  IF(MINGW OR CLANG)
    SET(BUNDLE_COMMAND
        cmd //C win_bundle.bat \"${NSIS_PATH}\" \"${TLP_DIR}\"
        \"${BINARY_DIR}\" \"${PYTHON_STDLIB_DIR}\" \"${PYTHON_VERSION}\")
  ELSE(MINGW OR CLANG)
    SET(BUNDLE_COMMAND
        cmd /C
        "\"win_bundle.bat \"${NSIS_PATH}\" \"${TLP_DIR}\" \"${BINARY_DIR}\" \"${PYTHON_STDLIB_DIR}\" \"${PYTHON_VERSION}\"\""
    )
  ENDIF(MINGW OR CLANG)

  ADD_CUSTOM_TARGET(
    bundle
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/win_bundle.bat
            ${CMAKE_CURRENT_BINARY_DIR}/win_bundle.bat
    COMMAND
      ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/FileAssociation.nsh
      ${CMAKE_CURRENT_BINARY_DIR}/FileAssociation.nsh
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/LICENSE
            ${CMAKE_CURRENT_BINARY_DIR}/LICENSE
    COMMAND ${BUNDLE_COMMAND}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ELSE(EXISTS ${NSIS_PATH})
  MESSAGE("Nullsoft Scriptable Install System is not installed on your system")
  MESSAGE("Talipot installer can not be generated without Nullsoft NSIS.")
ENDIF(EXISTS ${NSIS_PATH})
