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

#include <tulip/EdgeExtremityGlyph.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/GlTriangle.h>
#include <tulip/Glyph.h>
#include <tulip/TulipViewSettings.h>

using namespace tlp;
using namespace std;
class GlArrow2DEdgeExtremity : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Arrow", "Jonathan Dubois", "09/04/09", "Edge Extremity with 2D arrow",
                   "1.0", EdgeExtremityShape::Arrow)
  GlArrow2DEdgeExtremity(tlp::PluginContext *gc);
  ~GlArrow2DEdgeExtremity() override;
  void draw(edge e, node n, const Color &glyphColor, const Color &borderColor, float lod) override;

protected:
  static GlTriangle *triangle;
};

PLUGIN(GlArrow2DEdgeExtremity)

GlTriangle *GlArrow2DEdgeExtremity::triangle = nullptr;

GlArrow2DEdgeExtremity::GlArrow2DEdgeExtremity(tlp::PluginContext *gc) : EdgeExtremityGlyph(gc) {
  if (!triangle) {
    triangle = new GlTriangle(Coord(0, 0, 0), Size(0.5, 0.5, 0.5));
    triangle->setLightingMode(false);
    triangle->setStartAngle(0);
  }
}

GlArrow2DEdgeExtremity::~GlArrow2DEdgeExtremity() {}

void GlArrow2DEdgeExtremity::draw(edge e, node, const Color &glyphColor, const Color &borderColor,
                                  float lod) {

  double width = edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e);

  triangle->setFillColor(glyphColor);
  triangle->setOutlineSize(width);
  triangle->setOutlineColor(borderColor);
  triangle->draw(lod, nullptr);
}
