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

#ifndef TALIPOT_PLANE_H
#define TALIPOT_PLANE_H

#include <talipot/Coord.h>

namespace tlp {
/** \brief Class used to describe a plane
 *
 * This class is used to represent a plane with it's coordinates.
 * it follows the basic plane equation aX + bY + cZ + d = 0
 */
class TLP_SCOPE Plane {
  public:
    float a, b, c, d;

    /**
     * Constructs a Plane
     * \attention a = b = c = d = 1.0f
     */
    Plane();

    /**
     * Constructs a plane with given coordinates
     */
    Plane(float a, float b, float c, float d);

    /**
     * Destructs a plane
     */
    ~Plane();

    /**
     * Static function used to determine the last coordinate of a point in order to have it on the
     * plane
     * It follows the equation : Z = (aX + bY + d) / c
     */
    static float planeValue(float a, float b, float c, float d, float x, float y);

    /**
     * Computes a quad from the plane
     */
    bool computePlane(Coord &p1, Coord &p2, Coord &p3, Coord &p4) const;

    /**
     * Given a point, this function returns aX + bY + cZ + d.
     * This is mainly used to determine if a point is above or under the plane.
     */
    float planePointValue(const Coord &pos) const;
};
}
#endif // TALIPOT_PLANE_H
