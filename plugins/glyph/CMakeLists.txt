INCLUDE_DIRECTORIES(
  ${TalipotCoreBuildInclude} ${TalipotCoreInclude} ${TalipotOGLInclude}
  ${GLEW_INCLUDE_DIR} ${FTGLInclude} ${FREETYPE_INCLUDE_DIRS})

MACRO(GLYPH NAME SRCS)
  TALIPOT_ADD_PLUGIN(
    NAME
    ${NAME}
    SRCS
    ${SRCS}
    LINKS
    ${LibTalipotCoreName}
    ${LibTalipotOGLName}
    ${FTGLLibrary}
    INSTALL_DIR
    ${TalipotGlyphPluginsInstallDir})
ENDMACRO(GLYPH)

GLYPH(Cone cone.cpp)
GLYPH(Circle circle.cpp)
GLYPH(Sphere sphere.cpp)
GLYPH(Square square.cpp)
GLYPH(RoundedBox roundedbox.cpp)
GLYPH(Cube cube.cpp)
GLYPH(Window window.cpp)
GLYPH(Hexagone hexagone.cpp)
GLYPH(Triangle triangle.cpp)
GLYPH(Pentagone pentagone.cpp)
GLYPH(CubeOutlinedTransparent cubeoutlinedtransparent.cpp)
GLYPH(Ring ring.cpp)
GLYPH(Billboard billboard.cpp)
GLYPH(Cylinder cylinder.cpp)
GLYPH(Diamond diamond.cpp)
GLYPH(Cross cross.cpp)
GLYPH(Star star.cpp)
GLYPH(FontIcon fonticon.cpp)
