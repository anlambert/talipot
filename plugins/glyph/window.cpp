/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef WINDOWGLYPH_H
#define WINDOWGLYPH_H

#include <talipot/Glyph.h>
#include <talipot/GlRect.h>
#include <talipot/GlLabel.h>
#include <talipot/GlPolyQuad.h>
#include <talipot/GlQuad.h>
#include <talipot/GlTextureManager.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/ColorProperty.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/TlpTools.h>
#include <talipot/ViewSettings.h>

using namespace tlp;
using namespace std;

namespace tlp {

class Window : public Glyph {
public:
  GLYPHINFORMATION("2D - Window", "David Auber", "28/05/2010", "Window with a title bar", "1.0",
                   NodeShape::Window)
  Window(const tlp::PluginContext *context);
  BoundingBox getIncludeBoundingBox(node) override;
  BoundingBox getTextBoundingBox(node) override;
  void draw(node n, float lod) override;
  Coord getAnchor(const Coord &vector) const override;

private:
  Color _rectColor;
  Color _textColor;
  const float _borderWidth;
  GlPolyQuad _border;
  GlQuad _center;
  GlQuad _titleRec;
  BoundingBox _bb;
  BoundingBox _textbb;
};

/*
 *
0------------1 -0.5, 0.5      0.5, 0.5
|\          /|
| 4--------5 |
| |  TXT   | |
| 9--------8 |-0.5 + _borderWith   0.5, 0.5-bo
| |  FREE  | |
| 7--------6 |
|/          \|
3------------2  -0.5,-0.5        0.5, -0.5
*/

PLUGIN(Window)

Window::Window(const tlp::PluginContext *context)
    : Glyph(context), _rectColor(205, 205, 205, 255), _textColor(205, 205, 205, 255),
      _borderWidth(0.02f), _border(TalipotBitmapDir + "halfCylinderTexture.png") {

  const float textheight = 0.05f;

  Coord v[10];
  v[0].set(-0.5, 0.5, 0);
  v[1].set(0.5, 0.5, 0);
  v[2].set(0.5, -0.5, 0);
  v[3].set(-0.5, -0.5, 0);
  v[4].set(-0.5 + _borderWidth, 0.5 - _borderWidth, 0);
  v[5].set(0.5 - _borderWidth, 0.5 - _borderWidth, 0);
  v[6].set(0.5 - _borderWidth, -0.5 + _borderWidth, 0);
  v[7].set(-0.5 + _borderWidth, -0.5 + _borderWidth, 0);
  v[8].set(0.5 - _borderWidth, 0.5 - _borderWidth - textheight * 2.f, 0);
  v[9].set(-0.5 + _borderWidth, 0.5 - _borderWidth - textheight * 2.f, 0);

  _bb.expand(v[6]);
  _bb.expand(v[9]);
  _bb.expand(v[8]);
  _bb.expand(v[7]);

  _border.addQuadEdge(v[0], v[4], _textColor);
  _border.addQuadEdge(v[1], v[5], _textColor);
  _border.addQuadEdge(v[2], v[6], _textColor);
  _border.addQuadEdge(v[3], v[7], _textColor);
  _border.addQuadEdge(v[0], v[4], _textColor);

  _titleRec.setPosition(0, v[4]);
  _titleRec.setPosition(1, v[5]);
  _titleRec.setPosition(2, v[8]);
  _titleRec.setPosition(3, v[9]);
  _titleRec.setTextureName(TalipotBitmapDir + "titlebarGradient.png");

  _textbb.expand(v[4]);
  _textbb.expand(v[8]);
  _textbb.expand(v[5]);
  _textbb.expand(v[9]);

  _center.setPosition(0, v[9]);
  _center.setPosition(1, v[8]);
  _center.setPosition(2, v[6]);
  _center.setPosition(3, v[7]);
}
//=====================================================
BoundingBox Window::getIncludeBoundingBox(node) {
  return _bb;
}
//=====================================================
BoundingBox Window::getTextBoundingBox(node) {
  return _textbb;
}
//=====================================================
void Window::draw(node n, float lod) {
  ColorProperty *color = glGraphInputData->colors();
  ColorProperty *colorBorder = glGraphInputData->borderColors();
  string textureName = glGraphInputData->textures()->getNodeValue(n);

  if (!textureName.empty()) {
    textureName = glGraphInputData->renderingParameters()->getTexturePath() + textureName;
  }

  _border.setColor(colorBorder->getNodeValue(n));
  _titleRec.setColor(colorBorder->getNodeValue(n));

  _center.setFillColor(color->getNodeValue(n));
  _center.setTextureName(textureName);
  _center.draw(lod, nullptr);
  _titleRec.draw(lod, nullptr);
  _border.draw(lod, nullptr);
}
//=====================================================
Coord Window::getAnchor(const Coord &v) const {
  float fmax = std::max(fabsf(v.x()), fabsf(v.y()));

  if (fmax > 0.0f) {
    return v * (0.5f / fmax);
  } else {
    return v;
  }
}

} // end of namespace tlp

#endif // WINDOWGLYPH_H
