/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
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
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>
#include <talipot/DrawingTools.h>
#include <talipot/OpenGlConfigManager.h>

using namespace std;
using namespace tlp;

namespace tlp {

class CylinderBase {

public:
  void drawCylinder(float cylinderHeight = 1.0, float dz = 0) {
    if (cylinderVertices.empty()) {
      const uint numberOfSides = 30;
      cylinderVertices = computeRegularPolygon(numberOfSides, Coord(0, 0, -cylinderHeight / 2 + dz),
                                               Size(0.5, 0.5));
      cylinderVertices.push_back(Coord(0, 0, -cylinderHeight / 2 + dz));
      vector<Coord> tmp = computeRegularPolygon(numberOfSides, Coord(0, 0, cylinderHeight / 2 + dz),
                                                Size(0.5, 0.5));
      cylinderVertices.insert(cylinderVertices.end(), tmp.begin(), tmp.end());
      cylinderVertices.push_back(Coord(0, 0, cylinderHeight / 2 + dz));

      for (const auto &cylinderVertice : cylinderVertices) {
        cylinderTexCoords.push_back(Vec2f(cylinderVertice[0] + 0.5, cylinderVertice[1] + 0.5));
      }

      size_t startIdx = cylinderVertices.size();

      tmp = computeRegularPolygon(numberOfSides, Coord(0, 0, -cylinderHeight / 2 + dz),
                                  Size(0.5, 0.5));
      cylinderVertices.insert(cylinderVertices.end(), tmp.begin(), tmp.end());
      tmp = computeRegularPolygon(numberOfSides, Coord(0, 0, cylinderHeight / 2 + dz),
                                  Size(0.5, 0.5));
      cylinderVertices.insert(cylinderVertices.end(), tmp.begin(), tmp.end());

      for (size_t i = startIdx; i < cylinderVertices.size(); ++i) {
        cylinderTexCoords.push_back(
            Vec2f(cylinderVertices[i][0] + 0.5, cylinderVertices[i][2] + 0.5));
      }

      for (uint i = 0; i < numberOfSides - 1; ++i) {
        cylinderIndices.push_back(numberOfSides);
        cylinderIndices.push_back(i + 1);
        cylinderIndices.push_back(i);
      }

      cylinderIndices.push_back(numberOfSides);
      cylinderIndices.push_back(0);
      cylinderIndices.push_back(numberOfSides - 1);

      for (uint i = 0; i < numberOfSides - 1; ++i) {
        cylinderIndices.push_back(numberOfSides + 1 + i);
        cylinderIndices.push_back(numberOfSides + 1 + i + 1);
        cylinderIndices.push_back(2 * numberOfSides);
      }

      cylinderIndices.push_back(2 * numberOfSides);
      cylinderIndices.push_back(numberOfSides + 1);
      cylinderIndices.push_back(numberOfSides + 1 + numberOfSides - 1);

      for (uint i = 0; i < numberOfSides - 1; ++i) {
        cylinderIndices.push_back(startIdx + i);
        cylinderIndices.push_back(startIdx + i + 1);
        cylinderIndices.push_back(startIdx + numberOfSides + i);

        cylinderIndices.push_back(startIdx + i + 1);
        cylinderIndices.push_back(startIdx + numberOfSides + i + 1);
        cylinderIndices.push_back(startIdx + numberOfSides + i);
      }

      cylinderIndices.push_back(startIdx + numberOfSides - 1);
      cylinderIndices.push_back(startIdx);
      cylinderIndices.push_back(startIdx + numberOfSides);

      cylinderIndices.push_back(startIdx + numberOfSides);
      cylinderIndices.push_back(startIdx + numberOfSides + numberOfSides - 1);
      cylinderIndices.push_back(startIdx + numberOfSides - 1);

      cylinderNormals = computeNormals(cylinderVertices, cylinderIndices);

      buffers.resize(4);
      glGenBuffers(4, &buffers[0]);

      glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
      glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * 3 * sizeof(float),
                   &cylinderVertices[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
      glBufferData(GL_ARRAY_BUFFER, cylinderNormals.size() * 3 * sizeof(float), &cylinderNormals[0],
                   GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
      glBufferData(GL_ARRAY_BUFFER, cylinderTexCoords.size() * 2 * sizeof(float),
                   &cylinderTexCoords[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(unsigned short),
                   &cylinderIndices[0], GL_STATIC_DRAW);
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
    glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

private:
  vector<Coord> cylinderVertices;
  vector<Coord> cylinderNormals;
  vector<Vec2f> cylinderTexCoords;
  vector<unsigned short> cylinderIndices;
  vector<uint> buffers;
};

/// A 3D glyph.
/**
 * This glyph draws an textured cylinder using the "viewTexture" node
 * property value. If this property has no value, the cylinder is then colored
 * using the "viewColor" node property value.
 */
class Cylinder : public NoShaderGlyph, public CylinderBase {
public:
  GLYPHINFORMATION("3D - Cylinder", "Bertrand Mathieu", "31/07/2002", "Textured Cylinder", "1.0",
                   NodeShape::Cylinder)
  Cylinder(const tlp::PluginContext *context = nullptr);
  ~Cylinder() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};

PLUGIN(Cylinder)

Cylinder::Cylinder(const tlp::PluginContext *context) : NoShaderGlyph(context) {}

Cylinder::~Cylinder() = default;

BoundingBox Cylinder::getIncludeBoundingBox(node) {
  return {{-0.35f, -0.35f, 0}, {0.35f, 0.35f, 1}};
}

void Cylinder::draw(node n, float) {

  setMaterial(glGraphInputData->getElementColor()->getNodeValue(n));
  string texFile = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!texFile.empty()) {
    string texturePath = glGraphInputData->parameters->getTexturePath();
    GlTextureManager::activateTexture(texturePath + texFile);
  }

  drawCylinder();

  GlTextureManager::deactivateTexture();
}

Coord Cylinder::getAnchor(const Coord &v) const {
  float x = v.x(), y = v.y(), z = v.z();
  float n = sqrt(x * x + y * y);

  if (n == 0.0f) {
    return v;
  }

  n = 0.5f / n;
  x *= n;
  y *= n;
  z *= n;

  if (z < -0.5f) {
    z = -0.5f; // z = z >? -0.5f;
  }

  if (z > +0.5f) {
    z = +0.5f; // z = z <? +0.5f;
  }

  return {x, y, z};
}

/// A 3D glyph.
/**
 * This glyph draws an textured a half cylinder - a cylinder whose height
 * is half the one draws by the Cylinder glyph - using the "viewTexture"
 * node property value. If this property has no value, the half cylinder
 * is then colored using the "viewColor" node property value.
 */

//=================================================================================================
class HalfCylinder : public NoShaderGlyph, public CylinderBase {
public:
  GLYPHINFORMATION("3D - Half Cylinder", "Auber David", "31/07/2002", "Textured HalfCylinder",
                   "1.0", NodeShape::HalfCylinder)
  HalfCylinder(const tlp::PluginContext *context = nullptr);
  ~HalfCylinder() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;

private:
};

PLUGIN(HalfCylinder)
//=================================================================================================
HalfCylinder::HalfCylinder(const tlp::PluginContext *context) : NoShaderGlyph(context) {}
//=================================================================================================
HalfCylinder::~HalfCylinder() = default;
//=====================================================
BoundingBox HalfCylinder::getIncludeBoundingBox(node) {
  return {{-0.35f, -0.35f, 0}, {0.35f, 0.35f, 1}};
}
//=================================================================================================
void HalfCylinder::draw(node n, float) {

  setMaterial(glGraphInputData->getElementColor()->getNodeValue(n));
  string texFile = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!texFile.empty()) {
    string texturePath = glGraphInputData->parameters->getTexturePath();
    GlTextureManager::activateTexture(texturePath + texFile);
  }

  drawCylinder(0.5, 0.25);

  GlTextureManager::deactivateTexture();
}
//=================================================================================================
Coord HalfCylinder::getAnchor(const Coord &v) const {
  float n = sqrt(v.x() * v.x() + v.y() * v.y());

  if (n == 0.0f) {
    return v;
  }

  return v * (0.5f / n);
}

class EECylinder : public EdgeExtremityGlyph, public CylinderBase {
public:
  GLYPHINFORMATION("3D - Cylinder extremity", "Bertrand Mathieu", "31/07/2002",
                   "Textured Cylinder for edge extremities", "1.0", EdgeExtremityShape::Cylinder)

  EECylinder(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge, node n, const Color &glyphColor, const Color &, float) override {
    glEnable(GL_LIGHTING);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    setMaterial(glyphColor);
    string texFile = edgeExtGlGraphInputData->getElementTexture()->getNodeValue(n);

    if (!texFile.empty()) {
      string texturePath = edgeExtGlGraphInputData->parameters->getTexturePath();
      GlTextureManager::activateTexture(texturePath + texFile);
    }

    drawCylinder();

    GlTextureManager::deactivateTexture();
  }
};

PLUGIN(EECylinder)

}
