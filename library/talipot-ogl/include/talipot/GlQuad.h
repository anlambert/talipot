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

#ifndef TALIPOT_GL_QUAD_H
#define TALIPOT_GL_QUAD_H

#include <talipot/Coord.h>
#include <talipot/Color.h>

#include <talipot/GlPolygon.h>

namespace tlp {
/**
 * @ingroup OpenGL
 * \brief Utility class to render a quadrilateral .
 *
 * This is an utility class class for rendering a quadrilateral.
 */
class TLP_GL_SCOPE GlQuad : public GlPolygon {

  public:
    /**
     * Constructor
     */
    GlQuad();

    /**
     * Constructor
     *
     * Initializes a quadrilateral with 4 points winding in clockwise order and a single color.
     *
     * \param p1 the first quad vertex
     * \param p2 the second quad vertex
     * \param p3 the third quad vertex
     * \param p4 the fourth quad vertex
     * \param color A single color indicating the color for every point of the quadrilateral.
     */
    GlQuad(const Coord &p1, const Coord &p2, const Coord &p3, const Coord &p4, const Color &color);

    /* Constructor
     *
     * Initializes a quadrilateral with 4 points winding in clockwise order and a color attached to
     * each point.
     *
     * \param p1 the first quad vertex
     * \param p2 the second quad vertex
     * \param p3 the third quad vertex
     * \param p4 the fourth quad vertex
     * \param c1 the color of the first quad vertex
     * \param c2 the color of the second quad vertex
     * \param c3 the color of the third quad vertex
     * \param c4 the color of the fourth quad vertex
     */
    GlQuad(const Coord &p1, const Coord &p2, const Coord &p3, const Coord &p4, const Color &c1,
           const Color &c2, const Color &c3, const Color &c4);

    /**
     * Destructor
     */
    ~GlQuad() override;

    /**
     * Accessor in writing to the position.
     * \param idPosition Indicates which point we want to move.
     */
    void setPosition(const int idPosition, const Coord &position);

    /**
     * Accessor in writing to the color.
     * \param idColor Indicates which point we want to colorize.
     */
    void setColor(const int idColor, const Color &color);

    /**
     * Accessor in writing to the basic color of GlAugmentedDisplay
     */
    void setColor(const Color &color);

    /**
     * Accessor in reading to the position.
     * \param idPosition Indicates which point we want to get the position.
     */
    const Coord &getPosition(const int idPosition) const;

    /**
     * Accessor in reading to the color.
     * \param idColor Indicates which point we want to get the color.
     */
    const Color &getColor(const int idColor) const;

    /**
     * Function to export data in outString (in XML format)
     */
    void getXML(std::string &outString) override;

    /**
     * Function to set data with inString (in XML format)
     */
    void setWithXML(const std::string &inString, uint &currentPosition) override;
};
}
#endif // TALIPOT_GL_QUAD_H
