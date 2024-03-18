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
  INCLUDE(InstallRequiredSystemLibraries)

  IF(OPENMP_FOUND)
    IF(MSVC90)
      INSTALL(
        PROGRAMS
          "${MSVC90_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC90.OPENMP/vcomp90.dll"
        DESTINATION ${TalipotBinInstallDir})
    ENDIF(MSVC90)

    IF(MSVC10)
      INSTALL(
        PROGRAMS
          "${MSVC10_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC100.OPENMP/vcomp100.dll"
        DESTINATION ${TalipotBinInstallDir})
    ENDIF(MSVC10)

    IF(MSVC11)
      INSTALL(
        PROGRAMS
          "${MSVC11_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC110.OPENMP/vcomp110.dll"
        DESTINATION ${TalipotBinInstallDir})
    ENDIF(MSVC11)

    IF(MSVC12)
      INSTALL(
        PROGRAMS
          "${MSVC12_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC120.OPENMP/vcomp120.dll"
        DESTINATION ${TalipotBinInstallDir})
    ENDIF(MSVC12)

    IF(MSVC_VERSION EQUAL 1900)
      INSTALL(
        PROGRAMS
          "${MSVC_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC140.OPENMP/vcomp140.dll"
        DESTINATION ${TalipotBinInstallDir})
    ELSEIF(MSVC_VERSION EQUAL 1910)
      # Visual Studio 2017
      INSTALL(
        PROGRAMS
          "${MSVC_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC150.OPENMP/vcomp140.dll"
        DESTINATION ${TalipotBinInstallDir})

    ELSEIF(MSVC_VERSION GREATER 1910 AND MSVC_VERSION LESS 1920)
      # Visual Studio 2019
      INSTALL(
        PROGRAMS
          "${MSVC_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC141.OPENMP/vcomp140.dll"
        DESTINATION ${TalipotBinInstallDir})
    ELSEIF(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
      INSTALL(
        PROGRAMS
          "${MSVC_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC142.OPENMP/vcomp140.dll"
        DESTINATION ${TalipotBinInstallDir})
    ELSEIF(MSVC_VERSION GREATER_EQUAL 1930)
      # Visual Studio 2022
      INSTALL(
        PROGRAMS
          "${MSVC_REDIST_DIR}/${CMAKE_MSVC_ARCH}/Microsoft.VC143.OPENMP/vcomp140.dll"
        DESTINATION ${TalipotBinInstallDir})
    ENDIF()
  ENDIF(OPENMP_FOUND)
ENDIF(MSVC)

# initialize the list of bundle libs (second arg of FIXUP_BUNDLE), updated in
# plugins/view/GeographicView, and finally used in
# software/talipot/CMakeLists.txt
SET_PROPERTY(GLOBAL PROPERTY FIXUP_BUNDLE_LIBS "")

GET_FILENAME_COMPONENT(NSIS_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\NSIS]"
                       ABSOLUTE)
STRING(COMPARE EQUAL "${NSIS_PATH}" "" NSIS_PATH_EMPTY)

IF(NSIS_PATH_EMPTY)
  GET_FILENAME_COMPONENT(
    NSIS_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\NSIS]" ABSOLUTE)
  STRING(COMPARE EQUAL "${NSIS_PATH}" "" NSIS_PATH_EMPTY)
ENDIF(NSIS_PATH_EMPTY)

IF(NOT NSIS_PATH_EMPTY)
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

ELSE(NOT NSIS_PATH_EMPTY)
  MESSAGE("Nullsoft Scriptable Install System is not installed on your system")
  MESSAGE("Talipot installer can not be generated without Nullsoft NSIS.")
ENDIF(NOT NSIS_PATH_EMPTY)
