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

#ifndef TALIPOT_GL_LINE_H
#define TALIPOT_GL_LINE_H

#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/config.h>
#include <talipot/GlEntity.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * \brief This is a general class to render an OpenGL laine
 * This class is use to render an OpenGl line
 */
class TLP_GL_SCOPE GlLine : public GlEntity {
  public:
    /**
     * Enpty constructor
     */
    GlLine() : width(1.0), factor(1), pattern(0) {}
    /**
     * Constructor with a list of coord and a list of color
     */
    GlLine(const std::vector<Coord> &points, const std::vector<Color> &colors);

    ~GlLine() override;

    /**
     * Render the line
     */
    void draw(float lod, Camera *camera) override;

    /**
     * Change number of points
     */
    virtual void resizePoints(const uint nbPoints);
    /**
     * Change number of colors
     */
    virtual void resizeColors(const uint nbColors);

    virtual const Coord &point(const uint i) const;
    /**
     * Return the coord of point with given id
     */
    virtual Coord &point(const uint i);
    /**
     * Add a new point with given coord and given color
     */
    virtual void addPoint(const Coord &point, const Color &color);

    /**
     * return the ith color used for draw the line
     */
    virtual const Color &color(const uint i) const;
    /**
     * return the ith color used for draw the line
     */
    virtual Color &color(const uint i);
    /**
     * Set line width
     */
    virtual void setLineWidth(float width);
    /**
     * Set the style of the line (documentation can be found in OpenGL manuel of function
     * glLineStipple)
     */
    virtual void setLineStipple(unsigned char factor, uint pattern);

    /**
     * Translate entity
     */
    void translate(const Coord &vec) override;

    /**
     * Function to export data in outString (in XML format)
     */
    void getXML(std::string &outString) override;

    /**
     * Function to set data with inString (in XML format)
     */
    void setWithXML(const std::string &inString, uint &currentPosition) override;

  protected:
    std::vector<Coord> _points;
    std::vector<Color> _colors;
    float width;
    unsigned char factor;
    uint pattern;
};
}

#endif // TALIPOT_GL_LINE_H
