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

#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>
#include <talipot/GlSphere.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawGlyph(const Color &glyphColor, const string &texture, const string &texturePath) {
  static GlSphere sphere(Coord(0, 0, 0), 0.5);

  sphere.setColor(glyphColor);
  sphere.setTexture(texturePath + texture);

  sphere.draw(0, nullptr);
}

/// A 3D glyph.
/**
 * This glyph draws a textured sphere using the "viewTexture" node
 * property value. If this property has no value, the sphere
 * is then colored using the "viewColor" node property value.
 */
class Sphere : public NoShaderGlyph {
public:
  GLYPHINFORMATION("3D - Sphere", "Bertrand Mathieu", "09/07/2002", "Textured sphere", "1.0",
                   NodeShape::Sphere)
  Sphere(const tlp::PluginContext *context = nullptr);
  ~Sphere() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
};

PLUGIN(Sphere)

//=========================================================================================
Sphere::Sphere(const tlp::PluginContext *context) : NoShaderGlyph(context) {}

Sphere::~Sphere() = default;

BoundingBox Sphere::getIncludeBoundingBox(node) {
  return {{-0.35f, -0.35f, -0.35f}, {0.35f, 0.35f, 0.35f}};
}

void Sphere::draw(node n, float) {
  drawGlyph(glGraphInputData->getElementColor()->getNodeValue(n),
            glGraphInputData->getElementTexture()->getNodeValue(n),
            glGraphInputData->parameters->getTexturePath());
}

class EESphere : public EdgeExtremityGlyph {
  GLYPHINFORMATION("3D - Sphere extremity", "Bertrand Mathieu", "09/07/2002",
                   "Textured sphere for edge extremities", "1.0", EdgeExtremityShape::Sphere)
public:
  EESphere(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}
  ~EESphere() override = default;
  void draw(edge e, node, const Color &glyphColor, const Color &, float) override {
    glEnable(GL_LIGHTING);
    drawGlyph(glyphColor, edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e),
              edgeExtGlGraphInputData->parameters->getTexturePath());
  }
};

PLUGIN(EESphere)

}
