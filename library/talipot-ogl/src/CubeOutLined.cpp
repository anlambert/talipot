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
#include <talipot/GlBox.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;

namespace tlp {

/// A 3D glyph.
/** This glyph draws a textured cube using the "viewTexture" node
 * property value. If this property has no value, the cube is then colored
 * using the "viewColor" node property value. The "viewBorderColor"
 * node property value is used to draw its edges.
 */
class CubeOutLined : public NoShaderGlyph {
public:
  GLYPHINFORMATION("3D - Cube OutLined", "David Auber", "09/07/2002", "Textured cubeOutLined",
                   "1.0", NodeShape::CubeOutlined)
  CubeOutLined(tlp::PluginContext *context);
  ~CubeOutLined() override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};

PLUGIN(CubeOutLined)

//===================================================================================
CubeOutLined::CubeOutLined(tlp::PluginContext *context) : NoShaderGlyph(context) {}

CubeOutLined::~CubeOutLined() = default;

void CubeOutLined::draw(node n, float lod) {
  string textureName = glGraphInputData->textures()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->renderingParameters()->getTexturePath() + textureName;
  }

  GlBox::draw(glGraphInputData->colors()->getNodeValue(n),
              glGraphInputData->borderColors()->getNodeValue(n),
              glGraphInputData->borderWidths()->getNodeValue(n), textureName, lod);
}

Coord CubeOutLined::getAnchor(const Coord &vector) const {
  return GlBox::getAnchor(vector);
}

}
