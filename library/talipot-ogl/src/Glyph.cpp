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

using namespace tlp;
using namespace std;

Glyph::Glyph(const tlp::PluginContext *context) : glGraphInputData(nullptr) {
  if (context != nullptr) {
    const auto *glyphContext = static_cast<const GlyphContext *>(context);
    glGraphInputData = glyphContext->glGraphInputData;
  }
}
//=============================================
Glyph::~Glyph() = default;

//=============================================
void Glyph::getIncludeBoundingBox(BoundingBox &boundingBox, node) {
  boundingBox[0] = Coord(-0.5, -0.5, -0.5);
  boundingBox[1] = Coord(0.5, 0.5, 0.5);
}

//=============================================
void Glyph::getTextBoundingBox(BoundingBox &boundingBox, node n) {
  getIncludeBoundingBox(boundingBox, n);
}

//=============================================
Coord Glyph::getAnchor(const Coord &nodeCenter, const Coord &from, const Size &scale,
                       const double zRotation) const {
  Coord anchor = from - nodeCenter;

  if (anchor.getX() == 0.0f && anchor.getY() == 0.0f) {
    return nodeCenter;
  }

  if (scale.getW() == 0.0f || scale.getH() == 0.0f) {
    return nodeCenter;
  }

  if (zRotation != 0) {
    // unrotate
    Coord saveAnchor = anchor;
    double zRot = -2.0 * M_PI * zRotation / 360.0;
    anchor[0] = saveAnchor[0] * cos(zRot) - saveAnchor[1] * sin(zRot);
    anchor[1] = saveAnchor[0] * sin(zRot) + saveAnchor[1] * cos(zRot);
  }

  // unscale
  anchor.setX(anchor.getX() / scale.getW());
  anchor.setY(anchor.getY() / scale.getH());

  if (scale.getD() != 0.0f) {
    anchor.setZ(anchor.getZ() / scale.getD());
  } else {
    anchor.setZ(0.0f);
  }

  anchor = getAnchor(anchor);

  // rescale
  anchor.setX(anchor.getX() * scale.getW());
  anchor.setY(anchor.getY() * scale.getH());
  anchor.setZ(anchor.getZ() * scale.getD());

  if (zRotation != 0) {
    // rerotate
    Coord saveAnchor = anchor;
    double zRot = 2.0 * M_PI * zRotation / 360.0;
    anchor[0] = saveAnchor[0] * cos(zRot) - saveAnchor[1] * sin(zRot);
    anchor[1] = saveAnchor[0] * sin(zRot) + saveAnchor[1] * cos(zRot);
  }

  return nodeCenter + anchor;
}
//=================================================================
Coord Glyph::getAnchor(const Coord &v) const {
  return v * (0.5f / v.norm());
}
//=================================================================
void Glyph::drawRectInScreenPlane(GlRect &rect, const Size &size, bool disableMasks) {
  // setup rect orientation to ensure it is drawn is the screen plane
  float mdlM[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mdlM);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  mdlM[0] = size.getW();
  mdlM[5] = size.getH();
  mdlM[10] = size.getD();
  mdlM[1] = mdlM[2] = 0.0f;
  mdlM[4] = mdlM[6] = 0.0f;
  mdlM[8] = mdlM[9] = 0.0f;
  glLoadMatrixf(mdlM);
  if (disableMasks) {
    glStencilMask(0x00);
    glDepthMask(GL_FALSE);
  }
  rect.draw(0, nullptr);
  if (disableMasks) {
    glStencilMask(0xFF);
    glDepthMask(GL_TRUE);
  }
  glPopMatrix();
}
