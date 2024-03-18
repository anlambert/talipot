FIND_PACKAGE(CppUnit REQUIRED)

MACRO(UNIT_TEST)
  SET(TEST_NAME ${ARGV0})
  UNSET(TEST_SRCS)

  FOREACH(loop_var ${ARGN})
    STRING(REGEX MATCH ".*\\.cpp" TEST_SRC_FILE ${loop_var})

    IF(TEST_SRC_FILE)
      SET(TEST_SRCS ${TEST_SRCS} ${loop_var})
    ENDIF(TEST_SRC_FILE)
  ENDFOREACH()

  # this is a cpp[unit] test
  IF(TEST_SRCS)
    INCLUDE_DIRECTORIES(
      ${TalipotCoreBuildInclude} ${TalipotCoreInclude} ${CppUnit_INCLUDE_DIRS}
      ${CMAKE_SOURCE_DIR}/utils/crash_handler ${CMAKE_SOURCE_DIR}/tests/include)
    LINK_DIRECTORIES(${CppUnit_LIBRARY_DIRS})
    ADD_EXECUTABLE(${TEST_NAME} ${TEST_SRCS})
    ADD_DEPENDENCIES(tests ${TEST_NAME})
    TARGET_LINK_LIBRARIES(${TEST_NAME} ${LibTalipotCoreName} crash_handler
                          ${CppUnit_LIBRARIES})
    ADD_TEST(NAME ${TEST_NAME} COMMAND ${TEST_NAME} ${TEST_NAME})

    # otherwise this is a Python test
  ELSE()
    SET(PYTHON_TEST_MODULE ${ARGV1})
    ADD_TEST(NAME ${TEST_NAME} COMMAND ${PYTHON_EXECUTABLE} -m unittest
                                       ${PYTHON_TEST_MODULE})
  ENDIF()

  # In order to run the tests on windows, we need to add the paths of the dll
  # dependencies (cppunit, tulip, gzstream, yajl) in the PATH environment
  # variable
  IF(WIN32)
    IF(NOT MSVC)
      STRING(REGEX MATCH ".*dll\\.a" IMPORT_LIBRARY ${CppUnit_LIBRARIES})
      STRING(REGEX MATCH ".*dll" DLL_LIBRARY ${CppUnit_LIBRARIES})

      IF(IMPORT_LIBRARY)
        TALIPOT_GET_DLL_NAME_FROM_IMPORT_LIBRARY(${CppUnit_LIBRARIES}
                                                 CppUnitDllName)
      ELSEIF(DLL_LIBRARY)
        GET_FILENAME_COMPONENT(CppUnitDllName ${CppUnit_LIBRARIES} NAME)
      ENDIF()
    ELSE(NOT MSVC)
      TALIPOT_GET_DLL_NAME_FROM_IMPORT_LIBRARY(${CppUnit_LIBRARIES}
                                               CppUnitDllName)
    ENDIF(NOT MSVC)

    IF(CppUnitDllName)
      TALIPOT_FIND_EXTERNAL_LIB(${CppUnitDllName} CppUnitDllPath)

      IF(CppUnitDllPath)
        GET_FILENAME_COMPONENT(CppUnitDllDir ${CppUnitDllPath} DIRECTORY)
      ENDIF(CppUnitDllPath)
    ENDIF()

    SET(NEWPATH
        ${CppUnitDllDir}
        ${CMAKE_LIBRARY_PATH}
        ${QT_BINARY_DIR}
        ${PYTHON_HOME_PATH};${CMAKE_BINARY_DIR}/thirdparty/gzstream/
        ${CMAKE_BINARY_DIR}/thirdparty/ftgl
        ${CMAKE_BINARY_DIR}/library/talipot-core/src
        ${CMAKE_BINARY_DIR}/library/talipot-ogl/src
        ${CMAKE_BINARY_DIR}/library/talipot-gui/src
        ${CMAKE_BINARY_DIR}/thirdparty/libtess2
        ${CMAKE_BINARY_DIR}/thirdparty/OGDF
        ${CMAKE_BINARY_DIR}/thirdparty/quazip-1.4/quazip
        ${CMAKE_BINARY_DIR}/library/talipot-ogdf
        ${CMAKE_BINARY_DIR}/library/talipot-python/src
        $ENV{PATH})
    STRING(REPLACE "\\;" ";" NEWPATH "${NEWPATH}")
    STRING(REPLACE ";" "\\;" NEWPATH "${NEWPATH}")
  ELSE(WIN32)
    SET(NEWPATH "$ENV{PATH}")
  ENDIF(WIN32)

  # add Talipot modules path to PYTHONPATH in order to run the Python tests
  SET(PYTHONPATH
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/library/talipot-python/modules/
      ${TALIPOT_PYTHON_ROOT_FOLDER})

  IF(WIN32)
    STRING(REPLACE "\\;" ";" PYTHONPATH "${PYTHONPATH}")
    STRING(REPLACE ";" "\\;" PYTHONPATH "${PYTHONPATH}")
  ELSE(WIN32)
    STRING(REPLACE ";" ":" PYTHONPATH "${PYTHONPATH}")
  ENDIF(WIN32)

  # set tests environment
  SET_TESTS_PROPERTIES(
    ${TEST_NAME}
    PROPERTIES
      ENVIRONMENT
      "TALIPOT_BUILD_DIR=${CMAKE_BINARY_DIR};PYTHONPATH=${PYTHONPATH};PATH=${NEWPATH}"
  )
ENDMACRO(UNIT_TEST)

ADD_SUBDIRECTORY(library)
ADD_SUBDIRECTORY(plugins)
ADD_SUBDIRECTORY(python)
ADD_SUBDIRECTORY(external_plugins_build)
