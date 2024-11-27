CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

# Remove CMake warnings related to Qt5
IF(POLICY CMP0020)
  CMAKE_POLICY(SET CMP0020 NEW)
ENDIF(POLICY CMP0020)

# Needed by clang compiler provided by MacPorts or homebrew
IF(POLICY CMP0025)
  CMAKE_POLICY(SET CMP0025 NEW)
ENDIF(POLICY CMP0025)

# Remove CMake warnings related to rpath on MacOS
IF(POLICY CMP0042)
  CMAKE_POLICY(SET CMP0042 NEW)
ENDIF(POLICY CMP0042)

IF(POLICY CMP0068)
  CMAKE_POLICY(SET CMP0068 NEW)
ENDIF(POLICY CMP0068)

# FindOpenGL prefers GLVND by default when available
IF(POLICY CMP0072)
  CMAKE_POLICY(SET CMP0072 NEW)
ENDIF(POLICY CMP0072)

# enable the use of ccache for Talipot developers to speed up the build process
IF(NOT MSVC)
  SET(TALIPOT_USE_CCACHE
      OFF
      CACHE
        BOOL
        "Do you want to use ccache to speed up incremental compilation ? [OFF|ON]"
  )

  IF(TALIPOT_USE_CCACHE)
    FIND_PROGRAM(CCACHE_PROGRAM ccache)

    IF(CCACHE_PROGRAM)
      SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    ENDIF(CCACHE_PROGRAM)
  ENDIF(TALIPOT_USE_CCACHE)
ENDIF(NOT MSVC)

PROJECT(talipot)

# ========================================================
# Talipot CMake variables and macros
# ========================================================
INCLUDE(cmake/TalipotUseFile.cmake)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# convert CMAKE_INSTALL_PREFIX path into a cmake style path with unix / this is
# to avoid a path with a '\' as the last character which may be seen as an
# escape character (e.g. \")
FILE(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)

# Force build type to release with debug info if none has been provided
IF(NOT MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "")
  SET(CMAKE_BUILD_TYPE
      "RelWithDebInfo"
      CACHE STRING "" FORCE)
ENDIF(NOT MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "")

STRING(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "Release" result1)
STRING(COMPARE EQUAL "${CMAKE_BUILD_TYPE}" "RelWithDebInfo" result2)

IF(${result1} OR ${result2})
  SET(TalipotBuildIsRelease 1)
ELSE()
  SET(TalipotBuildIsRelease 0)
ENDIF()

# ========================================================
# Compiler-specific options and definitions
# ========================================================
TALIPOT_SET_COMPILER_OPTIONS_AND_DEFINITIONS()

# ========================================================
# Install paths
# ========================================================
INCLUDE(GNUInstallDirs)

SET(TalipotLibInstallDir ${CMAKE_INSTALL_LIBDIR})
SET(TalipotPluginsInstallDir ${TalipotLibInstallDir}/talipot)
SET(TalipotGlyphPluginsInstallDir ${TalipotPluginsInstallDir}/glyph)
SET(TalipotViewPluginsInstallDir ${TalipotPluginsInstallDir}/view)
SET(TalipotInteractorPluginsInstallDir ${TalipotPluginsInstallDir}/interactor)
SET(TalipotBinInstallDir ${CMAKE_INSTALL_BINDIR})
SET(TalipotIncludeInstallDir ${CMAKE_INSTALL_INCLUDEDIR})
SET(TalipotShareInstallDir ${CMAKE_INSTALL_DATADIR}/talipot)
SET(TalipotBitmapInstallDir ${TalipotShareInstallDir}/bitmaps)

# ========================================================
# Talipot version variables
# ========================================================
SET(TalipotMajorVersion 1)
SET(TalipotMinorVersion 0)
SET(TalipotReleaseVersion 0)
SET(TalipotVersionSuffix "-alpha3-dev")
SET(TalipotVersionSeparator .)

IF(MSVC) # specific options for Visual Studio
  SET(TalipotVersionSeparator _)
ENDIF()

SET(TalipotVersion "${TalipotMajorVersion}${TalipotVersionSeparator}")
SET(TalipotVersion
    "${TalipotVersion}${TalipotMinorVersion}${TalipotVersionSeparator}")
SET(TalipotVersion
    "${TalipotVersion}${TalipotReleaseVersion}${TalipotVersionSuffix}")
SET(TalipotMMVersion
    "${TalipotMajorVersion}${TalipotVersionSeparator}${TalipotMinorVersion}")
SET(TalipotMinorVersionTwoDigits "${TalipotMinorVersion}")

# Use two digits for the minor version when generating the Talipot integer
# version number as there was a 4.10 release that breaks preprocessor test like
# TALIPOT_INT_VERSION >= 4100 since the 5.0 release (see
# https://sourceforge.net/p/auber/bugs/869/)
STRING(LENGTH "${TalipotMinorVersion}" TalipotMinorVersionNbDigits)

IF(TalipotMinorVersionNbDigits LESS 2)
  SET(TalipotMinorVersionTwoDigits "0${TalipotMinorVersion}")
ENDIF(TalipotMinorVersionNbDigits LESS 2)

SET(TalipotIntMMVersion "${TalipotMajorVersion}${TalipotMinorVersionTwoDigits}")

IF(TalipotReleaseVersion MATCHES "^[0-9]+$")
  SET(TalipotIntVersion
      "${TalipotMajorVersion}${TalipotMinorVersionTwoDigits}${TalipotReleaseVersion}"
  )
ELSE(TalipotReleaseVersion MATCHES "^[0-9]+$")
  # just a hack to force the display of the full Talipot version in window title
  # bar when it contains non digit characters (like alpha, beta, dev, ...)
  SET(TalipotIntVersion "${TalipotMajorVersion}${TalipotMinorVersion}1")
ENDIF(TalipotReleaseVersion MATCHES "^[0-9]+$")

# ========================================================
# Dump git commit id to file
# ========================================================
INCLUDE(cmake/GetGitRevisionDescription.cmake)

IF(EXISTS ${PROJECT_SOURCE_DIR}/.git)
  SET(TALIPOT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
  GET_GIT_HEAD_REVISION(TALIPOT_REFSPEC TALIPOT_GIT_REVISION)
  CONFIGURE_FILE(utils/cmake-templates/GIT_COMMIT.in
                 ${CMAKE_CURRENT_BINARY_DIR}/GIT_COMMIT)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/GIT_COMMIT
          DESTINATION ${TalipotShareInstallDir})
ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/.git)

# ========================================================
# Talipot libraries names
# ========================================================
SET(LibTalipotCoreName "talipot-core-${TalipotMMVersion}")
SET(LibTalipotOGLName "talipot-ogl-${TalipotMMVersion}")
SET(LibTalipotGUIName "talipot-gui-${TalipotMMVersion}")
SET(LibTalipotPythonName "talipot-python-${TalipotMMVersion}")
SET(LibTalipotOGDFName "talipot-ogdf-${TalipotMMVersion}")
SET(LibTalipotCorePythonBindingsName "talipot-core-python-bindings")
SET(TalipotLibs ${LibTalipotCoreName} ${LibTalipotOGLName} ${LibTalipotGUIName}
                ${LibTalipotOGDFName})

# ========================================================
# Current year (used in documentation config files)
# ========================================================
STRING(TIMESTAMP CurrentYear "%Y")

# ========================================================
# CORE ONLY this used to have a minimal build of Talipot without any of the
# opengl, qt or python components
# ========================================================
IF(NOT EXISTS ${PROJECT_SOURCE_DIR}/cmake/FindQtX.cmake)
  SET(TALIPOT_BUILD_CORE_ONLY ON)
  MESSAGE("You can only configure a core build of Talipot.")
ELSE()
  SET(TALIPOT_BUILD_CORE_ONLY
      OFF
      CACHE BOOL "Do you want a core build of Talipot (no OpenGL and Qt GUI) ?")
ENDIF()

IF(TALIPOT_BUILD_CORE_ONLY)
  # Allow to build OpenGL components to tweak CI builds and cache more compiled
  # object files between stages.
  SET(TALIPOT_BUILD_OGL
      OFF
      CACHE BOOL "")
ENDIF(TALIPOT_BUILD_CORE_ONLY)

SET(TALIPOT_PYTHON_ROOT_FOLDER
    ${CMAKE_BINARY_DIR}/library/talipot-python/bindings/talipot-core/talipot_module
)
SET(TALIPOT_PYTHON_FOLDER ${TALIPOT_PYTHON_ROOT_FOLDER}/talipot)
SET(TALIPOT_PYTHON_NATIVE_FOLDER ${TALIPOT_PYTHON_FOLDER}/native)
SET(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET
    OFF
    CACHE
      BOOL
      "Add a wheel make target for generating Python talipot-core module wheel ?"
)

IF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
  SET(TALIPOT_BUILD_CORE_ONLY ON)

  IF(APPLE)
    # build binaries with install rpath when generating Talipot Python wheels
    # for MacOs as we don't install the project in that case
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  ELSEIF(LINUX)
    FILE(READ /etc/centos-release LINUX_ISSUE)

    IF(NOT "${LINUX_ISSUE}" MATCHES ".*CentOS Linux release 7\\.9.*"
       OR NOT EXISTS /usr/local/bin/auditwheel)
      MESSAGE(
        FATAL_ERROR
          "Python binary wheels for Linux platform must be compiled "
          "using the CentOS 7.9 docker image provided by the manylinux "
          "project from the Python Packaging Authority.\n"
          "See https://github.com/pypa/manylinux for more details.")
    ENDIF(NOT "${LINUX_ISSUE}" MATCHES ".*CentOS Linux release 7\\.9.*"
          OR NOT EXISTS /usr/local/bin/auditwheel)

    # build binaries with install rpath when generating Talipot Python wheels
    # for Linux as we don't install the project in that case
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  ENDIF(APPLE)
ENDIF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)

# ========================================================
# Required packages
# ========================================================
IF(WIN32 AND NOT MSVC)
  # Set some common libraries and headers search paths for MinGW
  SET(CMAKE_LIBRARY_PATH
      "${MINGW_BIN_PATH};${MINGW_LIB_PATH};${MINGW_LIB64_PATH};${CMAKE_LIBRARY_PATH}"
  )
  SET(CMAKE_INCLUDE_PATH
      "${MINGW_INCLUDE_PATH};${MINGW_INCLUDE_PATH}/freetype2;${CMAKE_INCLUDE_PATH}"
  )

  # Check if libintl is present in the MinGW build environment
  FIND_LIBRARY(
    INTL_LIBRARY intl
    PATHS ${MINGW_LIB_PATH} ${MINGW_LIB64_PATH}
    NO_DEFAULT_PATH)

  # Check if libsframe is present in the MinGW build environment
  FIND_LIBRARY(
    SFRAME_LIBRARY sframe
    PATHS ${MINGW_LIB_PATH} ${MINGW_LIB64_PATH}
    NO_DEFAULT_PATH)
ELSE(WIN32 AND NOT MSVC)
  FIND_LIBRARY(INTL_LIBRARY intl)
  FIND_LIBRARY(SFRAME_LIBRARY sframe)
ENDIF(WIN32 AND NOT MSVC)

MARK_AS_ADVANCED(INTL_LIBRARY)
MARK_AS_ADVANCED(SFRAME_LIBRARY)

# ZLIB
FIND_PACKAGE(ZLIB REQUIRED)

# Zstd
FIND_PACKAGE(Zstd REQUIRED)

# Qhull
FIND_PACKAGE(Qhull)

# Yajl
FIND_PACKAGE(Yajl)

# Graphviz
FIND_PACKAGE(Graphviz REQUIRED)

# Libgit2
FIND_PACKAGE(Libgit2 REQUIRED)

IF(NOT TALIPOT_BUILD_CORE_ONLY OR TALIPOT_BUILD_OGL)
  # OpenGL
  FIND_PACKAGE(OpenGL REQUIRED)

  # Glew
  FIND_PACKAGE(GLEW REQUIRED)

  # if needed, when using cmake 3.15, define some undefined glew variables
  IF(NOT ${CMAKE_VERSION} VERSION_LESS "3.15.0")
    IF(NOT GLEW_INCLUDE_DIR)
      GET_TARGET_PROPERTY(GLEW_INCLUDE_DIR GLEW::GLEW
                          INTERFACE_INCLUDE_DIRECTORIES)
    ENDIF(NOT GLEW_INCLUDE_DIR)

    IF(NOT GLEW_LIBRARY)
      SET(GLEW_LIBRARY GLEW::GLEW)
    ENDIF(NOT GLEW_LIBRARY)
  ENDIF(NOT ${CMAKE_VERSION} VERSION_LESS "3.15.0")

  # Freetype
  FIND_PACKAGE(Freetype REQUIRED)
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY OR TALIPOT_BUILD_OGL)

IF(NOT TALIPOT_BUILD_CORE_ONLY)
  # FontConfig
  FIND_PACKAGE(FontConfig REQUIRED)

  # FriBidi
  FIND_PACKAGE(FriBidi REQUIRED)

  # Qt
  FIND_PACKAGE(QtX)

  # Quazip
  FIND_PACKAGE(QuaZip)
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY)

# Python
INCLUDE(cmake/TalipotPython.cmake)

# ========================================================
# Include and library paths
# ========================================================
SET(GZStreamInclude ${PROJECT_SOURCE_DIR}/thirdparty/gzstream/)
SET(GZStreamLibrary gzstream-talipot-${TalipotMMVersion})
SET(ZstdStreamInclude ${PROJECT_SOURCE_DIR}/thirdparty/ZstdStream/)

IF(NOT TALIPOT_BUILD_CORE_ONLY OR TALIPOT_BUILD_OGL)
  SET(FTGLInclude ${PROJECT_SOURCE_DIR}/thirdparty/ftgl/)
  SET(FTGLLibrary ftgl-talipot-${TalipotMMVersion})
  SET(Tess2Include ${PROJECT_SOURCE_DIR}/thirdparty/libtess2/Include)
  SET(Tess2Library tess2-talipot-${TalipotMMVersion})

  IF(NOT QUAZIP_FOUND)
    SET(QUAZIP_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/thirdparty/quazip-1.4/quazip)
    SET(QUAZIP_LIBRARIES QuaZip)
  ENDIF(NOT QUAZIP_FOUND)
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY OR TALIPOT_BUILD_OGL)

SET(OGDFInclude ${PROJECT_SOURCE_DIR}/thirdparty/OGDF/include)
SET(OGDFLibrary OGDF-talipot-${TalipotMMVersion})

SET(VPSCInclude ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/)
SET(VPSCLibrary vpsc)

SET(StbInclude ${PROJECT_SOURCE_DIR}/thirdparty/stb)
SET(ParallelHashmapInclude ${PROJECT_SOURCE_DIR}/thirdparty/)

SET(TalipotCoreInclude ${PROJECT_SOURCE_DIR}/library/talipot-core/include/)
SET(TalipotCoreBuildInclude ${PROJECT_BINARY_DIR}/library/talipot-core/include/)
SET(TalipotOGLInclude ${PROJECT_SOURCE_DIR}/library/talipot-ogl/include/)
SET(TalipotOGDFInclude ${PROJECT_SOURCE_DIR}/library/talipot-ogdf/include)
SET(TalipotGUIInclude ${PROJECT_SOURCE_DIR}/library/talipot-gui/include/)
SET(TalipotUIGUIInclude ${PROJECT_BINARY_DIR}/library/talipot-gui/src/)
SET(TalipotGUIBuildInclude ${PROJECT_BINARY_DIR}/library/talipot-gui/include/)
SET(TalipotPythonInclude ${PROJECT_SOURCE_DIR}/library/talipot-python/include)

# ========================================================
# Point RPATH to lib folder
# ========================================================
# On MacOS, use rpaths (i.e. the "install_name" field of shared libraries will
# be @rpath) This is already enabled by default for CMake >= 3.0 (see
# http://www.cmake.org/cmake/help/v3.0/prop_tgt/MACOSX_RPATH.html)
SET(CMAKE_MACOSX_RPATH ON)

FUNCTION(ADD_LIBRARY name)
  _ADD_LIBRARY(${name} ${ARGN})

  IF(APPLE)
    # relative locations of Talipot dylibs dependencies are not the sames when
    # generating Python wheel
    IF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
      SET_TARGET_PROPERTIES(${name} PROPERTIES INSTALL_RPATH
                                               "@loader_path/;@loader_path/../")
    ELSE(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
      SET_TARGET_PROPERTIES(
        ${name}
        PROPERTIES
          INSTALL_RPATH
          "@loader_path/../lib;@loader_path/../Frameworks;${QT_FRAMEWORKS_DIR}")
    ENDIF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
  ELSE(APPLE)
    # CMake >= 3.1.0 does not allow to set the INSTALL_RPATH property on
    # INTERFACE_LIBRARY target type
    GET_TARGET_PROPERTY(TARGET_TYPE ${name} TYPE)

    IF(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
      SET_TARGET_PROPERTIES(
        ${name} PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/../:$ORIGIN/../../")

      IF(MSVC)
        SET_TARGET_PROPERTIES(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
                                                 "${CMAKE_CURRENT_BINARY_DIR}")
        SET_TARGET_PROPERTIES(
          ${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
                             "${CMAKE_CURRENT_BINARY_DIR}")
        SET_TARGET_PROPERTIES(
          ${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL
                             "${CMAKE_CURRENT_BINARY_DIR}")
        SET_TARGET_PROPERTIES(
          ${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
                             "${CMAKE_CURRENT_BINARY_DIR}")

        # MSVC linker PDB files are only generated for shared libraries
        IF("${ARGV1}" STREQUAL "SHARED" AND NOT "${ARGV2}" STREQUAL "IMPORTED")
          SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_NAME_DEBUG "${name}")
          SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_NAME_RELWITHDEBINFO
                                                   "${name}")
          SET_TARGET_PROPERTIES(
            ${name} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG
                               "${CMAKE_CURRENT_BINARY_DIR}")
          SET_TARGET_PROPERTIES(
            ${name} PROPERTIES PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO
                               "${CMAKE_CURRENT_BINARY_DIR}")
          INSTALL(
            FILES $<TARGET_PDB_FILE:${name}>
            DESTINATION ${TalipotShareInstallDir}/pdb
            CONFIGURATIONS Debug)
          INSTALL(
            FILES $<TARGET_PDB_FILE:${name}>
            DESTINATION ${TalipotShareInstallDir}/pdb
            CONFIGURATIONS RelWithDebInfo)
        ENDIF("${ARGV1}" STREQUAL "SHARED" AND NOT "${ARGV2}" STREQUAL
                                               "IMPORTED")
      ENDIF(MSVC)
    ENDIF(NOT "${TARGET_TYPE}" STREQUAL "INTERFACE_LIBRARY")
  ENDIF(APPLE)

  IF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
    ADD_DEPENDENCIES(wheel ${name})
  ENDIF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
ENDFUNCTION()

FUNCTION(ADD_EXECUTABLE name)
  _ADD_EXECUTABLE(${name} ${ARGN})

  IF(APPLE)
    SET_TARGET_PROPERTIES(
      ${name}
      PROPERTIES
        INSTALL_RPATH
        "@loader_path/../${CMAKE_INSTALL_LIBDIR};@loader_path/../Frameworks;${QT_FRAMEWORKS_DIR}"
    )
  ELSEIF(LINUX)
    SET_TARGET_PROPERTIES(
      ${name} PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
  ELSE()
    SET_TARGET_PROPERTIES(${name} PROPERTIES INSTALL_RPATH "$ORIGIN")
  ENDIF()

  IF(MSVC)
    SET_TARGET_PROPERTIES(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
                                             "${CMAKE_CURRENT_BINARY_DIR}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
                                             "${CMAKE_CURRENT_BINARY_DIR}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL
                                             "${CMAKE_CURRENT_BINARY_DIR}")
    SET_TARGET_PROPERTIES(
      ${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO
                         "${CMAKE_CURRENT_BINARY_DIR}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_NAME_DEBUG "${name}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_NAME_RELWITHDEBINFO "${name}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_OUTPUT_DIRECTORY_DEBUG
                                             "${CMAKE_CURRENT_BINARY_DIR}")
    SET_TARGET_PROPERTIES(${name} PROPERTIES PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO
                                             "${CMAKE_CURRENT_BINARY_DIR}")

    IF(NOT "${ARGV1}" STREQUAL "IMPORTED")
      INSTALL(
        FILES $<TARGET_PDB_FILE:${name}>
        DESTINATION ${TalipotShareInstallDir}/pdb
        CONFIGURATIONS Debug)
      INSTALL(
        FILES $<TARGET_PDB_FILE:${name}>
        DESTINATION ${TalipotShareInstallDir}/pdb
        CONFIGURATIONS RelWithDebInfo)
    ENDIF(NOT "${ARGV1}" STREQUAL "IMPORTED")
  ENDIF(MSVC)
ENDFUNCTION()

# ========================================================
# Tests
# ========================================================
IF(EXISTS ${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)
  SET(TALIPOT_BUILD_TESTS
      OFF
      CACHE BOOL "Do you want to build the tests ? [OFF|ON]")
ELSE()
  SET(TALIPOT_BUILD_TESTS OFF)
ENDIF()

# ========================================================
# Create library/talipot/include/talipot/Release.h base on Release.h.in
# ========================================================
CONFIGURE_FILE("library/talipot-core/include/talipot/Release.h.in"
               "library/talipot-core/include/talipot/Release.h")

# ========================================================
# Configure file for uninstall target
# ========================================================
IF(LINUX)
  IF("${CMAKE_INSTALL_PREFIX}" MATCHES "^/home.*$")
    SET(DESKTOP_CONFIG_INSTALL_PREFIX "$ENV{HOME}/.local")
  ELSE("${CMAKE_INSTALL_PREFIX}" MATCHES "^/home.*$")
    SET(DESKTOP_CONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
  ENDIF("${CMAKE_INSTALL_PREFIX}" MATCHES "^/home.*$")
ENDIF(LINUX)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(
  uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  VERBATIM)

ADD_SUBDIRECTORY(thirdparty)

INCLUDE_DIRECTORIES(${ParallelHashmapInclude})
ADD_SUBDIRECTORY(library)
ADD_SUBDIRECTORY(plugins)
ADD_SUBDIRECTORY(utils/crash_handler)

IF(NOT TALIPOT_BUILD_CORE_ONLY)
  ADD_SUBDIRECTORY(demos)
  ADD_SUBDIRECTORY(software)
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY)

ADD_SUBDIRECTORY(doc)

IF(TALIPOT_BUILD_TESTS)
  INCLUDE(CTest)
  ENABLE_TESTING()

  IF(CMAKE_CONFIGURATION_TYPES)
    ADD_CUSTOM_TARGET(
      tests COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process
                    --output-on-failure --build-config "$<CONFIGURATION>")
  ELSE(CMAKE_CONFIGURATION_TYPES)
    ADD_CUSTOM_TARGET(
      tests COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process
                    --output-on-failure)
  ENDIF(CMAKE_CONFIGURATION_TYPES)

  ADD_SUBDIRECTORY(tests)
ENDIF(TALIPOT_BUILD_TESTS)

INSTALL(FILES AUTHORS LICENSE DESTINATION ${TalipotShareInstallDir})

INSTALL(
  FILES cmake/FindTalipot.cmake
  RENAME TalipotConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Talipot/)

CONFIGURE_FILE(cmake/TalipotConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/cmake/TalipotConfigVersion.cmake @ONLY)

INSTALL(FILES cmake/TalipotUseFile.cmake
              ${CMAKE_BINARY_DIR}/cmake/TalipotConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Talipot/)

# ========================================================
# talipot-config script
# ========================================================
SET(prefix "${CMAKE_INSTALL_PREFIX}")
SET(exec_prefix "\${prefix}")
SET(includedir "\${prefix}/${TalipotIncludeInstallDir}")
SET(libdir "\${prefix}/${TalipotLibInstallDir}")
SET(libversion "${TalipotMMVersion}")
SET(VERSION "${TalipotVersion}")
SET(WIN_VERSION "${TalipotMajorVersion}.${TalipotMinorVersion}")

IF(APPLE AND NOT "${CMAKE_OSX_SYSROOT}" STREQUAL "")
  TALIPOT_SET_CXX_FLAGS("-isysroot ${CMAKE_OSX_SYSROOT}")
ENDIF(APPLE AND NOT "${CMAKE_OSX_SYSROOT}" STREQUAL "")

CONFIGURE_FILE("talipot-config.in" "talipot-config" @ONLY)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/talipot-config
        DESTINATION ${TalipotBinInstallDir})

IF(NOT TALIPOT_BUILD_CORE_ONLY)
  ADD_SUBDIRECTORY(bundlers)
ENDIF(NOT TALIPOT_BUILD_CORE_ONLY)
