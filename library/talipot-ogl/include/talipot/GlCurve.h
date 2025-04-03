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

#ifndef TALIPOT_GL_CURVE_H
#define TALIPOT_GL_CURVE_H

#include <talipot/GlEntity.h>

namespace tlp {
/** \brief This class is use to display an OpenGl curve
 *
 */
class TLP_GL_SCOPE GlCurve : public GlEntity {
  public:
    /**
     * Basic constructor with vector of coord, begin/end color and begin/end size
     */
    GlCurve(const std::vector<tlp::Coord> &points, const Color &beginFColor, const Color &endFColor,
            const float &beginSize = 0., const float &endSize = 0.);

    /**
     * Basic constructor with number of points
     */
    GlCurve(const uint nbPoints = 3u);
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
    virtual void resizePoints(const uint nbPoints);

    /**
     * Return the ith coord
     */
    virtual const tlp::Coord &point(const uint i) const;
    /**
     * Return the ith coord
     */
    virtual tlp::Coord &point(const uint i);

    /**
     * Translate entity
     */
    void translate(const Coord &move) override;

    /**
     * Function to export data in outString (in XML format)
     */
    void getXML(std::string &outString) override;

    /**
     * Function to set data with inString (in XML format)
     */
    void setWithXML(const std::string &inString, uint &currentPosition) override;

  protected:
    std::vector<tlp::Coord> _points;
    Color _beginFillColor;
    Color _endFillColor;
    float _beginSize;
    float _endSize;
    std::string texture;
};
}
#endif // TALIPOT_GL_CURVE_H
