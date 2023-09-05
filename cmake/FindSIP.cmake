FIND_PACKAGE(Python3 REQUIRED)

SET(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})

FIND_FILE(
  find_sip_py FindSIP.py
  PATHS ${CMAKE_MODULE_PATH}
  NO_CMAKE_FIND_ROOT_PATH)

EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${find_sip_py}
                OUTPUT_VARIABLE sip_config)
IF(sip_config)
  STRING(REGEX REPLACE "^sip_version:([^\n]+).*$" "\\1" SIP_VERSION
                       ${sip_config})
  STRING(REGEX REPLACE ".*\nsip_version_num:([^\n]+).*$" "\\1" SIP_VERSION_NUM
                       ${sip_config})
  STRING(REGEX REPLACE ".*\nsip_version_str:([^\n]+).*$" "\\1" SIP_VERSION_STR
                       ${sip_config})
  STRING(REGEX REPLACE ".*\nsip_module_version:([^\n]+).*$" "\\1"
                       SIP_MODULE_VERSION ${sip_config})
  STRING(REGEX REPLACE ".*\ndefault_sip_dir:([^\n]+).*$" "\\1"
                       SIP_DEFAULT_SIP_DIR ${sip_config})

  SET(SIP_PATHS ${PYTHON_LIB_PATH}/../bin ${PYTHON_HOME_PATH}
                ${PYTHON_HOME_PATH}/Scripts)

  FIND_PROGRAM(SIP_BUILD_EXECUTABLE sip-build PATHS ${SIP_PATHS})
  FIND_PROGRAM(SIP_MODULE_EXECUTABLE sip-module PATHS ${SIP_PATHS})

  SET(SIP_FOUND TRUE)
ENDIF(sip_config)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  SIP
  REQUIRED_VARS SIP_BUILD_EXECUTABLE SIP_MODULE_EXECUTABLE
  VERSION_VAR SIP_VERSION_STR)
