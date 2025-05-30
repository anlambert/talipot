INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${Graphviz_INCLUDE_DIRECTORIES})

IF(MSVC)
  ADD_COMPILE_DEFINITIONS(GVDLL)
ENDIF(MSVC)

TALIPOT_ADD_PLUGIN(
  NAME
  CircoLayout
  SRCS
  CircoLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

TALIPOT_ADD_PLUGIN(
  NAME
  DotLayout
  SRCS
  DotLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

TALIPOT_ADD_PLUGIN(
  NAME
  FdpLayout
  SRCS
  FdpLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

TALIPOT_ADD_PLUGIN(
  NAME
  NeatoLayout
  SRCS
  NeatoLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

TALIPOT_ADD_PLUGIN(
  NAME
  SfdpLayout
  SRCS
  SfdpLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

TALIPOT_ADD_PLUGIN(
  NAME
  TwopiLayout
  SRCS
  TwopiLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)

IF(WIN32)
  MACRO(INSTALL_GRAPHVIZ_PLUGIN GV_PLUGIN_IMPORT_LIB)
    TALIPOT_GET_DLL_NAME_FROM_IMPORT_LIBRARY(${GV_PLUGIN_IMPORT_LIB}
                                             GV_PLUGIN_DLL_NAME)
    TALIPOT_FIND_EXTERNAL_LIB(${GV_PLUGIN_DLL_NAME} GV_PLUGIN_DLL_PATH)
    INSTALL(FILES ${GV_PLUGIN_DLL_PATH} DESTINATION ${TalipotBinInstallDir})
    SET_PROPERTY(
      GLOBAL APPEND
      PROPERTY
        FIXUP_BUNDLE_LIBS
        "${CMAKE_INSTALL_PREFIX}/${TalipotBinInstallDir}/${GV_PLUGIN_DLL_NAME}")
  ENDMACRO()

  INSTALL_GRAPHVIZ_PLUGIN(${Graphviz_GVPLUGIN_CORE_LIBRARY})
  INSTALL_GRAPHVIZ_PLUGIN(${Graphviz_GVPLUGIN_DOT_LAYOUT_LIBRARY})
  INSTALL_GRAPHVIZ_PLUGIN(${Graphviz_GVPLUGIN_NEATO_LAYOUT_LIBRARY})
  GET_FILENAME_COMPONENT(GV_PLUGINS_DIR ${GV_PLUGIN_DLL_PATH} PATH)
  INSTALL(FILES ${GV_PLUGINS_DIR}/config6 DESTINATION ${TalipotBinInstallDir})
ENDIF(WIN32)
