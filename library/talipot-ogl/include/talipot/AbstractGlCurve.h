/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_ABSTRACT_GL_CURVE_H
#define TALIPOT_ABSTRACT_GL_CURVE_H

#include <talipot/OpenGlIncludes.h>

#include <memory>
#include <talipot/hash.h>

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/GlEntity.h>

namespace tlp {

class GlShaderProgram;
class GlShader;

class TLP_GL_SCOPE AbstractGlCurve : public GlEntity {

public:
  AbstractGlCurve(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode);

  AbstractGlCurve(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode,
                  const std::vector<Coord> &controlPoints, const Color &startColor,
                  const Color &endColor, const float startSize, const float endSize,
                  const uint nbCurvePoints);

  ~AbstractGlCurve() override;

  void draw(float lod, Camera *camera) override;

  void translate(const Coord &move) override;

  virtual void setTexture(const std::string &texture) {
    this->texture = texture;
  }

  virtual void setOutlined(const bool outlined) {
    this->outlined = outlined;
  }

  virtual void setOutlineColor(const Color &outlineColor) {
    this->outlineColor = outlineColor;
  }

  /**
   * If set to true, the curve quad outlines will have the same colors
   * than the curve quad
   */
  virtual void setOutlineColorInterpolation(const bool outlineColorInterpolation) {
    this->outlineColorInterpolation = outlineColorInterpolation;
  }

  /**
   * If set to true, the curve is drawn as a line and not as a thick quad
   */
  void setLineCurve(const bool lineCurve) {
    this->lineCurve = lineCurve;
  }

  void setCurveLineWidth(const float curveLineWidth) {
    this->curveLineWidth = curveLineWidth;
  }

  void setCurveQuadBordersWidth(const float curveQuadBorderWidth) {
    this->curveQuadBordersWidth = curveQuadBorderWidth;
  }

  virtual void setBillboardCurve(const bool billboardCurve) {
    this->billboardCurve = billboardCurve;
  }

  virtual void setLookDir(const Coord &lookDir) {
    this->lookDir = lookDir;
  }

  void getXML(std::string &) override;

  void setWithXML(const std::string &, uint &) override;

  virtual void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor,
                         const Color &endColor, const float startSize, const float endSize,
                         const uint nbCurvePoints = 100);

protected:
  virtual void setCurveVertexShaderRenderingSpecificParameters() {}

  virtual void cleanupAfterCurveVertexShaderRendering() {}

  virtual Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) = 0;

  virtual void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints,
                                       std::vector<Coord> &curvePoints, uint nbCurvePoints) = 0;

  static void buildCurveVertexBuffers(const uint nbCurvePoints, bool vboOk);

  void initShader(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode);

  static flat_hash_map<uint, std::vector<GLfloat>> curveVertexBuffersData;
  static flat_hash_map<uint, std::vector<std::vector<GLushort>>> curveVertexBuffersIndices;
  static flat_hash_map<uint, std::vector<GLuint>> curveVertexBuffersObject;
  static flat_hash_map<std::string, std::unique_ptr<GlShaderProgram>> curvesShadersMap;
  static flat_hash_map<std::string, std::unique_ptr<GlShaderProgram>> curvesBillboardShadersMap;
  static bool canUseGeometryShader;
  static flat_hash_map<
      std::string, std::pair<std::unique_ptr<GlShaderProgram>, std::unique_ptr<GlShaderProgram>>>
      curvesGeometryShadersMap;
  static flat_hash_map<
      std::string, std::pair<std::unique_ptr<GlShaderProgram>, std::unique_ptr<GlShaderProgram>>>
      curvesBillboardGeometryShadersMap;

  std::string shaderProgramName;
  GlShaderProgram *curveShaderProgramNormal;
  GlShaderProgram *curveShaderProgramBillboard;
  GlShaderProgram *curveShaderProgram;

  std::vector<Coord> controlPoints;
  Color startColor;
  Color endColor;
  float startSize;
  float endSize;
  uint nbCurvePoints;
  bool outlined;
  Color outlineColor;
  std::string texture;
  float texCoordFactor;
  bool billboardCurve;
  Coord lookDir;
  bool lineCurve;
  float curveLineWidth;
  float curveQuadBordersWidth;
  bool outlineColorInterpolation;
};
}

#endif // TALIPOT_ABSTRACT_GL_CURVE_H
