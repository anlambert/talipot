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

#include <talipot/GlTextureManager.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>
#include <talipot/DrawingTools.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;
using namespace tlp;

namespace tlp {

static vector<Coord> coneVertices;
static vector<Coord> coneNormals;
static vector<Vec2f> coneTexCoords;
static vector<unsigned short> coneIndices;
static vector<uint> buffers;

static void drawCone() {
  if (coneVertices.empty()) {
    const uint numberOfSides = 30;
    coneVertices = computeRegularPolygon(numberOfSides, Coord(0, 0, -0.5), Size(0.5, 0.5));
    coneVertices.push_back(Coord(0, 0, -0.5));
    coneVertices.push_back(Coord(0, 0, 0.5));

    for (const auto &coneVertice : coneVertices) {
      coneTexCoords.push_back(Vec2f(coneVertice[0] + 0.5, coneVertice[1] + 0.5));
    }

    for (uint i = 0; i < numberOfSides - 1; ++i) {
      coneIndices.push_back(numberOfSides);
      coneIndices.push_back(i + 1);
      coneIndices.push_back(i);
    }

    coneIndices.push_back(numberOfSides);
    coneIndices.push_back(0);
    coneIndices.push_back(numberOfSides - 1);

    for (uint i = 0; i < numberOfSides - 1; ++i) {
      coneIndices.push_back(i);
      coneIndices.push_back(i + 1);
      coneIndices.push_back(numberOfSides + 1);
    }

    coneIndices.push_back(numberOfSides - 1);
    coneIndices.push_back(0);
    coneIndices.push_back(numberOfSides + 1);

    coneNormals = computeNormals(coneVertices, coneIndices);

    buffers.resize(4);
    glGenBuffers(4, &buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, coneVertices.size() * 3 * sizeof(float), &coneVertices[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, coneNormals.size() * 3 * sizeof(float), &coneNormals[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, coneTexCoords.size() * 2 * sizeof(float), &coneTexCoords[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, coneIndices.size() * sizeof(unsigned short),
                 &coneIndices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
  glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
  glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/// A 3D glyph.
/**
 * This glyph draws a textured cone using the "viewTexture" node
 * property value. If this property has no value, the cone is then colored
 * using the "viewColor" node property value.
 */
class Cone : public NoShaderGlyph {
public:
  GLYPHINFORMATION("3D - Cone", "Bertrand Mathieu", "09/07/2002", "Textured cone", "1.0",
                   NodeShape::Cone)
  Cone(const tlp::PluginContext *context = nullptr);
  ~Cone() override;
  BoundingBox getIncludeBoundingBox(node) override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};
PLUGIN(Cone)

Cone::Cone(const tlp::PluginContext *context) : NoShaderGlyph(context) {}
Cone::~Cone() = default;
BoundingBox Cone::getIncludeBoundingBox(node) {
  return {{-0.25, -0.25, 0}, {0.25, 0.25, 0.5}};
}
void Cone::draw(node n, float) {

  setMaterial(glGraphInputData->colors()->getNodeValue(n));
  string texFile = glGraphInputData->textures()->getNodeValue(n);

  if (!texFile.empty()) {
    string texturePath = glGraphInputData->renderingParameters()->getTexturePath();
    GlTextureManager::activateTexture(texturePath + texFile);
  }

  drawCone();

  GlTextureManager::deactivateTexture();
}
Coord Cone::getAnchor(const Coord &v) const {
  Coord anchor = v;

  float x = v.x(), y = v.y(), z = v.z();
  float n = sqrt(x * x + y * y);
  float vx0, vy0, vx1, vy1, x0, y0, x1, y1, px, py;
  x0 = 0;
  y0 = 0.5;
  vx0 = 0.5;
  vy0 = -1.0;
  x1 = 0;
  y1 = 0;
  vx1 = sqrt(x * x + y * y);
  vy1 = z;
  py = -1.0 * (vy1 * (vx0 / vy0 * y0 + x0 - x1) - vx1 * y1) / (vx1 - vy1 / vy0 * vx0);
  px = vx0 * (py - y0) / vy0 + x0;

  if (fabsf(py) > 0.5) {
    n = anchor.norm();
    py = n * 0.5 / fabsf(z);
    anchor.setX(x * py / n);
    anchor.setY(y * py / n);
    anchor.setZ(z * py / n);
  } else {
    anchor.setX(x * px / n);
    anchor.setY(y * px / n);
    anchor.setZ(py);
  }

  return anchor;
}

class EECone : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("3D - Cone extremity", "Bertrand Mathieu", "09/07/2002",
                   "Textured cone for edge extremities", "1.0", EdgeExtremityShape::Cone)

  EECone(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}
  ~EECone() override = default;

  void draw(edge e, node /*n*/, const Color &glyphColor, const Color & /*borderColor*/,
            float /*lod*/) override {
    glEnable(GL_LIGHTING);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    setMaterial(glyphColor);
    string texFile = edgeExtGlGraphInputData->textures()->getEdgeValue(e);

    if (!texFile.empty()) {
      string texturePath = edgeExtGlGraphInputData->renderingParameters()->getTexturePath();
      GlTextureManager::activateTexture(texturePath + texFile);
    }

    drawCone();
    GlTextureManager::deactivateTexture();
  }
};

PLUGIN(EECone)

}
