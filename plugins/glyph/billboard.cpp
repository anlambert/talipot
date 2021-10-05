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
#include <talipot/GlRect.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/ViewSettings.h>

using namespace std;
using namespace tlp;

namespace tlp {

/// A 2D glyph.
/**
 * This glyph draws a textured square using the "viewTexture"
 * node property value. If this property has no value, the square is
 *  then colored using the "viewColor" node property value.
 * It is unsensitive to any axis rotation and so always remains displayed
 * in the same position.
 */
class Billboard : public NoShaderGlyph {
public:
  GLYPHINFORMATION("2D - Billboard", "Gerald Gainant", "08/03/2004", "Textured billboard", "1.0",
                   NodeShape::Billboard)
  Billboard(const tlp::PluginContext *context = nullptr);
  ~Billboard() override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;
};

PLUGIN(Billboard)

//===================================================================================
Billboard::Billboard(const tlp::PluginContext *context) : NoShaderGlyph(context) {}
//========================================================
Billboard::~Billboard() = default;
//========================================================
void Billboard::draw(node n, float lod) {
  static GlRect rect(Coord(0, 0, 0), 1., 1., Color(0, 0, 0, 255), Color(0, 0, 0, 255));

  rect.setFillColor(glGraphInputData->getElementColor()->getNodeValue(n));

  string texFile = glGraphInputData->getElementTexture()->getNodeValue(n);

  if (!texFile.empty()) {
    string texturePath = glGraphInputData->parameters->getTexturePath();
    rect.setTextureName(texturePath + texFile);
  } else {
    rect.setTextureName("");
  }

  double borderWidth = glGraphInputData->getElementBorderWidth()->getNodeValue(n);

  if (borderWidth > 0) {
    rect.setOutlineMode(true);
    rect.setOutlineColor(glGraphInputData->getElementBorderColor()->getNodeValue(n));
    rect.setOutlineSize(borderWidth);
  } else {
    rect.setOutlineMode(false);
  }

  Size sz = {1};

  if (glGraphInputData->getElementSize()) {
    sz = glGraphInputData->getElementSize()->getNodeValue(n);
  }

  // draw rect in the screen plane
  float mdlM[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mdlM);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  float nx = sz.getW();
  float ny = sz.getH();
  float nz = sz.getD();
  mdlM[0] = nx;
  mdlM[5] = ny;
  mdlM[10] = nz;
  mdlM[1] = mdlM[2] = 0.0f;
  mdlM[4] = mdlM[6] = 0.0f;
  mdlM[8] = mdlM[9] = 0.0f;
  glLoadMatrixf(mdlM);
  rect.draw(lod, nullptr);
  glPopMatrix();
}
//========================================================
Coord Billboard::getAnchor(const Coord &v) const {
  float fmax = std::max(fabsf(v.x()), fabsf(v.y()));

  if (fmax > 0.0f) {
    return v * (0.5f / fmax);
  } else {
    return v;
  }
}
//========================================================

} // end of namespace tlp
