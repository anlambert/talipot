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

#ifndef TALIPOT_GL_2D_RECT_H
#define TALIPOT_GL_2D_RECT_H

#include <string>

#include <talipot/GlRect.h>

namespace tlp {
/**
 * @ingroup OpenGL
 *
 * @brief Create a 2D rectangle
 *
 * This rect add screen percentage system (for example you can create a rectangle that start on
 * (10%,10%) and finish on (90%90%)
 * If you want a simple 2D rectangle with normal coordinates use GlRect class
 * @see GlRect
 */
class TLP_GL_SCOPE Gl2DRect : public GlRect {

  public:
    /**
     * @brief Don't use this constructor
     */
    Gl2DRect();

    /**
     * @brief Constructor
     *
     * @param top coordinate
     * @param bottom coordinate
     * @param left coordinate
     * @param right coordinate
     * @param textureName path to a texture
     * @param inPercent if true : top/bottom/left/right is used as in percent parameters
     */
    Gl2DRect(float top, float bottom, float left, float right, const std::string &textureName,
             bool inPercent = false);

    /**
     * @brief Constructor
     *
     * @param top coordinate
     * @param bottom coordinate
     * @param left coordinate
     * @param right coordinate
     * @param textureName path to a texture
     * @param xInv if true : use viewportX - left and viewportX - right
     * @param yInv if true : use viewportY - top and viewportY - bottom
     */
    Gl2DRect(float bottom, float left, float height, float width, const std::string &textureName,
             bool xInv, bool yInv);

    /**
     * @brief Destructor
     */
    ~Gl2DRect() override = default;

    BoundingBox getBoundingBox() override;

    void draw(float lod, Camera *camera) override;

    void translate(const Coord &move) override;

    /**
     * @brief Set texture
     */
    virtual void setTexture(const std::string &name);

    /**
     * @brief Get texture
     */
    virtual std::string getTexture();

    /**
     * @brief Set coordinates of rectangle
     */
    virtual void setCoordinates(float bottom, float left, float width, float height);

    void getXML(std::string &outString) override;

    void setWithXML(const std::string &inString, uint &currentPosition) override;

  protected:
    float top;
    float bottom;
    float left;
    float right;
    bool inPercent;
    bool xInv;
    bool yInv;
};
}
#endif // TALIPOT_GL_2D_RECT_H
