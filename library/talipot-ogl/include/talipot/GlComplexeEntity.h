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

#ifndef TALIPOT_GL_COMPLEXE_ENTITY_H
#define TALIPOT_GL_COMPLEXE_ENTITY_H

#include <talipot/BoundingBox.h>
#include <talipot/GlEntity.h>

namespace tlp {

struct OcclusionTest;
class TextRenderer;
class GlGraphInputData;
class Camera;

/** \brief Virtual class used by GlNode, GlMetaNode and GlEdge
 *
 * This class is a virtual class used by GlNode, GlMetaNode and GlEdge
 * Define draw function for entities who need GlGraphInputData variable
 * Define drawLabel function
 */
class TLP_GL_SCOPE GlComplexeEntity : public GlEntity {

  public:
    /**
     * return the bounding box of this entity
     */
    virtual BoundingBox getBoundingBox(const GlGraphInputData *data) = 0;

    /**
     * Draw this entity with GlGraphInputData and Camera
     */
    virtual void draw(float lod, const GlGraphInputData *data, Camera *camera) = 0;

    /**
     * Draw label of this entity
     */
    virtual void drawLabel(OcclusionTest *test, const GlGraphInputData *data) = 0;
};
}

#endif // TALIPOT_GL_COMPLEXE_ENTITY_H
