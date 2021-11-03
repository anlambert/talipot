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
#include <talipot/GlRect.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static void drawGlyph(const Color &glyphColor, const string &texture, const string &texturePath,
                      double borderWidth, const Color &borderColor, float lod) {
  static GlRect rect(Coord(0, 0, 0), 1., 1., Color(0, 0, 0, 255), Color(0, 0, 0, 255));
  rect.setFillColor(glyphColor);
  rect.setTextureName(texturePath + texture);

  if (borderWidth > 0) {
    rect.setOutlineMode(true);
    rect.setOutlineColor(borderColor);
    rect.setOutlineSize(borderWidth);
  } else {
    rect.setOutlineMode(false);
  }

  rect.draw(lod, nullptr);
}

/// A 2D glyph.
/**
 * This glyph draws a textured square using the "viewTexture" node
 * property value. If this property has no value, the square
 * is then colored using the "viewColor" node property value.
 */
class Square : public Glyph {
public:
  GLYPHINFORMATION("2D - Square", "David Auber", "09/07/2002", "Textured square", "1.0",
                   NodeShape::Square)
  Square(const tlp::PluginContext *context = nullptr);
  ~Square() override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};

PLUGIN(Square)

Square::Square(const tlp::PluginContext *context) : Glyph(context) {}

Square::~Square() = default;

void Square::draw(node n, float lod) {
  drawGlyph(glGraphInputData->colors()->getNodeValue(n),
            glGraphInputData->textures()->getNodeValue(n),
            glGraphInputData->renderingParameters()->getTexturePath(),
            glGraphInputData->borderWidths()->getNodeValue(n),
            glGraphInputData->borderColors()->getNodeValue(n), lod);
}

Coord Square::getAnchor(const Coord &v) const {
  float fmax = std::max(fabsf(v.x()), fabsf(v.y()));
  if (fmax > 0.0f) {
    return v * (0.5f / fmax);
  } else {
    return v;
  }
}

class EESquare : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Square extremity", "David Auber", "09/07/2002",
                   "Textured square for edge extremities", "1.0", EdgeExtremityShape::Square)
  EESquare(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    glDisable(GL_LIGHTING);
    drawGlyph(glyphColor, edgeExtGlGraphInputData->textures()->getEdgeValue(e),
              edgeExtGlGraphInputData->renderingParameters()->getTexturePath(),
              edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e), borderColor, lod);
  }
};

PLUGIN(EESquare)

}
