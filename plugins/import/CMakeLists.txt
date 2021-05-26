ADD_SUBDIRECTORY(SocialNetwork)
ADD_SUBDIRECTORY(BibTeX)
ADD_SUBDIRECTORY(Graphviz)
ADD_SUBDIRECTORY(Git)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${TalipotGUIInclude})

# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ERImport
  SRCS
  ERRandomGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  PlanarGraph
  SRCS
  PlanarGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  Grid
  SRCS
  Grid.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  GMLImport
  SRCS
  GMLImport.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  RandomGraph
  SRCS
  RandomGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  AdjacencyMatrixImport
  SRCS
  AdjacencyMatrixImport.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  CompleteGraph
  SRCS
  CompleteGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  RandomTree
  SRCS
  RandomTree.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  RandomTreeGeneral
  SRCS
  RandomTreeGeneral.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  CompleteTree
  SRCS
  CompleteTree.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  SmallWorldGraph
  SRCS
  SmallWorldGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ImportPajek
  SRCS
  ImportPajek.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ImportUCINET
  SRCS
  ImportUCINET.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  EmptyGraph
  SRCS
  EmptyGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
IF(NOT TALIPOT_BUILD_CORE_ONLY)
  QTX_SET_INCLUDES_AND_DEFINITIONS()

  TALIPOT_ADD_PLUGIN(
    NAME
    FileSystem
    SRCS
    FileSystem.cpp
    LINKS
    ${LibTalipotCoreName}
    ${QT_LIBRARIES}
    INSTALL_DIR
    ${TalipotPluginsInstallDir})
  # ----------------------------------------------------------------------------
  TALIPOT_ADD_PLUGIN(
    NAME
    GEXFImport
    SRCS
    GEXFImport.cpp
    LINKS
    ${LibTalipotCoreName}
    ${QT_LIBRARIES}
    INSTALL_DIR
    ${TalipotPluginsInstallDir})

ENDIF(NOT TALIPOT_BUILD_CORE_ONLY)
