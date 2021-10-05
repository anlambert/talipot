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
#include <talipot/GlPentagon.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawPentagon(const Color &fillColor, const Color &borderColor, float borderWidth,
                         const std::string &textureName, float lod, bool mode) {
  static GlPentagon pentagon(Coord(0, 0, 0), Size(.5, .5, 0));
  pentagon.setLightingMode(mode);
  pentagon.setFillColor(fillColor);

  if (borderWidth > 0) {
    pentagon.setOutlineMode(true);
    pentagon.setOutlineColor(borderColor);
    pentagon.setOutlineSize(borderWidth);
  } else {
    pentagon.setOutlineMode(false);
  }

  pentagon.setTextureName(textureName);
  pentagon.draw(lod, nullptr);
}

/// A 2D glyph
/**
 * This glyph draws a textured pentagon using the "viewTexture"
 * node property value. If this property has no value, the pentagon
 * is then colored using the "viewColor" node property value.
 */
class Pentagon : public Glyph {
public:
  GLYPHINFORMATION("2D - Pentagon", "David Auber", "09/07/2002", "Textured Pentagon", "1.0",
                   NodeShape::Pentagon)
  Pentagon(const tlp::PluginContext *context = nullptr);
  ~Pentagon() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
};

PLUGIN(Pentagon)

Pentagon::Pentagon(const tlp::PluginContext *context) : Glyph(context) {}

Pentagon::~Pentagon() = default;

BoundingBox Pentagon::getIncludeBoundingBox(node) {
  return {{-0.3f, -0.35f, 0}, {0.3f, 0.35f, 0}};
}
void Pentagon::draw(node n, float lod) {
  string textureName = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->parameters->getTexturePath() + textureName;
  }

  drawPentagon(glGraphInputData->getElementColor()->getNodeValue(n),
               glGraphInputData->getElementBorderColor()->getNodeValue(n),
               glGraphInputData->getElementBorderWidth()->getNodeValue(n), textureName, lod, true);
}

class EEPentagon : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Pentagon extremity", "David Auber", "09/07/2002",
                   "Textured Pentagon for edge extremities", "1.0", EdgeExtremityShape::Pentagon)

  EEPentagon(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->parameters->getTexturePath() + textureName;
    }

    drawPentagon(glyphColor, borderColor,
                 edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e), textureName,
                 lod, false);
  }
};

PLUGIN(EEPentagon)

}
