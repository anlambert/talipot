/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/StringProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/Size.h>
#include <talipot/Coord.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlRegularPolygon.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Graph.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawDiamond(const Color &fillColor, const Color &borderColor, float borderWidth,
                        const std::string &textureName, float lod, bool mode) {
  static GlRegularPolygon diamond(Coord(0, 0, 0), Size(.5, .5, 0), 4);
  diamond.setLightingMode(mode);
  diamond.setFillColor(fillColor);

  if (borderWidth > 0) {
    diamond.setOutlineMode(true);
    diamond.setOutlineColor(borderColor);
    diamond.setOutlineSize(borderWidth);
  } else {
    diamond.setOutlineMode(false);
  }

  diamond.setTextureName(textureName);
  diamond.draw(lod, nullptr);
}

/** \addtogroup glyph */

/// A 2D glyph
/**
 * This glyph draws a textured diamond using the "viewTexture"
 * node property value. If this property has no value, the diamond
 * is then colored using the "viewColor" node property value.
 */
class Diamond : public Glyph {
public:
  GLYPHINFORMATION("2D - Diamond", "Patrick Mary", "23/06/2011", "Textured Diamond", "1.0",
                   NodeShape::Diamond)
  Diamond(const tlp::PluginContext *context = nullptr);
  ~Diamond() override;
  void getIncludeBoundingBox(BoundingBox &boundingBox, node) override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};
PLUGIN(Diamond)
Diamond::Diamond(const tlp::PluginContext *context) : Glyph(context) {}
Diamond::~Diamond() {}
void Diamond::getIncludeBoundingBox(BoundingBox &boundingBox, node) {
  boundingBox[0] = Coord(-0.35f, -0.35f, 0);
  boundingBox[1] = Coord(0.35f, 0.35f, 0);
}
void Diamond::draw(node n, float lod) {
  string textureName = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!textureName.empty())
    textureName = glGraphInputData->parameters->getTexturePath() + textureName;

  drawDiamond(glGraphInputData->getElementColor()->getNodeValue(n),
              glGraphInputData->getElementBorderColor()->getNodeValue(n),
              glGraphInputData->getElementBorderWidth()->getNodeValue(n), textureName, lod, true);
}
Coord Diamond::getAnchor(const Coord &vector) const {
  Coord v(vector);
  float x, y, z;
  v.get(x, y, z);
  // initialize anchor as top corner
  Coord anchor(0, 0.5, 0);
  float distMin = x * x + ((y - 0.5) * (y - 0.5));
  // check with the right corner
  float dist = ((x - 0.5) * (x - 0.5)) + y * y;

  if (distMin > dist) {
    distMin = dist;
    anchor = Coord(0.5, 0, 0);
  }

  // check with the bottom corner
  dist = x * x + ((y + 0.5) * (y + 0.5));

  if (distMin > dist) {
    distMin = dist;
    anchor = Coord(0, -0.5, 0);
  }

  // check with left corner
  if (distMin > ((x + 0.5) * (x + 0.5)) + y * y)
    return Coord(-0.5, 0, 0);

  return anchor;
}

class EEDiamond : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Diamond extremity", "Patrick Mary", "23/06/2011",
                   "Textured Diamond for edge extremities", "1.0", EdgeExtremityShape::Diamond)

  EEDiamond(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e);

    if (!textureName.empty())
      textureName = edgeExtGlGraphInputData->parameters->getTexturePath() + textureName;

    drawDiamond(glyphColor, borderColor,
                edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e), textureName, lod,
                false);
  }
};

PLUGIN(EEDiamond)

} // end of namespace tlp
