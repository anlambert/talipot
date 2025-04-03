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

#ifndef TALIPOT_GL_RECT_H
#define TALIPOT_GL_RECT_H

#include <talipot/GlPolygon.h>
#include <talipot/Size.h>

namespace tlp {

/**
 * @ingroup OpenGL
 * \brief This is a general class for the rendering of 2D rectangles.
 *
 * This class is used as a 2D HUD for the rendering of 2D rectangles.
 */

class TLP_GL_SCOPE GlRect : public GlPolygon {
  protected:
  public:
    /**
     * Constructor : with topLeft/bottomRight coords, topLeft/bottomRight colors and if GlRect is
     * filled/outlined
     */
    GlRect(const Coord &topLeftPos, const Coord &bottomRightPos, const Color &topLeftCol,
           const Color &bottomRightCol, bool filled = true, bool outlined = false);

    /**
     * Constructor : with center coords and size, fill color and outline color
     */
    GlRect(const Coord &center, const float width, const float height, const Color &fillColor,
           const Color &outlineColor);

    /**
     * Constructor : with GlRect is filled/outlined
     */
    GlRect(bool filled = true, bool outlined = false);

    /**
     * Destructor
     */
    ~GlRect() override;

    /**
     * Accessor in reading to the Top Left Corner of the rectangle
     */
    virtual Coord getCenter();

    /**
     * Set the center and the size of the GlRect
     */
    void setCenterAndSize(const Coord &center, const Size &size);

    /**
     * Accessor in reading to the Top Left Corner of the rectangle
     */
    virtual Coord getTopLeftPos();

    /**
     * Accessor in reading to the Bottom Right Corner of the rectangle
     */
    virtual Coord getBottomRightPos();

    /**
     * Accessor in reading to the Top Left Corner Color of the rectangle
     */
    virtual Color getTopLeftColor();

    /**
     * Accessor in reading to the Bottom Right Corner Color of the Rectangle
     */
    virtual Color getBottomRightColor();

    /**
     * Accessor in writing to the Top Left Corner of the rectangle
     */
    virtual void setTopLeftPos(const Coord &topLeftPos);

    /**
     * Accessor in writing to the Bottom Right Corner Color of the rectangle
     */
    virtual void setBottomRightPos(const Coord &bottomRightPos);

    /**
     * Accessor in writing to the Top Left Corner of the rectangle
     */
    virtual void setTopLeftColor(const Color &topLeftCol);

    /**
     * Accessor in writing to the Bottom Right Corner Color of the rectangle
     */
    virtual void setBottomRightColor(const Color &bottomRightCol);

    /**
     * Is the point in the rectangle ?
     */
    bool inRect(double x, double y);

    void draw(float lod, Camera *camera) override;
};
}
#endif // TALIPOT_GL_RECT_H
