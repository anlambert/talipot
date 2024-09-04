# The FindTalipot script can be used from any CMake-based project to set up
# environment variables pointing to Talipot libraries and headers directories.
# The FIND_PACKAGE command provides several arguments. While the COMPONENTS
# argument will be ignored, you can tell FindTalipot to look for a specific
# version of Talipot libraries:.
#
# FIND_PACKAGE(Talipot 1.0.0 REQUIRED) # Will look for Talipot 1.0.0 or higher
# See CMake documentation for details about the FIND_PACKAGE macro.
#
# By default, this script will try to automatically detect the installation
# directory of Talipot libraries installed on your system. You can override this
# behavior by defining the TALIPOT_DIR variable. If this variable is set,
# FindTalipot will only check into that directory for a valid Talipot
# installation.
#
# This script will output the following variables:
#
# TALIPOT_DIR (will not be modified if already set): The Talipot installation
# path TALIPOT_FOUND: This variable is set to true if a valid Talipot
# installation has been found. TALIPOT_INCLUDE_DIR: The path where Talipot
# headers are located. TALIPOT_LIBRARIES: Contains path to the Talipot libraries
# TALIPOT_LIBRARIES_DIR: Contains path to the Talipot libraries
# TALIPOT_PLUGINS_DIR: Path where Talipot plugins are located
# TALIPOT_GLYPH_PLUGINS_DIR: Path where Talipot glyph plugins are located
# TALIPOT_INTERACTOR_PLUGINS_DIR: Path where Talipot interaction plugins are
# located TALIPOT_VIEW_PLUGINS_DIR: Path where Talipot view plugins are located
# TALIPOT_CORE_LIBRARY: The path to the TalipotCore module library.
# TALIPOT_OGL_LIBRARY: The path to the TalipotOGL module library.
# TALIPOT_GUI_LIBRARY: The path to the TalipotGUI module library.
# TALIPOT_OGDF_LIBRARY: The path to the TalipotOGDF module library.
# TALIPOT_PYTHON_LIBRARY: The path to the TalipotPython module library.
# TALIPOT_SHARE_DIR: Installation path for resources TALIPOT_VERSION: Complete
# version string. TALIPOT_MAJOR_VERSION: Major version digit.
# TALIPOT_MINOR_VERSION: Minor version digit. TALIPOT_PATCH_VERSION: Patch
# version digit. TALIPOT_USE_FILE: Additional Talipot-related macro definitions

# Define version separator (different for MSVC builds)
SET(TalipotVersionSeparator .)

IF(${CMAKE_GENERATOR} MATCHES "Visual Studio" OR ${CMAKE_GENERATOR} MATCHES
                                                 "NMake Makefiles")
  SET(TalipotVersionSeparator _)
ENDIF()

# A macro to retrieve Talipot version from the talipot/Release.h file This will
# output the following variables: TALIPOT_VERSION: Complete version string.
# TALIPOT_MAJOR_VERSION: Major version digit. TALIPOT_MINOR_VERSION: Minor
# version digit. TALIPOT_PATCH_VERSION: Patch version digit.
MACRO(RETRIEVE_VERSION)
  # Unset previous variables
  SET(TALIPOT_VERSION)
  SET(TALIPOT_MAJOR_VERSION)
  SET(TALIPOT_MINOR_VERSION)
  SET(TALIPOT_PATCH_VERSION)

  # Check for Release.h
  IF(TALIPOT_INCLUDE_DIR)
    FILE(
      STRINGS ${TALIPOT_INCLUDE_DIR}/talipot/Release.h TMPSTR
      REGEX
        "[0-9]*\\${TalipotVersionSeparator}[0-9]*\\${TalipotVersionSeparator}[0-9][^\"]*"
      NO_HEX_CONVERSION)

    STRING(
      REGEX
        MATCH
        "[0-9]*\\${TalipotVersionSeparator}[0-9]*\\${TalipotVersionSeparator}[0-9][^\"]*"
        TALIPOT_VERSION
        ${TMPSTR})

    STRING(
      REGEX
      REPLACE
        "([0-9]*)\\${TalipotVersionSeparator}([0-9]*)\\${TalipotVersionSeparator}([0-9][^\"]*)"
        "\\1"
        TALIPOT_MAJOR_VERSION
        ${TALIPOT_VERSION})

    STRING(
      REGEX
      REPLACE
        "([0-9]*)\\${TalipotVersionSeparator}([0-9]*)\\${TalipotVersionSeparator}([0-9][^\"]*)"
        "\\2"
        TALIPOT_MINOR_VERSION
        ${TALIPOT_VERSION})

    STRING(
      REGEX
      REPLACE
        "([0-9]*)\\${TalipotVersionSeparator}([0-9]*)\\${TalipotVersionSeparator}([0-9][^\"]*)"
        "\\3"
        TALIPOT_PATCH_VERSION
        ${TALIPOT_VERSION})
  ENDIF(TALIPOT_INCLUDE_DIR)
ENDMACRO(RETRIEVE_VERSION)

# Check if the given version corresponds to the one asked when calling
# FIND_PACKAGE This will exit with a fatal error if provided version is
# considered as not compatible with requested version
MACRO(CHECK_VERSION)
  # backward compatibility version check for users that still uses the old
  # Talipot CMake setup (using CMAKE_MODULE_PATH and FindTalipot.cmake)
  IF(TALIPOT_FIND_VERSION
     AND NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/TalipotConfigVersion.cmake")
    IF("${TALIPOT_VERSION}" VERSION_LESS "${TALIPOT_FIND_VERSION}")
      MESSAGE(
        FATAL_ERROR
          "Invalid Talipot version string given: ${TALIPOT_FIND_VERSION}, detected version is ${TALIPOT_VERSION}"
      )
    ENDIF("${TALIPOT_VERSION}" VERSION_LESS "${TALIPOT_FIND_VERSION}")
  ENDIF(TALIPOT_FIND_VERSION
        AND NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/TalipotConfigVersion.cmake")
ENDMACRO(CHECK_VERSION)

IF(NOT TALIPOT_INCLUDE_DIR)
  FIND_PATH(TALIPOT_INCLUDE_DIR talipot/Release.h PATHS ${TALIPOT_DIR}/include)

  IF(NOT TALIPOT_INCLUDE_DIR) # Last resort: use
    # CMAKE_MODULE_PATH/../../include/
    FOREACH(prefix ${CMAKE_MODULE_PATH} ${CMAKE_PREFIX_PATH})
      FIND_PATH(TALIPOT_INCLUDE_DIR talipot/Release.h
                PATHS "${prefix}/../../include")

      IF(TALIPOT_INCLUDE_DIR)
        BREAK()
      ENDIF(TALIPOT_INCLUDE_DIR)
    ENDFOREACH()
  ENDIF(NOT TALIPOT_INCLUDE_DIR)
ENDIF(NOT TALIPOT_INCLUDE_DIR)

IF(NOT TALIPOT_INCLUDE_DIR)
  MESSAGE(
    FATAL_ERROR
      "Auto-detecting your Talipot headers installation directory failed. Please try to provide the TALIPOT_DIR CMake variable pointing to your Talipot installation root directory"
  )
ELSE()
  SET(TALIPOT_DIR ${TALIPOT_INCLUDE_DIR}/../)
ENDIF(NOT TALIPOT_INCLUDE_DIR)

RETRIEVE_VERSION()
CHECK_VERSION()

SET(TALIPOT_INCLUDE_DIR ${TALIPOT_INCLUDE_DIR} ${TALIPOT_INCLUDE_DIR}/talipot)

# get absolute path of Talipot root installation folder
GET_FILENAME_COMPONENT(TALIPOT_DIR "${TALIPOT_DIR}" ABSOLUTE)

# set CMAKE_INSTALL_PREFIX in order for the GNUInstallDirs CMake module to
# correctly set the CMAKE_INSTALL_LIBDIR on Debian when Talipot has been
# installed in /usr through a package
SET(CMAKE_INSTALL_PREFIX "${TALIPOT_DIR}")

INCLUDE(GNUInstallDirs)

MARK_AS_ADVANCED(TALIPOT_DIR)
MARK_AS_ADVANCED(TALIPOT_INCLUDE_DIR)

IF(WIN32)
  SET(TALIPOT_LIBRARIES_DIR ${TALIPOT_DIR}/${CMAKE_INSTALL_BINDIR})
ELSE()
  SET(TALIPOT_LIBRARIES_DIR ${TALIPOT_DIR}/${CMAKE_INSTALL_LIBDIR})
ENDIF()

MARK_AS_ADVANCED(TALIPOT_LIBRARIES_DIR)
SET(TALIPOT_PLUGINS_DIR ${TALIPOT_DIR}/${CMAKE_INSTALL_LIBDIR}/talipot)
SET(TALIPOT_SHARE_DIR ${TALIPOT_DIR}/${CMAKE_INSTALL_DATADIR}/talipot)
MARK_AS_ADVANCED(TALIPOT_PLUGINS_DIR)
SET(TALIPOT_GLYPH_PLUGINS_DIR ${TALIPOT_PLUGINS_DIR}/glyph)
MARK_AS_ADVANCED(TALIPOT_GLYPH_PLUGINS_DIR)
SET(TALIPOT_INTERACTOR_PLUGINS_DIR ${TALIPOT_PLUGINS_DIR}/interactor)
MARK_AS_ADVANCED(TALIPOT_INTERACTOR_PLUGINS_DIR)
SET(TALIPOT_VIEW_PLUGINS_DIR ${TALIPOT_PLUGINS_DIR}/view)
MARK_AS_ADVANCED(TALIPOT_VIEW_PLUGINS_DIR)
SET(TALIPOT_FOUND true)

SET(TALIPOT_LIBRARIES_DIR ${TALIPOT_DIR}/${CMAKE_INSTALL_LIBDIR})

FIND_LIBRARY(
  TALIPOT_CORE_LIBRARY
  "talipot-core-${TALIPOT_MAJOR_VERSION}${TalipotVersionSeparator}${TALIPOT_MINOR_VERSION}"
  PATHS ${TALIPOT_LIBRARIES_DIR}
  NO_DEFAULT_PATH)

FIND_LIBRARY(
  TALIPOT_OGL_LIBRARY
  "talipot-ogl-${TALIPOT_MAJOR_VERSION}${TalipotVersionSeparator}${TALIPOT_MINOR_VERSION}"
  PATHS ${TALIPOT_LIBRARIES_DIR}
  NO_DEFAULT_PATH)

FIND_LIBRARY(
  TALIPOT_GUI_LIBRARY
  "talipot-gui-${TALIPOT_MAJOR_VERSION}${TalipotVersionSeparator}${TALIPOT_MINOR_VERSION}"
  PATHS ${TALIPOT_LIBRARIES_DIR}
  NO_DEFAULT_PATH)

FIND_LIBRARY(
  TALIPOT_OGDF_LIBRARY
  "talipot-ogdf-${TALIPOT_MAJOR_VERSION}${TalipotVersionSeparator}${TALIPOT_MINOR_VERSION}"
  PATHS ${TALIPOT_LIBRARIES_DIR}
  NO_DEFAULT_PATH)

FIND_LIBRARY(
  TALIPOT_PYTHON_LIBRARY
  "talipot-python-${TALIPOT_MAJOR_VERSION}${TalipotVersionSeparator}${TALIPOT_MINOR_VERSION}"
  PATHS ${TALIPOT_LIBRARIES_DIR}
  NO_DEFAULT_PATH)

IF((TALIPOT_OGL_LIBRARY AND NOT TALIPOT_CORE_LIBRARY)
   OR (TALIPOT_GUI_LIBRARY AND NOT TALIPOT_OGL_LIBRARY)
   OR (TALIPOT_OGDF_LIBRARY AND NOT TALIPOT_CORE_LIBRARY)
   OR (TALIPOT_PYTHON_LIBRARY AND NOT TALIPOT_CORE_LIBRARY))
  MESSAGE(
    "Dependency check between Talipot libraries failed. Detected libraries are:"
  )
  MESSAGE("TalipotCore: ${TALIPOT_CORE_LIBRARY}")
  MESSAGE("TalipotOGL: ${TALIPOT_OGL_LIBRARY} (depends on TalipotCore)")
  MESSAGE("TalipotGUI: ${TALIPOT_GUI_LIBRARY} (depends on TalipotOGL)")
  MESSAGE("TalipotOGDF: ${TALIPOT_OGDF_LIBRARY} (depends on TalipotCore)")
  MESSAGE("TalipotPython: ${TALIPOT_PYTHON_LIBRARY} (depends on TalipotCore)")
  MESSAGE(
    FATAL_ERROR "Please check that dependencies for detected libraries are met."
  )
ENDIF()

SET(TALIPOT_LIBRARIES)

IF(TALIPOT_CORE_LIBRARY)
  MARK_AS_ADVANCED(TALIPOT_CORE_LIBRARY)
  SET(TALIPOT_LIBRARIES ${TALIPOT_LIBRARIES} ${TALIPOT_CORE_LIBRARY})
ENDIF(TALIPOT_CORE_LIBRARY)

IF(TALIPOT_OGL_LIBRARY)
  MARK_AS_ADVANCED(TALIPOT_OGL_LIBRARY)
  SET(TALIPOT_LIBRARIES ${TALIPOT_LIBRARIES} ${TALIPOT_OGL_LIBRARY})
ENDIF(TALIPOT_OGL_LIBRARY)

IF(TALIPOT_GUI_LIBRARY)
  MARK_AS_ADVANCED(TALIPOT_GUI_LIBRARY)
  SET(TALIPOT_LIBRARIES ${TALIPOT_LIBRARIES} ${TALIPOT_GUI_LIBRARY})
ENDIF(TALIPOT_GUI_LIBRARY)

IF(TALIPOT_OGDF_LIBRARY)
  MARK_AS_ADVANCED(TALIPOT_OGDF_LIBRARY)
  SET(TALIPOT_LIBRARIES ${TALIPOT_LIBRARIES} ${TALIPOT_OGDF_LIBRARY})
ENDIF(TALIPOT_OGDF_LIBRARY)

IF(TALIPOT_PYTHON_LIBRARY)
  MARK_AS_ADVANCED(TALIPOT_PYTHON_LIBRARY)
ENDIF(TALIPOT_PYTHON_LIBRARY)

SET(TALIPOT_USE_FILE "${CMAKE_CURRENT_LIST_DIR}/TalipotUseFile.cmake")

INCLUDE(${TALIPOT_USE_FILE})
