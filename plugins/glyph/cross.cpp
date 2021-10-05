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
#include <talipot/GlComplexPolygon.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

static unique_ptr<GlComplexPolygon> cross;
static void initCross() {
  if (!cross.get()) {
    float bWidth = 0.1f;
    vector<Coord> points;
    points.push_back({-bWidth, 0.5});
    points.push_back({bWidth, 0.5});
    points.push_back({bWidth, bWidth});
    points.push_back({0.5, bWidth});
    points.push_back({0.5, -bWidth});
    points.push_back({bWidth, -bWidth});
    points.push_back({bWidth, -0.5});
    points.push_back({-bWidth, -0.5});
    points.push_back({-bWidth, -bWidth});
    points.push_back({-0.5, -bWidth});
    points.push_back({-0.5, bWidth});
    points.push_back({-bWidth, bWidth});
    cross.reset(new GlComplexPolygon(points, Color()));
  }
}
void drawCross(const Color &fillColor, const Color &borderColor, float borderWidth,
               const std::string &textureName, float lod) {

  cross->setFillColor(fillColor);

  if (borderWidth > 0) {
    cross->setOutlineMode(true);
    cross->setOutlineColor(borderColor);
    cross->setOutlineSize(borderWidth);
  } else {
    cross->setOutlineMode(false);
  }

  cross->setTextureName(textureName);
  cross->draw(lod, nullptr);
}

/// A 2D glyph
/**
 * This glyph draws a textured cross using the "viewTexture"
 * node property value. If this property has no value, the cross
 * is then colored using the "viewColor" node property value.
 */
class Cross : public Glyph {
public:
  GLYPHINFORMATION("2D - Cross", "Patrick Mary", "23/06/2011", "Textured Cross", "1.0",
                   NodeShape::Cross)
  Cross(const tlp::PluginContext *context = nullptr);
  ~Cross() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};
PLUGIN(Cross)
/*
 *
     0---1                       (-bWidth, 0.5)  (bWidth, 0.5)
     |   |
     |   |
10--11   2----3 (-0.5, bWidth)  (-bWidth, bWidth)(bWidth, bWidth) (0.5, bWidth)
|             |
9----8   5----4 (-0.5, -bWidth)(-bWidth, -bWidth)(bWidth, -bWidth)(0.5, -bWidth)
     |   |
     |   |
     7---6                       (-bWidth, -0.5)  (bWidth, -0.5)
*/

Cross::Cross(const tlp::PluginContext *context) : Glyph(context) {
  initCross();
}
Cross::~Cross() = default;
BoundingBox Cross::getIncludeBoundingBox(node) {
  return {{-0.5, -0.5, 0}, {0.5, 0.5, 0}};
}
void Cross::draw(node n, float lod) {
  string textureName = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->parameters->getTexturePath() + textureName;
  }

  drawCross(glGraphInputData->getElementColor()->getNodeValue(n),
            glGraphInputData->getElementBorderColor()->getNodeValue(n),
            glGraphInputData->getElementBorderWidth()->getNodeValue(n), textureName, lod);
}
Coord Cross::getAnchor(const Coord &v) const {
  float x = v.x(), y = v.y();
  // initialize anchor as the middle of segment points[0], points[1]
  Coord anchor = {0, 0.5, 0};
  float distMin = x * x + ((y - 0.5) * (y - 0.5));
  // check with the middle of segment points[3], points[4]
  float dist = ((x - 0.5) * (x - 0.5)) + y * y;

  if (distMin > dist) {
    distMin = dist;
    anchor = {0.5, 0, 0};
  }

  // check with the middle of segment points[7], points[6]
  dist = x * x + ((y + 0.5) * (y + 0.5));

  if (distMin > dist) {
    distMin = dist;
    anchor = {0, -0.5, 0};
  }

  // check with the middle of segment points[9], points[10]
  if (distMin > ((x + 0.5) * (x + 0.5)) + y * y) {
    anchor = {-0.5, 0, 0};
  }
  return anchor;
}

class EECross : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("2D - Cross extremity", "Patrick Mary", "23/06/2011",
                   "Textured Cross for edge extremities", "1.0", EdgeExtremityShape::Cross)

  EECross(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {
    initCross();
  }

  void draw(edge e, node, const Color &glyphColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->getElementTexture()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->parameters->getTexturePath() + textureName;
    }

    drawCross(glyphColor, borderColor,
              edgeExtGlGraphInputData->getElementBorderWidth()->getEdgeValue(e), textureName, lod);
  }
};
PLUGIN(EECross)

}
