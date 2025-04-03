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

#ifndef TALIPOT_GL_SCENE_VISITOR_H
#define TALIPOT_GL_SCENE_VISITOR_H

#include <talipot/config.h>

namespace tlp {

class GlEntity;
class GlNode;
class GlEdge;
class GlLayer;

class TLP_GL_SCOPE GlSceneVisitor {

  public:
    GlSceneVisitor() : threadSafe(false) {}
    virtual ~GlSceneVisitor() = default;

    virtual void visit(GlEntity *) {}
    virtual void visit(GlNode *) {}
    virtual void visit(GlEdge *) {}
    virtual void visit(GlLayer *) {}
    virtual void endOfVisit() {}
    virtual void reserveMemoryForGraphElts(uint /*nbNodes*/, uint /*nbEdges*/) {}

    bool isThreadSafe() const {
        return threadSafe;
    }

  protected:
    bool threadSafe;
};
}

#endif // TALIPOT_GL_SCENE_VISITOR_H
