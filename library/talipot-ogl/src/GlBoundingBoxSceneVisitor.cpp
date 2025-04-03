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

#include <talipot/GlBoundingBoxSceneVisitor.h>

#include <talipot/GlNode.h>
#include <talipot/GlEdge.h>

using namespace std;

namespace tlp {

GlBoundingBoxSceneVisitor::GlBoundingBoxSceneVisitor(GlGraphInputData *inputData)
    : inputData(inputData) {
    threadSafe = true;
    bbs.resize(ThreadManager::getNumberOfThreads());
}

BoundingBox GlBoundingBoxSceneVisitor::getBoundingBox() {
    BoundingBox bb = bbs[0];

    for (uint i = 1; i < bbs.size(); ++i) {
        bb.expand(bbs[i]);
    }
    return bb;
}

void GlBoundingBoxSceneVisitor::visit(GlEntity *entity) {
    if (entity->isVisible()) {
        BoundingBox bb = entity->getBoundingBox();

        if (bb.isValid()) {
            auto ti = ThreadManager::getThreadNumber();
            bbs[ti].expand(bb);
        }
    }
}

void GlBoundingBoxSceneVisitor::visit(GlNode *glNode) {
    BoundingBox bb = glNode->getBoundingBox(inputData);
    auto ti = ThreadManager::getThreadNumber();
    bbs[ti].expand(bb);
}

void GlBoundingBoxSceneVisitor::visit(GlEdge *glEdge) {
    BoundingBox bb = glEdge->getBoundingBox(inputData);
    auto ti = ThreadManager::getThreadNumber();

    bbs[ti].expand(bb);
}
}
