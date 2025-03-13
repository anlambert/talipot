# After finding the Python interpreter, try to find if SIP and its dev tools are
# installed on the host system. If not, compile the SIP version located in
# thirdparty.

IF(LINUX AND TALIPOT_BUILD_PYTHON_WHEEL)
  SET(PYTHON_COMPONENTS Interpreter Development.Module)
ELSE(LINUX AND TALIPOT_BUILD_PYTHON_WHEEL)
  SET(PYTHON_COMPONENTS Interpreter Development)
ENDIF(LINUX AND TALIPOT_BUILD_PYTHON_WHEEL)

FIND_PACKAGE(
  Python3
  COMPONENTS ${PYTHON_COMPONENTS}
  REQUIRED)

SET(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
SET(PYTHON_LIBRARY ${Python3_LIBRARIES})
SET(PYTHON_INCLUDE_DIR ${Python3_INCLUDE_DIRS})

EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} --version
  OUTPUT_VARIABLE PYTHON_VERSION_RAW
  ERROR_VARIABLE PYTHON_VERSION_RAW)
STRING(REPLACE "\n" "" PYTHON_VERSION_RAW "${PYTHON_VERSION_RAW}")
STRING(REGEX MATCH "[0-9]\\.[0-9]+" PYTHON_VERSION "${PYTHON_VERSION_RAW}")
STRING(REGEX MATCH "[0-9]\\.[0-9]+\\.[0-9]+" PYTHON_VERSION_WITH_PATCH
             "${PYTHON_VERSION_RAW}")
STRING(REPLACE "." "" PYTHON_VERSION_NO_DOT ${PYTHON_VERSION})

EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} -c
          "import os; print(os.path.dirname(os.__file__))"
  OUTPUT_VARIABLE PYTHON_STDLIB_DIR)
STRING(REPLACE "\n" "" PYTHON_STDLIB_DIR "${PYTHON_STDLIB_DIR}")

IF(NOT TALIPOT_BUILD_PYTHON_WHEEL)
  SET(TalipotPythonModulesInstallDir
      ${CMAKE_INSTALL_PREFIX}/${TalipotLibInstallDir}/talipot/python)
ELSE(NOT TALIPOT_BUILD_PYTHON_WHEEL)
  SET(TalipotPythonModulesInstallDir ${CMAKE_INSTALL_PREFIX})
ENDIF(NOT TALIPOT_BUILD_PYTHON_WHEEL)

# Unset the previous values of the CMake cache variables related to Python
# libraries in case the value of PYTHON_EXECUTABLE CMake variable changed
UNSET(PYTHONLIBS_FOUND CACHE)
UNSET(PYTHON_LIBRARY CACHE)
UNSET(PYTHON_INCLUDE_DIR CACHE)
UNSET(PYTHON_INCLUDE_PATH CACHE)

# Find the Python library with the same version as the interpreter Python 3.2
# library is suffixed by mu and Python >= 3.3 by m on some systems, also handle
# these cases
SET(Python_ADDITIONAL_VERSIONS ${PYTHON_VERSION}mu ${PYTHON_VERSION}m
                               ${PYTHON_VERSION})

GET_FILENAME_COMPONENT(PYTHON_HOME_PATH ${PYTHON_EXECUTABLE} PATH)

# Ensure the detection of Python library associated to the selected interpreter
IF(APPLE)
  SET(CMAKE_PREFIX_PATH
      ${PYTHON_HOME_PATH}/../Frameworks/Python.framework/Versions/${PYTHON_VERSION}
      /Library/Frameworks/Python.framework/Versions/${PYTHON_VERSION}
      ${CMAKE_PREFIX_PATH})
ENDIF(APPLE)

# Ensure that correct Python include path is selected by CMake on Windows
IF(WIN32)
  SET(CMAKE_INCLUDE_PATH ${PYTHON_HOME_PATH}/include ${CMAKE_INCLUDE_PATH})

  # Ensure that correct Python include path and library are selected by CMake on
  # Linux (in case of non standard installation)
ELSEIF(LINUX)
  SET(CMAKE_INCLUDE_PATH ${PYTHON_HOME_PATH}/../include ${CMAKE_INCLUDE_PATH})
  SET(CMAKE_LIBRARY_PATH ${PYTHON_HOME_PATH}/../lib ${CMAKE_LIBRARY_PATH})
ENDIF(WIN32)

IF(NOT WIN32 AND PYTHON_LIBRARY)
  GET_FILENAME_COMPONENT(PYTHON_LIB_PATH ${PYTHON_LIBRARY} PATH)
ENDIF(NOT WIN32 AND PYTHON_LIBRARY)

IF(WIN32 AND NOT MSVC)
  # Check if Python is provided by MSYS2 (it is compiled with GCC in that case
  # instead of MSVC)
  EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.version)"
    OUTPUT_VARIABLE PYTHON_VERSION_FULL
    ERROR_VARIABLE PYTHON_VERSION_FULL)
  STRING(REGEX MATCH "GCC" MSYS2_PYTHON "${PYTHON_VERSION_FULL}")

  # Python 64bits does not provide a dll import library for MinGW. Fortunately,
  # we can directly link to the Python dll with that compiler. So find the
  # location of that dll and overwrite the PYTHON_LIBRARY CMake cache variable
  # with it
  STRING(REPLACE "\\" "/" WINDIR $ENV{WINDIR})

  IF(MSYS2_PYTHON)
    IF(EXISTS ${PYTHON_HOME_PATH}/libpython${PYTHON_VERSION}.dll)
      SET(PYTHON_LIBRARY
          ${PYTHON_HOME_PATH}/libpython${PYTHON_VERSION}.dll
          CACHE FILEPATH "" FORCE)
    ELSEIF(EXISTS ${PYTHON_HOME_PATH}/libpython${PYTHON_VERSION}m.dll)
      SET(PYTHON_LIBRARY
          ${PYTHON_HOME_PATH}/libpython${PYTHON_VERSION}m.dll
          CACHE FILEPATH "" FORCE)
    ENDIF(EXISTS ${PYTHON_HOME_PATH}/libpython${PYTHON_VERSION}.dll)
  ELSE(MSYS2_PYTHON)
    # Check if the Python dll is located in the Python home directory (when
    # Python is installed for current user only)
    IF(EXISTS ${PYTHON_HOME_PATH}/python${PYTHON_VERSION_NO_DOT}.dll)
      SET(PYTHON_LIBRARY
          ${PYTHON_HOME_PATH}/python${PYTHON_VERSION_NO_DOT}.dll
          CACHE FILEPATH "" FORCE)

      # If not, the Python dll is located in %WINDIR%/System32 (when Python is
      # installed for all users)
    ELSE(EXISTS ${PYTHON_HOME_PATH}/python${PYTHON_VERSION_NO_DOT}.dll)
      IF(NOT WIN_AMD64 OR X64)
        SET(PYTHON_LIBRARY
            ${WINDIR}/System32/python${PYTHON_VERSION_NO_DOT}.dll
            CACHE FILEPATH "" FORCE)
      ELSE(NOT WIN_AMD64 OR X64)
        SET(PYTHON_LIBRARY
            ${WINDIR}/SysWOW64/python${PYTHON_VERSION_NO_DOT}.dll
            CACHE FILEPATH "" FORCE)
      ENDIF(NOT WIN_AMD64 OR X64)
    ENDIF(EXISTS ${PYTHON_HOME_PATH}/python${PYTHON_VERSION_NO_DOT}.dll)
  ENDIF(MSYS2_PYTHON)
ENDIF(WIN32 AND NOT MSVC)

# Ensure headers correspond to the ones associated to the detected Python
# library on MacOS
IF(APPLE)
  IF("${PYTHON_LIBRARY}" MATCHES
     "^/Library/Frameworks/Python.framework/Versions/${PYTHON_VERSION}.*$")
    SET(PYTHON_INCLUDE_DIR
        /Library/Frameworks/Python.framework/Versions/${PYTHON_VERSION}/Headers
        CACHE PATH "" FORCE)
  ENDIF()
ENDIF(APPLE)

FIND_PACKAGE(SIP 6.8.5 REQUIRED)
