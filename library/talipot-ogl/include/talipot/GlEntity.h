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

#ifndef TALIPOT_GL_SIMPLE_ENTITY_H
#define TALIPOT_GL_SIMPLE_ENTITY_H

#include <vector>

#include <talipot/Coord.h>
#include <talipot/BoundingBox.h>

namespace tlp {

class GlComposite;
class Camera;
class GlSceneVisitor;

/**
 * @ingroup OpenGL
 * @brief Base class for all Talipot OpenGL entities
 *
 * Other Talipot entities inherit for this class.
 *
 * You don't have to create a GlEntity, you have to use GlLine, GlRect or GlSphere for example
 * @see Gl2DRect
 * @see GlPolygon
 * @see GlAxis
 * @see GlBezierCurve
 * @see GlBox
 * @see GlCatmullRomCurve
 * @see GlCircle
 * @see GlComplexPolygon
 * @see GlGrid
 * @see GlHexagon
 * @see GlLabel
 * @see GlSphere
 * @see GlPentagon
 * @see GlTriangle
 * @see GlOpenUniformCubicBSpline
 *
 * To GlEntity manipulation :
 * @see GlLayer
 * @see GlScene
 */
class TLP_GL_SCOPE GlEntity {

  public:
    /**
     * @brief Constructor
     */
    GlEntity() : visible(true), stencil(0xFFFF) {}

    /**
     * @brief Destructor
     */
    virtual ~GlEntity();

    /**
     * @brief Set if entity is visible
     */
    virtual void setVisible(bool visible);
    /**
     * @brief Return if entity is visible
     */
    bool isVisible() const {
        return visible;
    }
    /**
     * @brief Set stencil number of the entity
     *
     * Stencil is an OpenGl system to ensure that other entity can't be displayed above this entity;
     * it's a "guaranteed visibility" system.
     * A small number causes a guaranteed visibility
     * Default value in Talipot is 0xFFFF (greater integer)
     * And when we have stencil on entity value is 0x2
     */
    virtual void setStencil(int stencil) {
        this->stencil = stencil;
    }
    /**
     * @brief Return stencil number of entity
     *
     * @see setStencil()
     */
    int getStencil() const {
        return stencil;
    }

    /**
     * @brief Draw function
     *
     * @warning You don't have to call this function, the Talipot OpenGL engine call it.
     */
    virtual void draw(float lod, Camera *camera) = 0;

    /**
     * @brief Return the entity boundingbox
     *
     * @warning You don't have to call this function, the Talipot OpenGL engine call it.
     */
    virtual BoundingBox getBoundingBox() {
        return boundingBox;
    }

    /**
     * @brief Save the entity in outString (in XML format)
     *
     * @warning You don't have to call this function, the Talipot OpenGL engine call it.
     */
    virtual void getXML(std::string &outString) = 0;

    /**
     * @brief Load entity with inString (in XML format)
     *
     * @warning You don't have to call this function, the Talipot OpenGL engine call it.
     */
    virtual void setWithXML(const std::string &inString, uint &currentPosition) = 0;

    /**
     * @brief Accept visitor function
     */
    virtual void acceptVisitor(GlSceneVisitor *visitor);

    /**
     * Add a parent to this entity
     */
    void addParent(GlComposite *composite);

    /**
     * remove a parent to this entity
     */
    void removeParent(GlComposite *composite);

    /**
     * virtual function : Translate entity of vector translation
     */
    virtual void translate(const Coord &) {}

    GlComposite *getParent() const {
        if (parents.empty()) {
            return nullptr;
        }

        return parents[0];
    }

  protected:
    bool visible;
    int stencil;

    BoundingBox boundingBox;

    std::vector<GlComposite *> parents;
};
}

#endif // TALIPOT_GL_SIMPLE_ENTITY_H
