INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${OGDFInclude} ${TalipotOGDFInclude})

SET(PLUGINS_SRCS
    OGDFPetersenGraph.cpp
    OGDFRandomGeographicalGraph.cpp
    OGDFRandomHierarchy.cpp
    OGDFGlobeGraph.cpp
    OGDFRegularLatticeGraph.cpp
    OGDFRegularTree.cpp
    OGDFCompleteKPartiteGraph.cpp
    OGDFCirculantGraph.cpp
    OGDFRandomRegularGraph.cpp
    OGDFRandomGraph.cpp
    OGDFRandomSimpleGraph.cpp
    OGDFRandomSimpleGraphByProbability.cpp
    OGDFRandomChungLuGraph.cpp
    OGDFRandomPlanarConnectedGraph.cpp
    OGDFRandomPlanarBiconnectedGraph.cpp)

DISABLE_COMPILER_WARNINGS()

TALIPOT_ADD_PLUGIN(
  NAME
  OGDFGraphGenerators
  SRCS
  ${PLUGINS_SRCS}
  LINKS
  ${LibTalipotCoreName}
  ${LibTalipotOGDFName}
  ${OGDF_LIBRARY}
  INSTALL_DIR
  ${TalipotPluginsInstallDir})
