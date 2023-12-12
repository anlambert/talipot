/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <GL/glew.h>
#include <talipot/AbstractGlCurve.h>
#include <talipot/GlTextureManager.h>
#include <talipot/Curves.h>
#include <talipot/GlShaderProgram.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;

namespace tlp {

static string commonUniformVariables = R"(#version 120

uniform sampler1D controlPoints;
uniform int nbControlPoints;
const int controlPointsTexSize = 1024;

vec3 getControlPoint(int index) {
  return texture1D(controlPoints, float(index) / float(nbControlPoints - 1)).xyz;
}

)";

static string curveVertexShaderNormalMainSrc = R"(#version 120

uniform int nbCurvePoints;
uniform float startSize;
uniform float endSize;
uniform vec4 startColor;
uniform vec4 endColor;
uniform float step;
uniform float texCoordFactor;

vec3 computeCurvePoint(float t);

void main () {
  float t = gl_Vertex.x;
  float size = mix(startSize, endSize, t);
  vec3 curvePoint = computeCurvePoint(t);
  if (gl_Vertex.y != 0.0) {
    vec3 tangent = vec3(0.0);
    if (t != 1.0) {
      vec3 nextCurvePoint = computeCurvePoint(t + step);
      tangent = normalize(nextCurvePoint - curvePoint);
    } else {
      vec3 prevCurvePoint = computeCurvePoint(t - step);
      tangent = normalize(curvePoint - prevCurvePoint);
    }
    vec3 normal = tangent;
    normal.x = -tangent.y;
    normal.y = tangent.x;
    curvePoint += normal * (gl_Vertex.y * size);
  }
  gl_Position = gl_ModelViewProjectionMatrix * vec4(curvePoint, 1.0);

  gl_FrontColor =  mix(startColor, endColor, t);
  if (gl_Vertex.y > 0.0) {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
  } else {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
  }
}

)";

static string curveVertexShaderGeometryNormalMainSrc = R"(#version 120

uniform float startSize;
uniform float endSize;
uniform vec4 startColor;
uniform vec4 endColor;

vec3 computeCurvePoint(float t);

void main () {
  float t = gl_Vertex.x;
  gl_BackColor.r = mix(startSize, endSize, t);
  gl_Position = vec4(computeCurvePoint(t), t);
  gl_FrontColor =  mix(startColor, endColor, t);
}

)";

static const string curveExtrusionGeometryShaderSrc = R"(#version 120
#extension GL_EXT_geometry_shader4 : enable

const float M_PI = 3.141592653589793238462643;

uniform bool topOutline;
uniform bool bottomOutline;

uniform int nbCurvePoints;
uniform float texCoordFactor;

uniform vec4 center;
uniform float radius;
uniform float height;

void computeExtrusionAndEmitVertices(vec3 pBefore, vec3 pCurrent, vec3 pAfter, float size, float t) {
  vec3 u = pBefore - pCurrent;
  vec3 v = pAfter - pCurrent;
  vec3 xu = normalize(u);
  vec3 xv = normalize(v);
  vec3 bi_xu_xv = normalize(xu+xv);
  float angle = M_PI - acos(dot(u, v) / (length(u)*length(v)));

  // Nan check
  if(angle != angle) {
    angle = 0.0;
  }

  float newSize = size;

  float cosA = cos(angle / 2.0);

  bool parallel = false;

  if (cosA > 1e-1) {
    newSize = size / cosA;
  }

  if (cosA < 1e-1 || angle < 1e-3) {
    vec3 tmp = vec3(0.0);
    tmp = normalize(pAfter - pCurrent);
    bi_xu_xv = tmp;
    bi_xu_xv.x = -tmp.y;
    bi_xu_xv.y = tmp.x;
    parallel = true;
    angle = 0.0;
  }

  gl_TexCoord[0] = vec4(1.0);

  if (parallel || cross(xu, xv)[2] < 0) {
    if (topOutline) {
      gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent + bi_xu_xv * newSize, 1.0);
      EmitVertex();
    }

    if (bottomOutline) {
      gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
      gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent - bi_xu_xv * newSize, 1.0);
      EmitVertex();
    }

  } else {
    if (topOutline) {
      gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent - bi_xu_xv * newSize, 1.0);
      EmitVertex();
    }

    if (bottomOutline) {
      gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
      gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent + bi_xu_xv * newSize, 1.0);
      EmitVertex();
    }
  }
}

void main() {

  if (gl_PositionIn[0].w == 0.0) {
    gl_FrontColor = gl_FrontColorIn[0];
    computeExtrusionAndEmitVertices(gl_PositionIn[0].xyz - (gl_PositionIn[1].xyz - gl_PositionIn[0].xyz),
                                    gl_PositionIn[0].xyz, gl_PositionIn[1].xyz, gl_BackColorIn[0].r,
                                    gl_PositionIn[0].w);
  }

  gl_FrontColor = gl_FrontColorIn[1];
  computeExtrusionAndEmitVertices(gl_PositionIn[0].xyz, gl_PositionIn[1].xyz, gl_PositionIn[2].xyz,
                                  gl_BackColorIn[1].r, gl_PositionIn[1].w);
  gl_FrontColor = gl_FrontColorIn[2];
  computeExtrusionAndEmitVertices(gl_PositionIn[1].xyz, gl_PositionIn[2].xyz, gl_PositionIn[3].xyz,
                                  gl_BackColorIn[2].r, gl_PositionIn[2].w);

  if (gl_PositionIn[3].w == 1.0) {
    gl_FrontColor = gl_FrontColorIn[3];
    computeExtrusionAndEmitVertices(gl_PositionIn[2].xyz, gl_PositionIn[3].xyz,
                                    gl_PositionIn[3].xyz + (gl_PositionIn[3].xyz - gl_PositionIn[2].xyz),
                                    gl_BackColorIn[3].r, gl_PositionIn[3].w);
  }
}

)";

static const string curveExtrusionBillboardGeometryShaderSrc = R"(#version 120
#extension GL_EXT_geometry_shader4 : enable

const float M_PI = 3.141592653589793238462643;

uniform bool topOutline;
uniform bool bottomOutline;

uniform int nbCurvePoints;
uniform float texCoordFactor;
uniform vec3 lookDir;

uniform vec4 center;
uniform float radius;
uniform float height;

void computeExtrusionAndEmitVertices(vec3 pBefore, vec3 pCurrent, vec3 pAfter, float size, float t) {
  vec3 dir = vec3(0.0);
  float angle = 0.0;
  if (t == 0.0) {
    vec3 xu = normalize(pCurrent - pAfter);
    dir = normalize(cross(xu, lookDir));
  } else if (t == 1.0) {
    vec3 xu = normalize(pBefore - pCurrent);
    dir = normalize(cross(xu, lookDir));
  } else {
    vec3 u = normalize(pBefore - pCurrent);
    vec3 v = normalize(pAfter - pCurrent);
    vec3 xu = cross(u, lookDir);
    vec3 xv = cross(v,-lookDir);
    dir = normalize(xu+xv);
     float angle = M_PI - acos(dot(u, v));
    // Check if angle == NaN (GLSL Spec NaN != NaN) because the isnan builtin function
    // is available since version 130 of GLSL
    if (angle != angle) {
      angle = 0;
    }
  }

  gl_TexCoord[0] = vec4(1.0);
  gl_TexCoord[1] = vec4(1.0);

  float newSize = size / cos(angle / 2.0);

  if (topOutline) {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
    gl_TexCoord[1].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent + dir * newSize, 1.0);
    EmitVertex();
  }

  if (bottomOutline) {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
    gl_TexCoord[1].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(pCurrent - dir * newSize, 1.0);
    EmitVertex();
  }
}

void main() {

  if (gl_PositionIn[0].w == 0.0) {
    gl_FrontColor = gl_FrontColorIn[0];
    computeExtrusionAndEmitVertices(gl_PositionIn[0].xyz - (gl_PositionIn[1].xyz - gl_PositionIn[0].xyz),
                                    gl_PositionIn[0].xyz, gl_PositionIn[1].xyz, gl_BackColorIn[0].r,
                                    gl_PositionIn[0].w);
  }

  gl_FrontColor = gl_FrontColorIn[1];
  computeExtrusionAndEmitVertices(gl_PositionIn[0].xyz, gl_PositionIn[1].xyz, gl_PositionIn[2].xyz,
                                  gl_BackColorIn[1].r, gl_PositionIn[1].w);
  gl_FrontColor = gl_FrontColorIn[2];
  computeExtrusionAndEmitVertices(gl_PositionIn[1].xyz, gl_PositionIn[2].xyz, gl_PositionIn[3].xyz,
                                  gl_BackColorIn[2].r, gl_PositionIn[2].w);

  if (gl_PositionIn[3].w == 1.0) {
    gl_FrontColor = gl_FrontColorIn[3];
    computeExtrusionAndEmitVertices(gl_PositionIn[2].xyz, gl_PositionIn[3].xyz,
                                    gl_PositionIn[3].xyz + (gl_PositionIn[3].xyz - gl_PositionIn[2].xyz),
                                    gl_BackColorIn[3].r, gl_PositionIn[3].w);
  }
}

)";

static string curveVertexShaderBillboardMainSrc = R"(#version 120

uniform int nbCurvePoints;
uniform float startSize;
uniform float endSize;
uniform vec4 startColor;
uniform vec4 endColor;
uniform float step;
uniform float texCoordFactor;
uniform vec3 lookDir;

const float PI = 3.141592653589793238462643;

vec3 computeCurvePoint(float t);

void main () {
  float t = gl_Vertex.x;
  float size = mix(startSize, endSize, t);
  vec3 curvePoint = computeCurvePoint(t);
  vec3 nextCurvePoint = computeCurvePoint(clamp(t + step, 0.0, 1.0));
  vec3 prevCurvePoint = computeCurvePoint(clamp(t - step, 0.0, 1.0));
  vec3 dir = vec3(0.0);
  if (t == 0.0) {
    vec3 xu = normalize(curvePoint - nextCurvePoint);
    dir = normalize(cross(xu, lookDir));
  } else if (t == 1.0) {
    vec3 xu = normalize(prevCurvePoint - curvePoint);
    dir = normalize(cross(xu, lookDir));
  } else {
    vec3 u = normalize(prevCurvePoint - curvePoint);
    vec3 v = normalize(nextCurvePoint - curvePoint);
    vec3 xu = cross(u, lookDir);
    vec3 xv = cross(v,-lookDir);
    dir = normalize(xu + xv);
     float angle = PI - acos(dot(u, v));
    // Check if angle == NaN (GLSL Spec NaN != NaN) because the isnan builtin function
    // is available since version 130 of GLSL
    if (angle != angle) {
      angle = 0;
    }
    size = size/cos(angle / 2.0);
  }
  curvePoint += dir * (gl_Vertex.y * size);
  gl_Position = gl_ModelViewProjectionMatrix * vec4(curvePoint, 1.0);
  gl_FrontColor =  mix(startColor, endColor, t);
  if (gl_Vertex.y > 0.0) {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
    gl_TexCoord[1].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 0.0);
  } else {
    gl_TexCoord[0].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
    gl_TexCoord[1].st = vec2(t * float(nbCurvePoints - 1) * texCoordFactor, 1.0);
  }
}

)";

static string curveFragmentShaderSrc = R"(#version 120

uniform sampler2D texture;
uniform sampler2D texture3d;

uniform bool useTexture;
uniform bool billboard;

void main() {
  gl_FragColor = gl_Color;
  if (useTexture) {
    gl_FragColor *= texture2D(texture, gl_TexCoord[0].st);
  }
  if (billboard) {
    gl_FragColor *= texture2D(texture3d, gl_TexCoord[0].st);
  }
}

)";

unordered_map<uint, vector<GLfloat>> AbstractGlCurve::curveVertexBuffersData;
unordered_map<uint, vector<vector<GLushort>>> AbstractGlCurve::curveVertexBuffersIndices;
unordered_map<uint, vector<GLuint>> AbstractGlCurve::curveVertexBuffersObject;
unordered_map<string, unique_ptr<GlShaderProgram>> AbstractGlCurve::curvesShadersMap;
unordered_map<string, unique_ptr<GlShaderProgram>> AbstractGlCurve::curvesBillboardShadersMap;
bool AbstractGlCurve::canUseGeometryShader = false;
std::unordered_map<std::string, std::pair<unique_ptr<GlShaderProgram>, unique_ptr<GlShaderProgram>>>
    AbstractGlCurve::curvesGeometryShadersMap;
std::unordered_map<std::string, std::pair<unique_ptr<GlShaderProgram>, unique_ptr<GlShaderProgram>>>
    AbstractGlCurve::curvesBillboardGeometryShadersMap;

AbstractGlCurve::AbstractGlCurve(const string &shaderProgramName,
                                 const string &curveSpecificShaderCode)
    : curveShaderProgramNormal(nullptr), curveShaderProgramBillboard(nullptr),
      curveShaderProgram(nullptr), outlined(false), outlineColor(Color(0, 0, 0)), texCoordFactor(1),
      billboardCurve(false), lookDir(Coord(0, 0, 1)), lineCurve(false), curveLineWidth(1.0f),
      curveQuadBordersWidth(1.0f), outlineColorInterpolation(false) {
  canUseGeometryShader = GlShaderProgram::geometryShaderSupported();
  initShader(shaderProgramName, curveSpecificShaderCode);
}

AbstractGlCurve::AbstractGlCurve(const string &shaderProgramName,
                                 const string &curveSpecificShaderCode,
                                 const vector<Coord> &controlPoints, const Color &startColor,
                                 const Color &endColor, const float startSize, const float endSize,
                                 const uint nbCurvePoints)
    : shaderProgramName(shaderProgramName), curveShaderProgramNormal(nullptr),
      curveShaderProgramBillboard(nullptr), curveShaderProgram(nullptr),
      controlPoints(controlPoints), startColor(startColor), endColor(endColor),
      startSize(startSize), endSize(endSize), nbCurvePoints(nbCurvePoints), outlined(false),
      outlineColor(Color(0, 0, 0)), texCoordFactor(1), billboardCurve(false),
      lookDir(Coord(0, 0, 1)), lineCurve(false), curveLineWidth(1.0f), curveQuadBordersWidth(1.0f),
      outlineColorInterpolation(false) {

  canUseGeometryShader = GlShaderProgram::geometryShaderSupported();
  initShader(shaderProgramName, curveSpecificShaderCode);

  for (const auto &controlPoint : controlPoints) {
    boundingBox.expand(controlPoint);
  }
}

AbstractGlCurve::~AbstractGlCurve() = default;

void AbstractGlCurve::buildCurveVertexBuffers(const uint nbCurvePoints, bool vboOk) {
  curveVertexBuffersObject[nbCurvePoints].resize(5);
  curveVertexBuffersData[nbCurvePoints].resize(nbCurvePoints * 6);
  curveVertexBuffersIndices[nbCurvePoints].resize(4);
  curveVertexBuffersIndices[nbCurvePoints][0].resize(nbCurvePoints * 2);
  curveVertexBuffersIndices[nbCurvePoints][1].resize(nbCurvePoints);
  curveVertexBuffersIndices[nbCurvePoints][2].resize(nbCurvePoints);
  curveVertexBuffersIndices[nbCurvePoints][3].resize(nbCurvePoints);

  for (uint i = 0; i < nbCurvePoints; ++i) {
    float t = i / float(nbCurvePoints - 1);
    curveVertexBuffersData[nbCurvePoints][6 * i] = t;
    curveVertexBuffersData[nbCurvePoints][6 * i + 1] = 1.0f;
    curveVertexBuffersData[nbCurvePoints][6 * i + 2] = t;
    curveVertexBuffersData[nbCurvePoints][6 * i + 3] = 0.0f;
    curveVertexBuffersData[nbCurvePoints][6 * i + 4] = t;
    curveVertexBuffersData[nbCurvePoints][6 * i + 5] = -1.0f;

    curveVertexBuffersIndices[nbCurvePoints][0][2 * i] = 3 * i;
    curveVertexBuffersIndices[nbCurvePoints][0][2 * i + 1] = 3 * i + 2;
    curveVertexBuffersIndices[nbCurvePoints][1][i] = 3 * i + 1;
    curveVertexBuffersIndices[nbCurvePoints][2][i] = 3 * i;
    curveVertexBuffersIndices[nbCurvePoints][3][i] = 3 * i + 2;
  }

  if (vboOk) {
    glGenBuffers(5, curveVertexBuffersObject[nbCurvePoints].data());
    glBindBuffer(GL_ARRAY_BUFFER, curveVertexBuffersObject[nbCurvePoints][0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * nbCurvePoints * sizeof(GLfloat),
                 curveVertexBuffersData[nbCurvePoints].data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curveVertexBuffersObject[nbCurvePoints][1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * nbCurvePoints * sizeof(GLushort),
                 curveVertexBuffersIndices[nbCurvePoints][0].data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curveVertexBuffersObject[nbCurvePoints][2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbCurvePoints * sizeof(GLushort),
                 curveVertexBuffersIndices[nbCurvePoints][1].data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curveVertexBuffersObject[nbCurvePoints][3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbCurvePoints * sizeof(GLushort),
                 curveVertexBuffersIndices[nbCurvePoints][2].data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curveVertexBuffersObject[nbCurvePoints][4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbCurvePoints * sizeof(GLushort),
                 curveVertexBuffersIndices[nbCurvePoints][3].data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void AbstractGlCurve::draw(float, Camera *) {
  drawCurve(controlPoints, startColor, endColor, startSize, endSize, nbCurvePoints);
}

void AbstractGlCurve::initShader(const std::string &shaderProgramName,
                                 const std::string &curveSpecificShaderCode) {
  // restrict shaders compilation on compatible hardware, crashes can happen
  // when using a software implementation of OpenGL
  static string glVendor = OpenGlConfigManager::getOpenGLVendor();
  static bool glVendorOk = (glVendor.find("NVIDIA") != string::npos) ||
                           (glVendor.find("ATI") != string::npos) ||
                           (glVendor.find("Intel") != string::npos);

  static unique_ptr<GlShader> curveVertexShaderNormalMain;
  static unique_ptr<GlShader> curveVertexShaderBillboardMain;
  static unique_ptr<GlShader> curveFragmentShader;
  static unique_ptr<GlShader> curveVertexGeometryShaderNormalMain;

  if (glVendorOk && GlShaderProgram::shaderProgramsSupported()) {

    if (curveVertexShaderNormalMain.get() == nullptr) {
      curveVertexShaderNormalMain.reset(new GlShader(Vertex));
      curveVertexShaderNormalMain->compileFromSourceCode(curveVertexShaderNormalMainSrc);
    }

    if (curveVertexShaderBillboardMain.get() == nullptr) {
      curveVertexShaderBillboardMain.reset(new GlShader(Vertex));
      curveVertexShaderBillboardMain->compileFromSourceCode(curveVertexShaderBillboardMainSrc);
    }

    if (curveVertexGeometryShaderNormalMain.get() == nullptr) {
      curveVertexGeometryShaderNormalMain.reset(new GlShader(Vertex));
      curveVertexGeometryShaderNormalMain->compileFromSourceCode(
          curveVertexShaderGeometryNormalMainSrc);
    }

    if (curveFragmentShader.get() == nullptr) {
      curveFragmentShader.reset(new GlShader(Fragment));
      curveFragmentShader->compileFromSourceCode(curveFragmentShaderSrc);
    }

    if (!curvesShadersMap.contains(shaderProgramName)) {
      curvesShadersMap[shaderProgramName].reset(new GlShaderProgram(shaderProgramName));
      curvesShadersMap[shaderProgramName]->addShaderFromSourceCode(
          Vertex, commonUniformVariables + curveSpecificShaderCode);
      curvesShadersMap[shaderProgramName]->addShader(curveVertexShaderNormalMain.get());
      curvesShadersMap[shaderProgramName]->addShader(curveFragmentShader.get());
      curvesShadersMap[shaderProgramName]->link();
      curvesShadersMap[shaderProgramName]->printInfoLog();
    }

    if (canUseGeometryShader && !curvesGeometryShadersMap.contains(shaderProgramName)) {
      auto *polygonShader = new GlShaderProgram(shaderProgramName);
      polygonShader->addShaderFromSourceCode(Vertex,
                                             commonUniformVariables + curveSpecificShaderCode);
      polygonShader->addShader(curveVertexGeometryShaderNormalMain.get());
      polygonShader->addGeometryShaderFromSourceCode(curveExtrusionGeometryShaderSrc,
                                                     GL_LINES_ADJACENCY_EXT, GL_TRIANGLE_STRIP);
      polygonShader->setMaxGeometryShaderOutputVertices(6);
      polygonShader->addShader(curveFragmentShader.get());
      polygonShader->link();
      polygonShader->printInfoLog();

      if (!polygonShader->isLinked()) {
        delete polygonShader;
        polygonShader = nullptr;
      }

      auto *lineShader = new GlShaderProgram(shaderProgramName);
      lineShader->addShaderFromSourceCode(Vertex, commonUniformVariables + curveSpecificShaderCode);
      lineShader->addShader(curveVertexGeometryShaderNormalMain.get());
      lineShader->addGeometryShaderFromSourceCode(curveExtrusionGeometryShaderSrc,
                                                  GL_LINES_ADJACENCY_EXT, GL_LINE_STRIP);
      lineShader->setMaxGeometryShaderOutputVertices(6);
      lineShader->addShader(curveFragmentShader.get());
      lineShader->link();
      lineShader->printInfoLog();

      if (!lineShader->isLinked()) {
        delete lineShader;
        lineShader = nullptr;
      }

      curvesGeometryShadersMap[shaderProgramName].first.reset(polygonShader);
      curvesGeometryShadersMap[shaderProgramName].second.reset(lineShader);
    }

    if (!curvesBillboardShadersMap.contains(shaderProgramName)) {
      curvesBillboardShadersMap[shaderProgramName].reset(new GlShaderProgram(shaderProgramName));
      curvesBillboardShadersMap[shaderProgramName]->addShaderFromSourceCode(
          Vertex, commonUniformVariables + curveSpecificShaderCode);

      curvesBillboardShadersMap[shaderProgramName]->addShader(curveVertexShaderBillboardMain.get());
      curvesBillboardShadersMap[shaderProgramName]->addShader(curveFragmentShader.get());
      curvesBillboardShadersMap[shaderProgramName]->link();
      curvesBillboardShadersMap[shaderProgramName]->printInfoLog();
    }

    if (canUseGeometryShader && curvesBillboardGeometryShadersMap.find(shaderProgramName) ==
                                    curvesBillboardGeometryShadersMap.end()) {
      auto *polygonShader = new GlShaderProgram(shaderProgramName);
      polygonShader->addShaderFromSourceCode(Vertex,
                                             commonUniformVariables + curveSpecificShaderCode);
      polygonShader->addShader(curveVertexGeometryShaderNormalMain.get());
      polygonShader->addGeometryShaderFromSourceCode(curveExtrusionBillboardGeometryShaderSrc,
                                                     GL_LINES_ADJACENCY_EXT, GL_TRIANGLE_STRIP);
      polygonShader->setMaxGeometryShaderOutputVertices(6);
      polygonShader->addShader(curveFragmentShader.get());
      polygonShader->link();
      polygonShader->printInfoLog();

      if (!polygonShader->isLinked()) {
        delete polygonShader;
        polygonShader = nullptr;
      }

      auto *lineShader = new GlShaderProgram(shaderProgramName);
      lineShader->addShaderFromSourceCode(Vertex, commonUniformVariables + curveSpecificShaderCode);
      lineShader->addShader(curveVertexGeometryShaderNormalMain.get());
      lineShader->addGeometryShaderFromSourceCode(curveExtrusionBillboardGeometryShaderSrc,
                                                  GL_LINES_ADJACENCY_EXT, GL_LINE_STRIP);
      lineShader->setMaxGeometryShaderOutputVertices(6);
      lineShader->addShader(curveFragmentShader.get());
      lineShader->link();
      lineShader->printInfoLog();

      if (!lineShader->isLinked()) {
        delete lineShader;
        lineShader = nullptr;
      }

      curvesBillboardGeometryShadersMap[shaderProgramName].first.reset(polygonShader);
      curvesBillboardGeometryShadersMap[shaderProgramName].second.reset(lineShader);
    }

    if (curvesShadersMap[shaderProgramName]->isLinked()) {
      curveShaderProgramNormal = curvesShadersMap[shaderProgramName].get();
    }

    if (curvesBillboardShadersMap[shaderProgramName]->isLinked()) {
      curveShaderProgramBillboard = curvesBillboardShadersMap[shaderProgramName].get();
    }
  }
}

void AbstractGlCurve::drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                                const Color &endColor, const float startSize, const float endSize,
                                const uint nbCurvePoints) {

  GLint renderMode;
  glGetIntegerv(GL_RENDER_MODE, &renderMode);

  float currentLineWidth;
  glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);

  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);

  if (!texture.empty()) {
    uint i = nbCurvePoints / 2;
    Coord firstCurvePoint = computeCurvePointOnCPU(controlPoints, i / float(nbCurvePoints - 1));
    Coord nexCurvePoint = computeCurvePointOnCPU(controlPoints, (i + 1) / float(nbCurvePoints - 1));
    float dist = firstCurvePoint.dist(nexCurvePoint);
    texCoordFactor = dist / (startSize * 2.0f);
  }

  if (billboardCurve) {
    curveShaderProgram = curveShaderProgramBillboard;
  } else {
    curveShaderProgram = curveShaderProgramNormal;
  }

  static bool canUseFloatTextures =
      OpenGlConfigManager::isExtensionSupported("GL_ARB_texture_float");

  if (curveShaderProgram != nullptr && canUseFloatTextures && renderMode != GL_SELECT) {

    static bool vboOk = OpenGlConfigManager::hasVertexBufferObject();

    pair<GlShaderProgram *, GlShaderProgram *> geometryShaders = {nullptr, nullptr};
    pair<GlShaderProgram *, GlShaderProgram *> geometryBillboardShaders = {nullptr, nullptr};

    if (canUseGeometryShader && curvesGeometryShadersMap.find(curveShaderProgram->getName()) !=
                                    curvesGeometryShadersMap.end()) {
      auto &p = curvesGeometryShadersMap[curveShaderProgram->getName()];
      geometryShaders = {p.first.get(), p.second.get()};
      auto &p2 = curvesBillboardGeometryShadersMap[curveShaderProgram->getName()];
      geometryBillboardShaders = {p2.first.get(), p2.second.get()};
    }

    if (!curveVertexBuffersData.contains(nbCurvePoints)) {
      buildCurveVertexBuffers(nbCurvePoints, vboOk);
    }

    GLuint *vbo = curveVertexBuffersObject[nbCurvePoints].data();

    static GLuint controlPointsTexId = 0;

    if (controlPointsTexId == 0) {
      glGenTextures(1, &controlPointsTexId);
    }

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, controlPointsTexId);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F_ARB, controlPoints.size(), 0, GL_RGB, GL_FLOAT,
                 &controlPoints[0][0]);
    glBindTexture(GL_TEXTURE_1D, 0);
    glDisable(GL_TEXTURE_1D);

    bool geometryShaderActivated = false;

    if (canUseGeometryShader && nbCurvePoints > 3 && !lineCurve && geometryShaders.first) {
      if (!billboardCurve) {
        curveShaderProgram = geometryShaders.first;
      } else {
        curveShaderProgram = geometryBillboardShaders.first;
      }

      geometryShaderActivated = true;
    }

    curveShaderProgram->activate();
    curveShaderProgram->setUniformTextureSampler("controlPoints", 3);
    curveShaderProgram->setUniformInt("nbControlPoints", controlPoints.size());
    curveShaderProgram->setUniformInt("nbCurvePoints", nbCurvePoints);
    curveShaderProgram->setUniformFloat("startSize", startSize);
    curveShaderProgram->setUniformFloat("endSize", endSize);
    curveShaderProgram->setUniformColor("startColor", startColor);
    curveShaderProgram->setUniformColor("endColor", endColor);
    curveShaderProgram->setUniformTextureSampler("texture", 0);
    curveShaderProgram->setUniformTextureSampler("texture3d", 1);
    curveShaderProgram->setUniformBool("useTexture", !texture.empty() && !lineCurve);
    curveShaderProgram->setUniformBool("billboard", billboardCurve && !lineCurve);

    if (!geometryShaderActivated) {
      curveShaderProgram->setUniformFloat("step", 1.0f / (float(nbCurvePoints) - 1.0f));
    } else {
      curveShaderProgram->setUniformBool("topOutline", true);
      curveShaderProgram->setUniformBool("bottomOutline", true);
    }

    curveShaderProgram->setUniformFloat("texCoordFactor", texCoordFactor);

    if (billboardCurve) {
      curveShaderProgram->setUniformVec3Float("lookDir", lookDir);
    }

    setCurveVertexShaderRenderingSpecificParameters();

    glEnableClientState(GL_VERTEX_ARRAY);

    if (vboOk) {
      glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
      glVertexPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), nullptr);
    } else {
      glVertexPointer(2, GL_FLOAT, 2 * sizeof(GLfloat),
                      curveVertexBuffersData[nbCurvePoints].data());
    }

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, controlPointsTexId);

    if (lineCurve) {
      glLineWidth(curveLineWidth);

      if (vboOk) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
        glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
      } else {
        glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT,
                       curveVertexBuffersIndices[nbCurvePoints][1].data());
      }

    } else {
      if (!texture.empty()) {
        glActiveTexture(GL_TEXTURE0);
        GlTextureManager::activateTexture(texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      }

      if (billboardCurve) {
        glActiveTexture(GL_TEXTURE1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GlTextureManager::activateTexture(TalipotBitmapDir + "cylinderTexture.png");
      }

      if (vboOk) {
        if (geometryShaderActivated) {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
          glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
        } else {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
          glDrawElements(GL_TRIANGLE_STRIP, nbCurvePoints * 2, GL_UNSIGNED_SHORT, nullptr);
        }
      } else {
        if (geometryShaderActivated) {
          glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT,
                         curveVertexBuffersIndices[nbCurvePoints][1].data());
        } else {
          glDrawElements(GL_TRIANGLE_STRIP, nbCurvePoints * 2, GL_UNSIGNED_SHORT,
                         curveVertexBuffersIndices[nbCurvePoints][0].data());
        }
      }

      if (billboardCurve) {
        glActiveTexture(GL_TEXTURE1);
        GlTextureManager::deactivateTexture();
      }

      if (!texture.empty()) {
        glActiveTexture(GL_TEXTURE0);
        GlTextureManager::deactivateTexture();
      }

      if (outlined) {

        if (canUseGeometryShader && nbCurvePoints > 3 && geometryShaders.second) {
          curveShaderProgram->deactivate();

          if (billboardCurve) {
            curveShaderProgram = geometryBillboardShaders.second;
          } else {
            curveShaderProgram = geometryShaders.second;
          }

          curveShaderProgram->activate();
          curveShaderProgram->setUniformTextureSampler("controlPoints", 3);
          curveShaderProgram->setUniformInt("nbControlPoints", controlPoints.size());
          curveShaderProgram->setUniformInt("nbCurvePoints", nbCurvePoints);
          curveShaderProgram->setUniformFloat("startSize", startSize);
          curveShaderProgram->setUniformFloat("endSize", endSize);
          curveShaderProgram->setUniformBool("topOutline", true);
          curveShaderProgram->setUniformBool("bottomOutline", false);
          curveShaderProgram->setUniformFloat("texCoordFactor", texCoordFactor);

          if (billboardCurve) {
            curveShaderProgram->setUniformVec3Float("lookDir", lookDir);
          }

          setCurveVertexShaderRenderingSpecificParameters();
        }

        if (outlineColorInterpolation) {
          curveShaderProgram->setUniformColor("startColor", startColor);
          curveShaderProgram->setUniformColor("endColor", endColor);
        } else {
          curveShaderProgram->setUniformColor("startColor", outlineColor);
          curveShaderProgram->setUniformColor("endColor", outlineColor);
        }

        glLineWidth(curveQuadBordersWidth);

        if (vboOk) {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);

          if (geometryShaderActivated) {
            glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
          } else {
            glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
          }
        } else {
          if (geometryShaderActivated) {
            glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT,
                           curveVertexBuffersIndices[nbCurvePoints][2].data());
          } else {
            glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT,
                           curveVertexBuffersIndices[nbCurvePoints][2].data());
          }
        }

        if (canUseGeometryShader && nbCurvePoints > 3 && geometryShaders.second) {
          curveShaderProgram->setUniformBool("topOutline", false);
          curveShaderProgram->setUniformBool("bottomOutline", true);
        }

        if (vboOk) {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);

          if (geometryShaderActivated) {
            glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
          } else {
            glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT, nullptr);
          }
        } else {
          if (geometryShaderActivated) {
            glDrawElements(GL_LINE_STRIP_ADJACENCY_EXT, nbCurvePoints, GL_UNSIGNED_SHORT,
                           curveVertexBuffersIndices[nbCurvePoints][3].data());
          } else {
            glDrawElements(GL_LINE_STRIP, nbCurvePoints, GL_UNSIGNED_SHORT,
                           curveVertexBuffersIndices[nbCurvePoints][3].data());
          }
        }
      }
    }

    if (vboOk) {
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glDisableClientState(GL_VERTEX_ARRAY);

    curveShaderProgram->deactivate();

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_1D, 0);

    glActiveTexture(GL_TEXTURE0);

    cleanupAfterCurveVertexShaderRendering();

  } else {
    vector<Coord> curvePoints;
    computeCurvePointsOnCPU(controlPoints, curvePoints, nbCurvePoints);

    if (lineCurve) {
      glLineWidth(curveLineWidth);
      polyLine(curvePoints, startColor, endColor);
    } else if (!billboardCurve) {
      polyQuad(
          curvePoints, startColor, endColor, startSize, endSize,
          Coord(2.f * curvePoints[0] - curvePoints[1]),
          Coord(2.f * curvePoints[curvePoints.size() - 1] - curvePoints[curvePoints.size() - 2]),
          outlineColorInterpolation, outlineColor, texture, curveQuadBordersWidth);
    } else {
      simpleQuad(
          curvePoints, startColor, endColor, startSize, endSize,
          Coord(2.f * curvePoints[0] - curvePoints[1]),
          Coord(2.f * curvePoints[curvePoints.size() - 1] - curvePoints[curvePoints.size() - 2]),
          lookDir, !outlined, outlineColor, texture);
    }
  }

  glLineWidth(currentLineWidth);

  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
}

void AbstractGlCurve::translate(const Coord &move) {
  for (auto &controlPoint : controlPoints) {
    controlPoint += move;
  }

  boundingBox.translate(move);
}

void AbstractGlCurve::getXML(string &) {}

void AbstractGlCurve::setWithXML(const string &, uint &) {}
}
