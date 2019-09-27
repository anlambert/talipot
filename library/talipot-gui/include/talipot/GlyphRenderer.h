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

#ifndef TALIPOT_GLYPH_RENDERER_H
#define TALIPOT_GLYPH_RENDERER_H

#include <QPixmap>

#include <talipot/config.h>

namespace tlp {

/**
 * @brief Generate Qt previews for Glyphs plug-ins.
 **/
class TLP_QT_SCOPE GlyphRenderer {
public:
  /**
   * @brief Get the preview for the glyph with the given Id.
   */
  static QPixmap render(int glyphId);
};

/**
 * @brief Generate Qt previews for edge extremities glyphs plug-ins.
 **/
class TLP_QT_SCOPE EdgeExtremityGlyphRenderer {
public:
  /**
   * @brief Get the preview for the edge extremity glyph with the given Id.
   */
  static QPixmap render(int glyphId);
};
} // namespace tlp
#endif // TALIPOT_GLYPH_RENDERER_H
///@endcond
