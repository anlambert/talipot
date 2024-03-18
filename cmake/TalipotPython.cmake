# After finding the Python interpreter, try to find if SIP and its dev tools are
# installed on the host system. If not, compile the SIP version located in
# thirdparty.

IF(LINUX AND TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
  SET(PYTHON_COMPONENTS Interpreter)
ELSE(LINUX AND TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
  SET(PYTHON_COMPONENTS Interpreter Development)
ENDIF(LINUX AND TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)

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

SET(TalipotPythonModulesInstallDir
    ${CMAKE_INSTALL_PREFIX}/${TalipotLibInstallDir}/talipot/python)

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

FIND_PACKAGE(SIP 6.8.1 REQUIRED)

IF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
  SET(TALIPOT_PYTHON_WHEEL_VERSION
      "${TalipotMajorVersion}.${TalipotMinorVersion}.${TalipotReleaseVersion}")

  IF(WIN32)
    SET(WHEEL_INSTALL_PATH "\\")
  ELSE(WIN32)
    SET(WHEEL_INSTALL_PATH "/")
  ENDIF(WIN32)

  ADD_CUSTOM_TARGET(
    wheel
    COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel
    WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER})

  # check generation of test wheels
  STRING(COMPARE NOTEQUAL "${TALIPOT_PYTHON_TEST_WHEEL_SUFFIX}" ""
                 TALIPOT_GENERATE_TESTPYPI_WHEEL)

  IF(TALIPOT_GENERATE_TESTPYPI_WHEEL)
    SET(TALIPOT_PYTHON_TEST_WHEEL_VERSION
        ${TALIPOT_PYTHON_WHEEL_VERSION}${TALIPOT_PYTHON_TEST_WHEEL_SUFFIX})

    ADD_CUSTOM_TARGET(
      test-wheel
      COMMAND ${PYTHON_EXECUTABLE} setuptest.py bdist_wheel
      WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER})
    ADD_DEPENDENCIES(test-wheel wheel)
  ENDIF(TALIPOT_GENERATE_TESTPYPI_WHEEL)

  IF(NOT LINUX)
    EXECUTE_PROCESS(
      COMMAND ${PYTHON_EXECUTABLE} -c "import wheel"
      RESULT_VARIABLE WHEEL_OK
      OUTPUT_QUIET ERROR_QUIET)
    EXECUTE_PROCESS(
      COMMAND ${PYTHON_EXECUTABLE} -c "import twine"
      RESULT_VARIABLE TWINE_OK
      OUTPUT_QUIET ERROR_QUIET)

    IF(NOT WHEEL_OK EQUAL 0)
      MESSAGE(
        "The 'wheel' Python module has to be installed to generate wheels for "
        "talipot modules.")
      MESSAGE("You can install it through the 'pip' tool ($ pip install wheel)")
    ENDIF(NOT WHEEL_OK EQUAL 0)

    IF(NOT TWINE_OK EQUAL 0)
      MESSAGE(
        "The 'twine' Python module has to be installed to upload talipot wheels"
        " on PyPi.")
      MESSAGE("You can install it through the 'pip' tool ($ pip install twine)")
    ENDIF(NOT TWINE_OK EQUAL 0)

  ELSE(NOT LINUX)
    IF(EXISTS ${PYTHON_HOME_PATH}/../include/python${PYTHON_VERSION}m)
      SET(PYTHON_INCLUDE_DIR
          ${PYTHON_HOME_PATH}/../include/python${PYTHON_VERSION}m
          CACHE PATH "" FORCE)
    ELSE()
      SET(PYTHON_INCLUDE_DIR
          ${PYTHON_HOME_PATH}/../include/python${PYTHON_VERSION}
          CACHE PATH "" FORCE)
    ENDIF()

    IF(NOT EXISTS ${PYTHON_HOME_PATH}/wheel)
      EXECUTE_PROCESS(COMMAND ${PYTHON_HOME_PATH}/pip install --upgrade wheel)
    ENDIF(NOT EXISTS ${PYTHON_HOME_PATH}/wheel)

    IF(NOT EXISTS ${PYTHON_HOME_PATH}/twine)
      EXECUTE_PROCESS(COMMAND ${PYTHON_HOME_PATH}/pip install --upgrade twine)
    ENDIF(NOT EXISTS ${PYTHON_HOME_PATH}/twine)

    # When building Python binary wheels on Linux, produced binaries have to be
    # patched in order for the talipot module to be successfully imported and
    # loaded on every computer. The auditwheel tool has been developed in order
    # to ease that patching task (see https://github.com/pypa/auditwheel).
    ADD_CUSTOM_COMMAND(
      TARGET wheel
      POST_BUILD
      COMMAND
        bash -c
        "auditwheel repair -L /native -w ./dist ./dist/$(ls -t ./dist/ | head -1)"
      COMMAND bash -c "rm ./dist/$(ls -t ./dist/ | head -2 | tail -1)"
      WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER}
      COMMENT "Repairing linux talipot wheel"
      VERBATIM)

    IF(TALIPOT_GENERATE_TESTPYPI_WHEEL)
      ADD_CUSTOM_COMMAND(
        TARGET test-wheel
        POST_BUILD
        COMMAND
          bash -c
          "auditwheel repair -L /native -w ./dist ./dist/$(ls -t ./dist/ | head -1)"
        COMMAND bash -c "rm ./dist/$(ls -t ./dist/ | head -2 | tail -1)"
        WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER}
        COMMENT "Repairing linux talipot test wheel"
        VERBATIM)
    ENDIF(TALIPOT_GENERATE_TESTPYPI_WHEEL)
  ENDIF(NOT LINUX)

  # In order to upload the generated wheels, an account must be created on PyPi
  # and the following configuration must be stored in the ~/.pypirc file
  # ############################################################################
  # [distutils] index-servers= pypi testpypi
  #
  # [testpypi] repository: https://test.pypi.org/legacy/ username: <your user
  # name goes here> password: <your password goes here>
  #
  # [pypi] repository: https://upload.pypi.org/legacy/ username: <your user name
  # goes here> password: <your password goes here>
  # ############################################################################
  SET(TWINE twine)

  IF(EXISTS ${PYTHON_HOME_PATH}/twine)
    SET(TWINE ${PYTHON_HOME_PATH}/twine)
  ENDIF(EXISTS ${PYTHON_HOME_PATH}/twine)

  IF(WIN32)
    SET(TWINE ${PYTHON_INCLUDE_DIR}/../Scripts/twine.exe)
  ENDIF(WIN32)

  SET(WHEEL_FILES_REGEXP "*${TALIPOT_PYTHON_WHEEL_VERSION}-cp*")
  ADD_CUSTOM_TARGET(
    wheel-upload
    COMMAND bash -c "echo 'Uploading wheels ...'"
    COMMAND ${TWINE} upload -r pypi dist/${WHEEL_FILES_REGEXP}
    WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER}
    VERBATIM)

  IF(TALIPOT_GENERATE_TESTPYPI_WHEEL)
    SET(TEST_WHEEL_FILES_REGEXP "*${TALIPOT_PYTHON_TEST_WHEEL_VERSION}*")
    ADD_CUSTOM_TARGET(
      test-wheel-upload
      COMMAND bash -c "echo 'Uploading test wheels ...'"
      COMMAND ${TWINE} upload -r testpypi dist/${TEST_WHEEL_FILES_REGEXP}
      WORKING_DIRECTORY ${TALIPOT_PYTHON_ROOT_FOLDER}
      VERBATIM)
  ENDIF(TALIPOT_GENERATE_TESTPYPI_WHEEL)
ENDIF(TALIPOT_ACTIVATE_PYTHON_WHEEL_TARGET)
