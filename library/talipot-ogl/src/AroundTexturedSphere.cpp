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
#include <talipot/GlGraphInputData.h>
#include <talipot/GlRect.h>
#include <talipot/GlSphere.h>
#include <talipot/AroundTexturedSphere.h>

using namespace std;
using namespace tlp;

BoundingBox AroundTexturedSphere::getIncludeBoundingBox(node) {
  return {{-0.35f, -0.35f, -0.35f}, {0.35f, 0.35f, 0.35f}};
}

void AroundTexturedSphere::drawGlyph(const Color &glyphColor, const Size &glyphSize,
                                     const string &texture, const string &texturePath,
                                     const string &aroundTextureFile, unsigned char alpha) {
  // draw a sphere
  static GlSphere sphere(Coord(0, 0, 0), 0.5);
  sphere.setColor(glyphColor);
  sphere.setTexture(texturePath + texture);
  sphere.draw(0, nullptr);

  // draw a texture in the screen plane around the sphere
  static GlRect rect(Coord(0, 0, 0), 2., 2, Color(0, 0, 0, 255), Color(0, 0, 0, 255));
  rect.setOutlineMode(false);

  rect.setTextureName(TalipotBitmapDir + aroundTextureFile);
  Color aroundColor = glyphColor;
  aroundColor.setA(alpha);
  rect.setFillColor(aroundColor);

  Glyph::drawRectInScreenPlane(rect, glyphSize, true);
}

void AroundTexturedSphere::draw(node n, float) {
  drawGlyph(glGraphInputData->getElementColor()->getNodeValue(n),
            glGraphInputData->getElementSize()->getNodeValue(n),
            glGraphInputData->getElementTexture()->getNodeValue(n),
            glGraphInputData->parameters->getTexturePath(), textureFile, alpha);
}
