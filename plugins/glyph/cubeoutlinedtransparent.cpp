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

#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/Glyph.h>
#include <talipot/EdgeExtremityGlyph.h>
#include <talipot/GlBox.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

/// A 3D glyph.
/** This glyph draws a transparent cube using the "viewBorderColor" node
 * property value to draw its edges.
 */
class CubeOutLinedTransparent : public Glyph {
public:
  GLYPHINFORMATION("3D - Cube OutLined Transparent", "David Auber", "09/07/2002",
                   "Textured cubeOutLined", "1.0", NodeShape::CubeOutlinedTransparent)
  CubeOutLinedTransparent(const tlp::PluginContext *context = nullptr);
  ~CubeOutLinedTransparent() override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};

PLUGIN(CubeOutLinedTransparent)

CubeOutLinedTransparent::CubeOutLinedTransparent(const tlp::PluginContext *context)
    : Glyph(context) {}

CubeOutLinedTransparent::~CubeOutLinedTransparent() = default;

void CubeOutLinedTransparent::draw(node n, float lod) {
  GlBox::draw(Color(0, 0, 0, 0), glGraphInputData->borderColors()->getNodeValue(n),
              glGraphInputData->borderWidths()->getNodeValue(n), "", lod);
}

Coord CubeOutLinedTransparent::getAnchor(const Coord &vector) const {
  return GlBox::getAnchor(vector);
}

class EECubeOutlinedTransparent : public EdgeExtremityGlyph {
public:
  GLYPHINFORMATION("3D - Cube OutLined Transparent extremity", "David Auber", "09/07/2002",
                   "Textured cubeOutLined for edge extremities", "1.0",
                   EdgeExtremityShape::CubeOutlinedTransparent)

  EECubeOutlinedTransparent(const tlp::PluginContext *context) : EdgeExtremityGlyph(context) {}

  void draw(edge e, node, const Color &fillColor, const Color &borderColor, float lod) override {
    string textureName = edgeExtGlGraphInputData->textures()->getEdgeValue(e);

    if (!textureName.empty()) {
      textureName = edgeExtGlGraphInputData->renderingParameters()->getTexturePath() + textureName;
    }

    glEnable(GL_LIGHTING);
    GlBox::draw(fillColor, borderColor, edgeExtGlGraphInputData->borderWidths()->getEdgeValue(e),
                textureName, lod);
    glDisable(GL_LIGHTING);
  }
};

PLUGIN(EECubeOutlinedTransparent)

}
