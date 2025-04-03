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

#ifndef TALIPOT_GL_CIRCLE_H
#define TALIPOT_GL_CIRCLE_H

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/GlRegularPolygon.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * @brief Class used to render circles as GlEntity.
 *
 * If you want a circle : use this class
 */
class TLP_GL_SCOPE GlCircle : public GlRegularPolygon {
  public:
    /**
     * @brief Constructor
     *
     * @param startAngle if start angle is equal to 0, the circle construction begin with the top
     * center point
     * @param segment number of triangle used to render the circle
     */
    GlCircle(const Coord &center = Coord(0, 0, 0), float radius = 1.,
             const Color &outlineColor = Color(255, 0, 0, 255),
             const Color &fillColor = Color(0, 0, 255, 255), bool filled = false,
             bool outlined = true, float startAngle = 0.0, uint segments = 10);

    /**
     * @brief Write-acess Accessor to the center, the radius and the startAngle of the circle.
     */
    void set(const Coord &center, float radius, float startAngle);

    /**
     * @brief Function to export data in XML
     */
    void getXML(std::string &outString) override;
};
}

#endif // TALIPOT_GL_CIRCLE_H
