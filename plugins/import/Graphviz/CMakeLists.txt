INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${Graphviz_INCLUDE_DIRECTORIES})

TALIPOT_ADD_PLUGIN(
  NAME
  GraphvizImport
  SRCS
  GraphvizImport.cpp
  GraphvizUtils.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)
