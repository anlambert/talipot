INCLUDE(GetPrerequisites)

SET(TALIPOT_NATIVE_PYTHON_PATH "${CWD}/talipot/native")

FILE(GLOB TALIPOT_NATIVE_PYTHON_MODULE
     "${TALIPOT_NATIVE_PYTHON_PATH}/talipot*.pyd")

FILE(GLOB TALIPOT_DLL_PLUGINS "${TALIPOT_NATIVE_PYTHON_PATH}/plugins/*.dll")
FILE(GLOB TALIPOT_GRAPHVIZ_DLL_PLUGINS
     "${TALIPOT_NATIVE_PYTHON_PATH}/graphviz/*.dll")

STRING(REPLACE ";" "\;" LIBRARY_PATHS "${LIBRARY_PATHS}")

SET(DIRS "${TALIPOT_NATIVE_PYTHON_PATH}\;${LIBRARY_PATHS}")

MESSAGE("Gathering dll dependencies for talipot Python module ...")

# Get top level prerequisites (no recursion as it fails with python >= 3.4)
GET_PREREQUISITES(${TALIPOT_NATIVE_PYTHON_MODULE}
                  TALIPOT_NATIVE_PYTHON_MODULE_TL_DEPS 1 0 "" ${DIRS})

# The DLL_DEP_RESOLVED variable is not set when calling GP_RESOLVE_ITEM even if
# the item is correctly resolved (I added some debug messages to
# GetPrerequisites.cmake to verify). My guess is PARENT_SCOPE is not honored
# when executing cmake in script mode ($ cmake -P ...). Hopefully, CMake
# provides a hook mechanism called at the end of the GP_RESOLVE_ITEM process
# that enables us to get the resolved item

# Store the resolved top level items for the talipot module in an environment
# variable (as PARENT_SCOPE does not seem to work when executing CMake in script
# mode ($ cmake -P ...)
FUNCTION(
  gp_resolve_item_override
  context
  item
  exepath
  dirs
  resolved_item_var
  resolved)
  IF(DEFINED ENV{TALIPOT_NATIVE_PYTHON_MODULE_DEPS})
    SET(ENV{TALIPOT_NATIVE_PYTHON_MODULE_DEPS}
        "$ENV{TALIPOT_NATIVE_PYTHON_MODULE_DEPS};${${resolved_item_var}}")
  ELSE()
    SET(ENV{TALIPOT_NATIVE_PYTHON_MODULE_DEPS} "${${resolved_item_var}}")
  ENDIF()
ENDFUNCTION()

# Append the top level prerequisites items to the
# TALIPOT_NATIVE_PYTHON_MODULE_DEPS list and resolve them (get their absolute
# path)
FOREACH(DLL_DEP ${TALIPOT_NATIVE_PYTHON_MODULE_TL_DEPS})
  IF(NOT "${DLL_DEP}" MATCHES "^python.*\\.dll$")
    LIST(APPEND TALIPOT_NATIVE_PYTHON_MODULE_DEPS ${DLL_DEP})
    GP_RESOLVE_ITEM(${TALIPOT_NATIVE_PYTHON_MODULE} ${DLL_DEP} "" ${DIRS}
                    DLL_DEP_RESOLVED)
  ENDIF()
ENDFOREACH()

FUNCTION(
  gp_resolve_item_override
  context
  item
  exepath
  dirs
  resolved_item_var
  resolved)

ENDFUNCTION()

# Get recursively prerequisites of top level prerequisites and append them to
# the TALIPOT_NATIVE_PYTHON_MODULE_DEPS list
FOREACH(DLL_DEP $ENV{TALIPOT_NATIVE_PYTHON_MODULE_DEPS} ${TALIPOT_DLL_PLUGINS}
                ${TALIPOT_GRAPHVIZ_DLL_PLUGINS})
  GET_PREREQUISITES(${DLL_DEP} DLL_DEP_DEPS 1 1 "" ${DIRS})

  FOREACH(DLL_DEP_DEP ${DLL_DEP_DEPS})
    LIST(APPEND TALIPOT_NATIVE_PYTHON_MODULE_DEPS ${DLL_DEP_DEP})
  ENDFOREACH()
ENDFOREACH()

# Remove duplicate entries in the TALIPOT_NATIVE_PYTHON_MODULE_DEPS list
LIST(REMOVE_DUPLICATES TALIPOT_NATIVE_PYTHON_MODULE_DEPS)

FUNCTION(
  gp_resolve_item_override
  context
  item
  exepath
  dirs
  resolved_item_var
  resolved)
  IF(NOT EXISTS "${TALIPOT_NATIVE_PYTHON_PATH}/${item}" AND NOT item MATCHES
                                                            "^python.*\\.dll")
    MESSAGE("Copying ${item} inside talipot Python module native folder")
    FILE(COPY ${${resolved_item_var}} DESTINATION ${TALIPOT_NATIVE_PYTHON_PATH})
  ENDIF()
ENDFUNCTION()

# Finally, resolve all prerequisites for the talipot Python module and copy them
# to the module native folder
FOREACH(DLL_DEP ${TALIPOT_NATIVE_PYTHON_MODULE_DEPS})
  GP_RESOLVE_ITEM(${TALIPOT_NATIVE_PYTHON_MODULE} ${DLL_DEP} "" ${DIRS}
                  DLL_DEP_RESOLVED)
ENDFOREACH()
