SET(talipot-ogl_LIB_SRCS
    AbstractGlCurve.cpp
    Camera.cpp
    CubeOutLined.cpp
    Curves.cpp
    edgeExtremityArrow.cpp
    EdgeExtremityGlyph.cpp
    EdgeExtremityGlyphManager.cpp
    Font.cpp
    Gl2DRect.cpp
    GlAbstractPolygon.cpp
    GlAxis.cpp
    GlBezierCurve.cpp
    GlBoundingBoxSceneVisitor.cpp
    GlBox.cpp
    GlCatmullRomCurve.cpp
    GlCircle.cpp
    GlColorScale.cpp
    GlComplexPolygon.cpp
    GlComposite.cpp
    GlConvexGraphHull.cpp
    GlCPULODCalculator.cpp
    GlCubicBSplineInterpolation.cpp
    GlCurve.cpp
    GlEdge.cpp
    GlGlyphRenderer.cpp
    GlGraph.cpp
    GlGraphRenderer.cpp
    GlGraphHighDetailsRenderer.cpp
    GlGraphInputData.cpp
    GlGraphRenderingParameters.cpp
    GlGraphStaticData.cpp
    GlGrid.cpp
    GlHexagon.cpp
    GlLabel.cpp
    GlLayer.cpp
    GlLine.cpp
    GlLines.cpp
    GlMetaNodeRenderer.cpp
    GlNode.cpp
    GlNominativeAxis.cpp
    GlOpenUniformCubicBSpline.cpp
    GlPentagon.cpp
    GlPolygon.cpp
    GlPolyQuad.cpp
    GlProgressBar.cpp
    GlQuad.cpp
    GlQuadTreeLODCalculator.cpp
    GlQuantitativeAxis.cpp
    GlRect.cpp
    GlRegularPolygon.cpp
    GlScene.cpp
    GlSceneObserver.cpp
    GlSceneZoomAndPan.cpp
    GlShaderProgram.cpp
    GlEntity.cpp
    GlSphere.cpp
    GlStar.cpp
    GlTextureManager.cpp
    GlTools.cpp
    GlTriangle.cpp
    GlVertexArrayManager.cpp
    GlXMLTools.cpp
    Glyph.cpp
    GlyphManager.cpp
    OpenGlConfigManager.cpp)

INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude}
  ${TalipotCoreInclude}
  ${TalipotOGLInclude}
  ${PROJECT_SOURCE_DIR}
  ${FTGLInclude}
  ${FREETYPE_INCLUDE_DIRS}
  ${FONTCONFIG_INCLUDE_DIR}
  ${FRIBIDI_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${StbInclude}
                    ${Tess2Include})

ADD_LIBRARY(${LibTalipotOGLName} SHARED ${talipot-ogl_LIB_SRCS})

TARGET_LINK_LIBRARIES(
  ${LibTalipotOGLName}
  ${FTGLLibrary}
  ${GLEW_LIBRARY}
  ${LibTalipotCoreName}
  ${OPENGL_gl_LIBRARY}
  ${FREETYPE_LIBRARY}
  ${Tess2Library}
  ${FONTCONFIG_LIBRARIES}
  ${FRIBIDI_LIBRARY})

INSTALL(
  TARGETS ${LibTalipotOGLName}
  RUNTIME DESTINATION ${TalipotBinInstallDir}
  LIBRARY DESTINATION ${TalipotLibInstallDir}
  ARCHIVE DESTINATION ${TalipotLibInstallDir})
