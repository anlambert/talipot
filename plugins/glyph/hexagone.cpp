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
#include <talipot/GlHexagon.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawHexagon(const Color &fillColor, const Color &borderColor, float borderWidth,
                        const std::string &textureName, float lod, bool mode) {
  static GlHexagon hexagon(Coord(0, 0, 0), Size(.5, .5, 0));

  hexagon.setLightingMode(mode);
  hexagon.setFillColor(fillColor);

  if (borderWidth > 0) {
    hexagon.setOutlineMode(true);
    hexagon.setOutlineColor(borderColor);
    hexagon.setOutlineSize(borderWidth);
  } else {
    hexagon.setOutlineMode(false);
  }

  hexagon.setTextureName(textureName);
  hexagon.draw(lod, nullptr);
}

/** \addtogroup glyph */

/// A 2D glyph
/**
 * This glyph draws a textured hexagon using the "viewTexture"
 * node property value. If this property has no value, the hexagon
 * is then colored using the "viewColor" node property value.
 */
class Hexagon : public Glyph {
public:
  GLYPHINFORMATION("2D - Hexagon", "David Auber", "09/07/2002", "Textured Hexagon", "1.0",
                   NodeShape::Hexagon)
  Hexagon(const tlp::PluginContext *context = nullptr);
  ~Hexagon() override;
  void getIncludeBoundingBox(BoundingBox &boundingBox, node) override;
  void draw(node n, float lod) override;
};

PLUGIN(Hexagon)

Hexagon::Hexagon(const tlp::PluginContext *context) : Glyph(context) {}

Hexagon::~Hexagon() = default;

void Hexagon::getIncludeBoundingBox(BoundingBox &boundingBox, node) {
  boundingBox[0] = Coord(-0.35f, -0.35f, 0);
  boundingBox[1] = Coord(0.35f, 0.35f, 0);
}

void Hexagon::draw(node n, float lod) {
  string textureName = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->parameters->getTexturePath() + textureName;
  }

  drawHexagon(glGraphInputData->getElementColor()->getNodeValue(n),
              glGraphInputData->getElementBorderColor()->getNodeValue(n),
              glGraphInputData->getElementBorderWidth()->getNodeValue(n), textureName, lod, true);
}

class EEHexagon : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Hexagon extremity", "David Auber", "09/07/2002",
                   "Textured Hexagon for edge extremities", "1.0", EdgeExtremityShape::Hexagon)

  EEHexagon(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->parameters->getTexturePath() + textureName;
    }

    drawHexagon(glyphColor, borderColor,
                edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e), textureName, lod,
                false);
  }
};

PLUGIN(EEHexagon)

} // end of namespace tlp
