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

#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlCircle.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawCircle(const Color &fillColor, const Color &borderColor, float borderWidth,
                       const std::string &textureName, float lod, bool mode) {
  static GlCircle circle(Coord(0, 0, 0), 0.5, Color(0, 0, 0, 255), Color(0, 0, 0, 255), true, true,
                         0., 30);
  circle.setFillColor(fillColor);
  circle.setLightingMode(mode);

  if (borderWidth > 0) {
    circle.setOutlineMode(true);
    circle.setOutlineColor(borderColor);
    circle.setOutlineSize(borderWidth);
  } else {
    circle.setOutlineMode(false);
  }

  circle.setTextureName(textureName);
  circle.draw(lod, nullptr);
}

/// A 2D glyph.
/**
 * This glyph draws a textured disc using the "viewTexture" node
 * property value. If this property has no value, the disc is then colored
 * using the "viewColor" node property value.
 */
class Circle : public Glyph {
public:
  GLYPHINFORMATION("2D - Circle", "David Auber", "09/07/2002", "Textured Circle", "1.1",
                   NodeShape::Circle)
  Circle(const tlp::PluginContext *context = nullptr);
  ~Circle() override;
  BoundingBox getIncludeBoundingBox(node) override;
  void draw(node n, float lod) override;
};

PLUGIN(Circle)

Circle::Circle(const tlp::PluginContext *context) : Glyph(context) {}

Circle::~Circle() = default;

BoundingBox Circle::getIncludeBoundingBox(node) {
  return {{-0.35f, -0.35f, 0}, {0.35f, 0.35f, 0}};
}
void Circle::draw(node n, float lod) {
  string textureName = glGraphInputData->textures()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->renderingParameters()->getTexturePath() + textureName;
  }

  drawCircle(Glyph::glGraphInputData->colors()->getNodeValue(n),
             Glyph::glGraphInputData->borderColors()->getNodeValue(n),
             Glyph::glGraphInputData->borderWidths()->getNodeValue(n), textureName, lod, true);
}

class EECircle : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Circle extremity", "David Auber", "09/07/2002",
                   "Textured Circle for edge extremities", "1.1", EdgeExtremityShape::Circle)
  EECircle(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}
  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->textures()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->renderingParameters()->getTexturePath() + textureName;
    }

    drawCircle(glyphColor, borderColor, edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e),
               textureName, lod, false);
  }
};

PLUGIN(EECircle)

}
