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

#ifndef TALIPOT_GL_STAR_H
#define TALIPOT_GL_STAR_H

#include <talipot/Size.h>
#include <talipot/GlComplexPolygon.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief class to create a star
 */
class TLP_GL_SCOPE GlStar : public GlComplexPolygon {
  public:
    /**
     * @brief Constructor
     *
     * The outline is the border of the regular star
     *
     * The fill is inside the regular star
     */
    GlStar(const Coord &position, const Size &size, uint numberOfStarPoints,
           const Color &fillColor = Color(0, 0, 255), const Color &outlineColor = Color(0, 0, 0),
           bool outlined = true, const std::string &textureName = "", float outlineSize = 1.);
    /**
     * @brief Destructor
     */
    ~GlStar() override;

    /**
     * @brief Get the number of star points
     */
    uint getNumberOfStarPoints() const;

  protected:
    void computeStar();

    Coord position;
    Size size;
    uint numberOfStarPoints;
};
}
#endif // TALIPOT_GL_STAR_H
