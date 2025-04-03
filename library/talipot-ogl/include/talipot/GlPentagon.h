/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_PENTAGON_H
#define TALIPOT_GL_PENTAGON_H

#include <talipot/GlRegularPolygon.h>

namespace tlp {
/**
 * @ingroup OpenGL
 * @brief class to create an pentagon
 */
class TLP_GL_SCOPE GlPentagon : public GlRegularPolygon {
  public:
    /**
     * @brief Constructor
     */
    GlPentagon(const Coord &position, const Size &size,
               const Color &outlineColor = Color(255, 0, 0, 255),
               const Color &fillColor = Color(0, 0, 255, 255), bool filled = true,
               bool outlined = true, const std::string &textureName = "", float outlineSize = 1.);
    /**
     * @brief Default empty destructor
     *
     * @warning Don't use this constructor
     */
    ~GlPentagon() override;
};
}
#endif // TALIPOT_GL_PENTAGON_H
