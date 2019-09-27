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

#ifndef Tulip_GLYPHMANAGER_H
#define Tulip_GLYPHMANAGER_H

#ifndef DOXYGEN_NOTFOR_DEVEL

//#include <string>

#include <tulip/tulipconf.h>
#include <tulip/Glyph.h>
#include <tulip/MutableContainer.h>

namespace tlp {

class GlGraphInputData;
class Graph;

/**
 * Used to store Glyphs plugins
 */
class TLP_GL_SCOPE GlyphManager {

public:
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

  /**
   * Create the glyph list and store it in glyphs parameter
   */
  static void initGlyphList(Graph **graph, GlGraphInputData *glGraphInputData,
                            MutableContainer<Glyph *> &glyphs);
  /**
   * Clear the glyph list
   */
  static void clearGlyphList(Graph **graph, GlGraphInputData *glGraphInputData,
                             MutableContainer<Glyph *> &glyphs);
};
} // namespace tlp

#endif // DOXYGEN_NOTFOR_DEVEL

#endif // Tulip_GLYPHMANAGER_H
///@endcond
