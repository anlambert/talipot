INCLUDE_DIRECTORIES(BEFORE ${TalipotCoreBuildInclude})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIR}
  ${TalipotCoreInclude} ${TalipotOGLInclude} ${TalipotGUIInclude})

SET(LIB_SRCS
    PathFinder.cpp
    PathFinderComponent.cpp
    PathFinderConfigurationWidget.cpp
    PathFinderTools.cpp
    PathAlgorithm.cpp
    highlighters/EnclosingCircleHighlighter.cpp
    highlighters/PathHighlighter.cpp
    highlighters/ZoomAndPanHighlighter.cpp)

QT_WRAP_UI(LIB_UI_HEADERS designer/EnclosingCircleConfiguration.ui
           designer/PathFinderConfiguration.ui)

QT_WRAP_CPP(LIB_MOC_SRCS PathFinderConfigurationWidget.h PathFinder.h
            highlighters/EnclosingCircleHighlighter.h)

TALIPOT_ADD_PLUGIN(
  NAME
  PathFinder
  SRCS
  ${LIB_SRCS}
  ${LIB_UI_HEADERS}
  ${LIB_MOC_SRCS}
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGLName}
  ${LibTalipotGUIName}
  INSTALL_DIR
  ${TalipotInteractorPluginsInstallDir})
