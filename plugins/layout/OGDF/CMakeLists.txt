INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${TalipotCoreBuildInclude}
                    ${TalipotCoreInclude} ${OGDFInclude} ${TalipotOGDFInclude})

SET(PLUGINS_SRCS
    OGDFFm3.cpp
    OGDFUpwardPlanarization.cpp
    OGDFFrutchermanReingold.cpp
    OGDFFastMultipoleMultilevelEmbedder.cpp
    OGDFFastMultipoleEmbedder.cpp
    OGDFVisibility.cpp
    OGDFKamadaKawai.cpp
    OGDFCircular.cpp
    OGDFBalloon.cpp
    OGDFTree.cpp
    OGDFGemFrick.cpp
    OGDFStressMinimization.cpp
    OGDFSugiyama.cpp
    OGDFDominance.cpp
    OGDFPlanarizationGrid.cpp
    OGDFBertaultLayout.cpp
    OGDFPivotMDS.cpp
    OGDFTileToRowsPacking.cpp
    OGDFPlanarizationLayout.cpp
    OGDFDTreeMultilevelEmbedder.cpp)

DISABLE_COMPILER_WARNINGS()

FOREACH(OGDF_PLUGIN_SRC ${PLUGINS_SRCS})
  STRING(REGEX REPLACE ".cpp" "" OGDF_PLUGIN_NAME ${OGDF_PLUGIN_SRC})
  STRING(REGEX REPLACE ".*/" "" OGDF_PLUGIN_NAME ${OGDF_PLUGIN_NAME})

  TALIPOT_ADD_PLUGIN(
    NAME
    ${OGDF_PLUGIN_NAME}
    SRCS
    ${OGDF_PLUGIN_SRC}
    LINKS
    ${LibTalipotCoreName}
    ${LibTalipotOGDFName}
    ${OGDF_LIBRARY}
    INSTALL_DIR
    ${TalipotPluginsInstallDir})
ENDFOREACH()
