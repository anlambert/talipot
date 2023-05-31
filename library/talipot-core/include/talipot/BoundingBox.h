/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_BOUNDING_BOX_H
#define TALIPOT_BOUNDING_BOX_H

#include <cassert>
#include <talipot/Coord.h>
#include <talipot/config.h>

namespace tlp {
/**
 * \ingroup Structures
 * \brief This class represents the 3D bounding box of an object.
 * It is mostly used to determine whether or not two object are in a state of collision.
 *
 * It is defined by two 3d points, the first one (A) being the lowest point, the second (B) being
 the highest.
 * As a bounding box is a mathematical entity describing the lowest and highest points, whether
 these points are in the top-left corner or
 * lower-right corner depends on the axes we use.
 * Below is a crude ASCII-art description of the axes we use in our 3D world and the points where
 the min and max are thus positioned.
 * Through the rest of this class's documentation, it will be assumed that this is the convention.
 *
 *
 * @verbatim
      y
     |
     |
     |_____ x
    /
   /
  z

     _________ B
    /        /|
   /        / |
  /________/  |
  |        |  |
  |        |  |
  |        | /
  |________|/
  A

  @endverbatim
 *
 *
 */
struct TLP_SCOPE BoundingBox : public Array<Coord, 2> {

  /**
   * @brief Creates an invalid bounding box.
   * The minimum is (1, 1, 1) and the maximum is (-1, -1, -1).
   *
   **/
  BoundingBox();

  /**
   * @brief Creates a bounding box that must be valid.
   * Validity is checked in debug mode by an assert.
   *
   * @param min The lower left closest point of the box.
   * @param max The higher right most farther point of the box.
   * @param compute indicates whether the bounding box has to be computed with the min/max args
   *
   **/
  BoundingBox(const tlp::Coord &min, const tlp::Coord &max);

  /**
   * @brief Returns the geometrical center of the bounding box.
   * An assertion is raised in debug mode if the BoundingBox is not valid.
   *
   * @return The center of the bounding box :Coord
   **/
  Coord center() const {
    assert(isValid());
    return ((*this)[0] + (*this)[1]) / 2.f;
  }

  /**
   * @brief Returns the width of the bounding box
   * An assertion is raised in debug mode if the BoundingBox is not valid.
   *
   **/
  float width() const {
    assert(isValid());
    return ((*this)[1][0] - (*this)[0][0]);
  }

  /**
   * @brief Returns the height of the bounding box
   * An assertion is raised in debug mode if the bounding box is not valid.
   *
   **/
  float height() const {
    assert(isValid());
    return ((*this)[1][1] - (*this)[0][1]);
  }

  /**
   * @brief Returns the depth of the bounding box
   * An assertion is raised in debug mode if the bounding box is not valid.
   *
   **/
  float depth() const {
    assert(isValid());
    return ((*this)[1][2] - (*this)[0][2]);
  }

  /**
   * @brief Expands the bounding box to one containing the vector passed as parameter.
   * If the parameter is inside the bounding box, it remains unchanged.
   *
   * @param coord A point in the 3D space we want the bounding box to encompass.
   * @return void
   **/
  void expand(const tlp::Coord &coord);

  /**
   * @brief Expands the bounding box to one containing the bounding box passed as parameter.
   * If the parameter is inside the bounding box, it remains unchanged.
   *
   * @param bb A bounding box.
   * @return void
   **/
  void expand(const tlp::BoundingBox &bb);

  /**
   * @brief Translates the bounding box by the displacement given by the vector passed as parameter.
   *
   * @param vec The displacement vector in 3D space to translate this bounding box by.
   * @return void
   **/
  void translate(const tlp::Coord &vec);

  /**
   * @brief Scales the bounding box, i.e. multiplying its components by a vector passed as
   *parameter.
   *
   * @param factor The factor vector to scale this bounding box by.
   * @return void
   **/
  void scale(const tlp::Coord &factor);

  /**
   * @brief Checks whether the bounding box's lowest point is less than it's highest point.
   * "Less Than" means axis-by-axis comparison, i.e. x1 < x2 && y1 < y2 && z1 < z2.
   *
   * @return bool Whether this bounding box is valid.
   **/
  bool isValid() const;

  /**
   * @brief Checks if the given vector is inside the current bounding box. If the bounding box is
   *invalid the result is always false.
   * @param coord A point in the 3D space.
   * @return bool Whether coord is in the bounding box.
   **/
  bool contains(const tlp::Coord &coord) const;

  /**
   * @brief Checks if the given bounding box is inside the current bounding box. If one of the
   *bounding boxes is invalid the result is always false.
   * @param boundingBox The bounding box to test inclusion
   * @return bool Whether boundingBox is in the bounding box.
   **/
  bool contains(const tlp::BoundingBox &boundingBox) const;

  /**
   * @brief Checks if the given bounding box intersect the current one. If one of the bounding box
   *is invalid return false.
   * @param boundingBox The bounding box to compare with.
   * @return bool Whether the bounding boxes intersect.
   **/
  bool intersect(const tlp::BoundingBox &boundingBox) const;

  /**
   * @brief Checks if the bounding box intersects a given line segment. If the bounding box is
   *invalid the result is always false.
   * @param segStart the start point of the line segment on which to check intersection
   * @param segEnd the end point of the line segment on which to check intersection
   * @return bool Whether the line segment intersects the bounding box
   **/
  bool intersect(const Coord &segStart, const Coord &segEnd) const;

  /**
  * @brief The vector passed as parameter is modified to contain the 8 points of the bounding box.
  * The points are, in order :
  * 0: lower leftmost closest point (the bounding box's minimum)
  * 1: lower rightmost closest point
  * 2: highest rightmost closest point
  * 3: highest leftmost closest point
  * 4: lower rightmost farthest point
  * 5: lower rightmost farthest point
  * 6: highest rightmost farthest point
  * 7: highest leftmost farthest point
  *
  * Crude ASCII art again, sorry for your eyes.
  *
  * @verbatim

     6_________ 7
     /|       /|
    / |      / |
  3/__|_____/2 |
   |  |_____|__|
   |  /4    |  /5
   | /      | /
   |/_______|/
   0        1

   @endverbatim
  *
  * @param bb A vector in which to put the points of the bounding box.
  * @return void
  **/
  void getCompleteBB(Coord bb[8]) const;
};
}

#endif // TALIPOT_BOUNDING_BOX_H
