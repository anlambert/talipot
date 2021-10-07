ADD_SUBDIRECTORY(OGDF)
ADD_SUBDIRECTORY(Grip)
ADD_SUBDIRECTORY(LinLog)

INCLUDE_DIRECTORIES(${TalipotCoreBuildInclude} ${TalipotCoreInclude})

SET(LayoutUtils_SRCS DatasetTools.cpp OrientableCoord.cpp OrientableLayout.cpp
                     OrientableSize.cpp OrientableSizeProxy.cpp Orientation.cpp)

IF(NOT EMSCRIPTEN)
  SET(LayoutUtilsLibraryName LayoutUtils)
  ADD_LIBRARY(${LayoutUtilsLibraryName} STATIC ${LayoutUtils_SRCS})
  SET_TARGET_PROPERTIES(${LayoutUtilsLibraryName}
                        PROPERTIES POSITION_INDEPENDENT_CODE ON)
  ADD_DEPENDENCIES(${LayoutUtilsLibraryName} ${LibTalipotCoreName})
ELSE(NOT EMSCRIPTEN)
  SET(LayoutUtilsLibraryName "")
ENDIF(NOT EMSCRIPTEN)

# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  MixedModel
  SRCS
  MixedModel.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
SET(ConnectedComponentsPacking_SRCS
    ConnectedComponentsPacking.cpp Number.cpp RectanglePacking.cpp
    rectanglePackingFonctions.cpp RectangleRelativePositionList.cpp)

TALIPOT_ADD_PLUGIN(
  NAME
  ConnectedComponentsPacking
  SRCS
  ${ConnectedComponentsPacking_SRCS}
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  RandomLayout
  SRCS
  Random.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  GEMLayout
  SRCS
  GEMLayout.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  TreeReingoldAndTilfordExtended
  SRCS
  TreeReingoldAndTilfordExtended.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ConeTreeExtended
  SRCS
  ConeTreeExtended.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  TreeRadial
  SRCS
  TreeRadial.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  TreeLeaf
  SRCS
  TreeLeaf.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  BubblePack
  SRCS
  BubblePack.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  BubbleTree
  SRCS
  BubbleTree.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  Circular
  SRCS
  Circular.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  HierarchicalGraph
  SRCS
  HierarchicalGraph.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  Tutte
  SRCS
  Tutte.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  Dendrogram
  SRCS
  Dendrogram.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  ImprovedWalker
  SRCS
  ImprovedWalker.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  SquarifiedTreeMap
  SRCS
  SquarifiedTreeMap.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  PerfectLayout
  SRCS
  PerfectLayout.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  OrthoTree
  SRCS
  OrthoTree.cpp
  LINKS
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  PolyominoPacking
  SRCS
  PolyominoPacking.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
# ------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${VPSCInclude})

TALIPOT_ADD_PLUGIN(
  NAME
  FastOverlapRemoval
  SRCS
  FastOverlapRemoval.cpp
  LINKS
  ${LayoutUtilsLibraryName}
  ${LibTalipotCoreName}
  ${VPSCLibrary}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
