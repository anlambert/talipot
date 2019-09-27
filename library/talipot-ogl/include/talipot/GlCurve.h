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

#ifndef TALIPOT_GL_CURVE_H
#define TALIPOT_GL_CURVE_H

#include <talipot/GlSimpleEntity.h>

namespace tlp {
/** \brief This class is use to display an OpenGl curve
 *
 */
class TLP_GL_SCOPE GlCurve : public GlSimpleEntity {
public:
  /**
   * Basic constructor with vector of coord, begin/end color and begin/end size
   */
  GlCurve(const std::vector<tlp::Coord> &points, const Color &beginFColor, const Color &endFColor,
          const float &beginSize = 0., const float &endSize = 0.);

  /**
   * Basic constructor with number of points
   */
  GlCurve(const unsigned int nbPoints = 3u);
  ~GlCurve() override;

  /**
   * Draw the curve
   */
  void draw(float lod, Camera *camera) override;

  /**
   * Set the texture of the curve (if you want texture)
   */
  void setTexture(const std::string &texture);

  /**
   * Change the number of points
   */
  virtual void resizePoints(const unsigned int nbPoints);

  /**
   * Return the ith coord
   */
  virtual const tlp::Coord &point(const unsigned int i) const;
  /**
   * Return the ith coord
   */
  virtual tlp::Coord &point(const unsigned int i);

  /**
   * Translate entity
   */
  void translate(const Coord &mouvement) override;

  /**
   * Function to export data in outString (in XML format)
   */
  void getXML(std::string &outString) override;

  /**
   * Function to set data with inString (in XML format)
   */
  void setWithXML(const std::string &inString, unsigned int &currentPosition) override;

protected:
  std::vector<tlp::Coord> _points;
  Color _beginFillColor;
  Color _endFillColor;
  float _beginSize;
  float _endSize;
  std::string texture;
};
} // namespace tlp
#endif // TALIPOT_GL_CURVE_H
///@endcond
