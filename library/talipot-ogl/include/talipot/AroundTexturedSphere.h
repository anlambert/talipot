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

using namespace std;
using namespace tlp;

namespace tlp {

struct BoundingBox;

/// A 3D glyph.
/**
 * This glyph draws a sphere using the "viewColor" node property value.
 * and apply a texture around it
 */
class TLP_GL_SCOPE AroundTexturedSphere : public NoShaderGlyph {
  const std::string textureFile;
  const unsigned char alpha;

public:
  AroundTexturedSphere(const tlp::PluginContext *context = nullptr,
                       const std::string &aroundTextureFile = "", unsigned char alphaVal = 255)
      : NoShaderGlyph(context), textureFile(aroundTextureFile), alpha(alphaVal) {}
  BoundingBox getIncludeBoundingBox(node)
  override;
  void draw(node n, float) override;
  static void drawGlyph(const Color &glyphColor, const Size &glyphSize, const string &texture,
                        const string &texturePath, const string &aroundTextureFile,
                        unsigned char alpha = 255);
};
} // end of namespace tlp
