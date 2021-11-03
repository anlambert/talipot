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
#include <talipot/GlTriangle.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

class Triangle : public Glyph {
public:
  GLYPHINFORMATION("2D - Triangle", "David Auber", "09/07/2002", "Textured Triangle", "1.0",
                   NodeShape::Triangle)
  Triangle(const tlp::PluginContext *context = nullptr);
  ~Triangle() override;
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float lod) override;
};

//=====================================================
PLUGIN(Triangle)
//===================================================================================
Triangle::Triangle(const tlp::PluginContext *context) : Glyph(context) {}
//=====================================================
Triangle::~Triangle() = default;
//=====================================================
BoundingBox Triangle::getIncludeBoundingBox(node) {
  return {{-0.25, -0.5, 0}, {0.25, 0, 0}};
}
//=====================================================
void Triangle::draw(node n, float lod) {
  GlTriangle triangle(Coord(0, 0, 0), Size(0.5, 0.5, 0));

  triangle.setFillColor(glGraphInputData->colors()->getNodeValue(n));

  string texFile = glGraphInputData->textures()->getNodeValue(n);

  if (!texFile.empty()) {
    string texturePath = glGraphInputData->renderingParameters()->getTexturePath();
    triangle.setTextureName(texturePath + texFile);
  } else {
    triangle.setTextureName("");
  }

  double lineWidth = glGraphInputData->borderWidths()->getNodeValue(n);

  if (lineWidth > 0) {
    triangle.setOutlineMode(true);
    triangle.setOutlineColor(glGraphInputData->borderColors()->getNodeValue(n));
    triangle.setOutlineSize(lineWidth);
  } else {
    triangle.setOutlineMode(false);
  }

  triangle.draw(lod, nullptr);
}
//=====================================================

}
