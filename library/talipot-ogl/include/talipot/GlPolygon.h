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

#ifndef TALIPOT_GL_POLYGON_H
#define TALIPOT_GL_POLYGON_H

#include <vector>

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/config.h>
#include <talipot/GlAbstractPolygon.h>

namespace tlp {
/**
 * @ingroup OpenGL
 * @brief Class to create a polygon GlEntity
 *
 */
class TLP_GL_SCOPE GlPolygon : public GlAbstractPolygon {
public:
  /**
   * @brief Constructor where specify if the polygon is filled, is outlines the texture name and the
   * outline size
   */
  GlPolygon(const bool filled = true, const bool outlined = true,
            const std::string &textureName = "", const float outlineSize = 1);
  /**
   * @brief Constructor with a vector of point, a vector of fill color, a vector of outline color
   * and if the polygon is filled, is outlined and the outline size
   */
  GlPolygon(const std::vector<Coord> &points, const std::vector<Color> &fillColors,
            const std::vector<Color> &outlineColors, const bool filled, const bool outlined,
            const std::string &textureName = "", const float outlineSize = 1);
  /**
   * @brief Constructor with a number of point, a number of fill color, a number of outline color
   * and if the polygon is filled, outlined and the outline size
   */
  GlPolygon(const uint nbPoints, const uint nbFillColors, const uint nbOutlineColors,
            const bool filled = true, const bool outlined = true,
            const std::string &textureName = "", const float outlineSize = 1);
  ~GlPolygon() override;

  /**
   * @brief Change number of point of the polygon
   */
  virtual void resizePoints(const uint nbPoints);
  /**
   * @brief Change number of colors of the polygon
   */
  virtual void resizeColors(const uint nbColors);

  /**
   * @brief return the ith point
   */
  virtual const Coord &point(const uint i) const;
  /**
   * @brief return the ith point
   */
  virtual Coord &point(const uint i);
};
}
#endif // TALIPOT_GL_POLYGON_H
