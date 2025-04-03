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

#ifndef TALIPOT_GL_REGULAR_POLYGON_H
#define TALIPOT_GL_REGULAR_POLYGON_H

#include <talipot/GlPolygon.h>
#include <talipot/Size.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief class to create a regular polygon
 */
class TLP_GL_SCOPE GlRegularPolygon : public GlPolygon {
  public:
    /**
     * @brief Constructor
     *
     * The outline is the border of the regular polygon
     *
     * The fill is inside the regular polygon
     */
    GlRegularPolygon(const Coord &position, const Size &size, uint numberOfSides,
                     const Color &outlineColor = Color(255, 0, 0, 255),
                     const Color &fillColor = Color(0, 0, 255, 255), bool filled = true,
                     bool outlined = true, const std::string &textureName = "",
                     float outlineSize = 1.);
    /**
     * @brief Destructor
     */
    ~GlRegularPolygon() override;

    /**
     * @brief Set the start angle in radian
     *
     * If the start angle is 0 : the regular polygon begin at the top middle point
     */
    void setStartAngle(float angle);

    /**
     * @brief Get the number of Sides
     */
    uint getNumberOfSides() const;

    /**
     * @brief Set the number of sides (use setNumberOfSides)
     */
    void resizePoints(const uint number) override;

    /**
     * @brief Set the number of sides
     */
    void setNumberOfSides(uint number);

  protected:
    void computePolygon();

    Coord position;
    Size size;
    uint numberOfSides;
    float startAngle;
};
}
#endif // TALIPOT_GL_REGULAR_POLYGON_H
