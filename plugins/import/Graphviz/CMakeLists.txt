INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${Graphviz_INCLUDE_DIRECTORIES})

IF(MSVC)
  ADD_DEFINITIONS(-DGVDLL)
ENDIF(MSVC)

TALIPOT_ADD_PLUGIN(
  NAME
  GraphvizImport
  SRCS
  GraphvizImport.cpp
  LINKS
  ${LibTalipotCoreName}
  ${Graphviz_LIBRARIES}
  graphviz_utils
  INSTALL_DIR
  ${TalipotPluginsInstallDir}
  FIXUP_INSTALL)
