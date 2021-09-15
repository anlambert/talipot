INCLUDE_DIRECTORIES(${TalipotCoreBuildInclude} ${TalipotCoreInclude})

# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  SpanningDagSelection
  SRCS
  SpanningDagSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ReachableSubGraphSelection
  SRCS
  ReachableSubGraphSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  SpanningTreeSelection
  SRCS
  SpanningTreeSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InducedSubGraphSelection
  SRCS
  InducedSubGraphSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  LoopSelection
  SRCS
  LoopSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ParallelEdgesSelection
  SRCS
  ParallelEdgesSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  Kruskal
  SRCS
  Kruskal.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  MakeSelectionGraph
  SRCS
  MakeSelectionGraph.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  BridgesSelection
  SRCS
  BridgesSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
