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

///@cond DOXYGEN_HIDDEN

#ifndef TALIPOT_EDGE_EXTREMITY_GLYPH_H
#define TALIPOT_EDGE_EXTREMITY_GLYPH_H

#include <talipot/Edge.h>
#include <talipot/Size.h>
#include <talipot/Coord.h>
#include <talipot/Matrix.h>
#include <talipot/Plugin.h>

namespace tlp {

class Color;

static const std::string EEGLYPH_CATEGORY = "Edge extremity";

typedef Matrix<float, 4> MatrixGL;
class GlGraphInputData;

class TLP_GL_SCOPE EdgeExtremityGlyph : public Plugin {
public:
  std::string category() const override {
    return EEGLYPH_CATEGORY;
  }
  std::string icon() const override {
    return ":/talipot/gui/icons/32/plugin_glyph.png";
  }

  EdgeExtremityGlyph(const tlp::PluginContext *context);
  ~EdgeExtremityGlyph() override;
  virtual void draw(edge e, node n, const Color &glyphColor, const Color &borderColor,
                    float lod) = 0;
  void get2DTransformationMatrix(const Coord &src, const Coord &dest, const Size &glyphSize,
                                 MatrixGL &transformationMatrix, MatrixGL &scalingMatrix);
  void get3DTransformationMatrix(const Coord &src, const Coord &dest, const Size &glyphSize,
                                 MatrixGL &transformationMatrix, MatrixGL &scalingMatrix);

protected:
  GlGraphInputData *edgeExtGlGraphInputData;
};
} // namespace tlp

#endif // TALIPOT_EDGE_EXTREMITY_GLYPH_H
///@endcond
