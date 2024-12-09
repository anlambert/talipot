/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GLYPH_MANAGER_H
#define TALIPOT_GLYPH_MANAGER_H

#include <memory>

#include <talipot/config.h>
#include <talipot/Glyph.h>

namespace tlp {

class GlGraphInputData;
class Graph;

/**
 * Used to store Glyphs plugins
 */
class TLP_GL_SCOPE GlyphManager {

public:
  GlyphManager(GlGraphInputData *inputData);
  ~GlyphManager();

  Glyph *getGlyph(int id) const;

  /**
   * Return the name of glyph with given id
   */
  static std::string glyphName(int id);
  /**
   * Return the id if glyph with given name
   */
  static int glyphId(const std::string &name, bool warnIfNotFound = true);
  /**
   * Load glyphs plugins
   */
  static void loadGlyphPlugins();

private:
  flat_hash_map<int, Glyph *> _glyphs;
  std::unique_ptr<Glyph> _defaultGlyph;
};
}

#endif // TALIPOT_GLYPH_MANAGER_H
