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

#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlBox.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

/// A 3D glyph.
/** This glyph draws a textured cube using the "viewTexture" node
 * property value. If this property has no value, the cube is then colored
 * using the "viewColor" node property value.
 */
class Cube : public NoShaderGlyph {
public:
  GLYPHINFORMATION("3D - Cube", "Bertrand Mathieu", "09/07/2002", "Textured cube", "1.0",
                   NodeShape::Cube)
  Cube(const tlp::PluginContext *context = nullptr);
  ~Cube() override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;

protected:
};

PLUGIN(Cube)

Cube::Cube(const tlp::PluginContext *context) : NoShaderGlyph(context) {}

Cube::~Cube() = default;

void Cube::draw(node n, float lod) {
  string textureName = glGraphInputData->textures()->getNodeValue(n);
  if (!textureName.empty()) {
    textureName = textureName + glGraphInputData->renderingParameters()->getTexturePath();
  }

  GlBox::draw(glGraphInputData->colors()->getNodeValue(n),
              glGraphInputData->colors()->getNodeValue(n),
              glGraphInputData->borderWidths()->getNodeValue(n), textureName, lod);
}
Coord Cube::getAnchor(const Coord &vector) const {
  return GlBox::getAnchor(vector);
}

class EECube : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("3D - Cube extremity", "Bertrand Mathieu", "09/07/2002",
                   "Textured cube for edge extremities", "1.0", EdgeExtremityShape::Cube)

  EECube(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->textures()->getEdgeValue(e);
    if (!textureName.empty()) {
      textureName = textureName + edgeExtGlGraphInputData->renderingParameters()->getTexturePath();
    }

    glEnable(GL_LIGHTING);
    GlBox::draw(glyphColor, borderColor, edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e),
                textureName, lod);
    glDisable(GL_LIGHTING);
  }
};

PLUGIN(EECube)

}
