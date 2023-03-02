QTX_SET_INCLUDES_AND_DEFINITIONS()

ADD_SUBDIRECTORY(Fisheye)
ADD_SUBDIRECTORY(GraphSplatting)
ADD_SUBDIRECTORY(MouseMagnifyingGlass)
ADD_SUBDIRECTORY(MouseLassoNodesSelector)
ADD_SUBDIRECTORY(NeighborhoodHighlighter)
ADD_SUBDIRECTORY(PathFinder)

INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude} ${TalipotCoreInclude} ${TalipotOGLInclude}
  ${TalipotGUIInclude} ${OPENGL_INCLUDE_DIR})

# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorNavigation
  SRCS
  InteractorNavigation.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorGetInformation
  SRCS
  InteractorGetInformation.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorSelection
  SRCS
  InteractorSelection.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorSelectionModifier
  SRCS
  InteractorSelectionModifier.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorRectangleZoom
  SRCS
  InteractorRectangleZoom.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorAddNodeEdge
  SRCS
  InteractorAddEdge.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorEditEdgeBends
  SRCS
  InteractorEditEdgeBends.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
# ------------------------------------------------------------------------------
TALIPOT_ADD_PLUGIN(
  NAME
  InteractorDeleteElement
  SRCS
  InteractorDeleteElement.cpp
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
