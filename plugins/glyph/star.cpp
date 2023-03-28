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
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlStar.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawStar(const Color &fillColor, const Color &borderColor, float borderWidth,
                     const std::string &textureName, float lod) {
  static GlStar star(Coord(0, 0, 0), Size(.5, .5, 0), 5);
  star.setFillColor(fillColor);

  if (borderWidth > 0) {
    star.setOutlineMode(true);
    star.setOutlineColor(borderColor);
    star.setOutlineSize(borderWidth);
  } else {
    star.setOutlineMode(false);
  }

  star.setTextureName(textureName);
  star.draw(lod, nullptr);
}

/// A 2D glyph
/**
 * This glyph draws a textured star using the "viewTexture"
 * node property value. If this property has no value, the star
 * is then colored using the "viewColor" node property value.
 */
class Star : public Glyph {
public:
  GLYPHINFORMATION("2D - Star", "David Auber", "09/07/2002", "Textured Star", "1.0",
                   NodeShape::Star)
  Star(const tlp::PluginContext *context = nullptr);
  ~Star() override;
  BoundingBox getIncludeBoundingBox(node) override;
  void draw(node n, float lod) override;
};

PLUGIN(Star)

Star::Star(const tlp::PluginContext *context) : Glyph(context) {}

Star::~Star() = default;

BoundingBox Star::getIncludeBoundingBox(node) {
  return {{-0.3f, -0.35f, 0}, {0.3f, 0.35f, 0}};
}

void Star::draw(node n, float lod) {
  string textureName = glGraphInputData->textures()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->renderingParameters()->getTexturePath() + textureName;
  }

  drawStar(glGraphInputData->colors()->getNodeValue(n),
           glGraphInputData->borderColors()->getNodeValue(n),
           glGraphInputData->borderWidths()->getNodeValue(n), textureName, lod);
}

class EEStar : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Star extremity", "David Auber", "09/07/2002",
                   "Textured Star for edge extremities", "1.0", EdgeExtremityShape::Star)

  EEStar(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->textures()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->renderingParameters()->getTexturePath() + textureName;
    }

    drawStar(glyphColor, borderColor, edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e),
             textureName, lod);
  }
};

PLUGIN(EEStar)

}
