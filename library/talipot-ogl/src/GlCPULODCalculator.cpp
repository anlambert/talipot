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

#include <talipot/GlCPULODCalculator.h>

using namespace std;

namespace tlp {

GlCPULODCalculator::GlCPULODCalculator() : computeEdgesLOD(true) {
    threadSafe = true;
    bbs.resize(ThreadManager::getNumberOfThreads());
}

GlCPULODCalculator::~GlCPULODCalculator() = default;

void GlCPULODCalculator::beginNewCamera(Camera *camera) {
    // add a new layerLODUnit and set camera pointer
    layersLODVector.push_back(LayerLODUnit(camera));
    currentLayerLODUnit = &layersLODVector.back();
}

void GlCPULODCalculator::addEntityBoundingBox(GlEntity *entity, const BoundingBox &bb) {
    assert(bb.isValid());

    // This code is here to prevent adding entities in percentage
    //  If you look at the Gl2DRect, you can see an option inPercent,
    //   if this entity is inPercent we cannot compute bounding box, so we create the biggest
    //   possible bounding box and here we don't add this false bounding box to the scene bounding
    //   box
    //   TODO: See if we can change the bounding box compute in Gl2DRect
    if (bb[0][0] != numeric_limits<float>::min()) {
        auto ti = ThreadManager::getThreadNumber();
        bbs[ti].expand(bb);
    }

    currentLayerLODUnit->entitiesLODVector.push_back(EntityLODUnit(entity, bb));
}
void GlCPULODCalculator::addNodeBoundingBox(Graph *graph, node n, const BoundingBox &bb) {
    auto ti = ThreadManager::getThreadNumber();
    bbs[ti].expand(bb);
    currentLayerLODUnit->nodesLODVector[graph->nodePos(n)].init(n.id, bb);
}
void GlCPULODCalculator::addEdgeBoundingBox(Graph *graph, edge e, const BoundingBox &bb) {
    auto ti = ThreadManager::getThreadNumber();
    bbs[ti].expand(bb);
    currentLayerLODUnit->edgesLODVector[graph->edgePos(e)].init(e.id, bb);
}

BoundingBox GlCPULODCalculator::getSceneBoundingBox() {
    BoundingBox bb = bbs[0];

    for (uint i = 1; i < bbs.size(); ++i) {
        bb.expand(bbs[i]);
    }
    return bb;
}

void GlCPULODCalculator::reserveMemoryForGraphElts(uint nbNodes, uint nbEdges) {
    currentLayerLODUnit->nodesLODVector.resize(nbNodes);
    currentLayerLODUnit->edgesLODVector.resize(nbEdges);
}

void GlCPULODCalculator::compute(const Vec4i &globalViewport, const Vec4i &currentViewport) {

    for (auto &it : layersLODVector) {
        Camera *camera = it.camera;

        const MatrixGL &transformMatrix = camera->getTransformMatrix(globalViewport);

        if (camera->is3D()) {
            Coord eye = camera->getEyes() +
                        (camera->getEyes() - camera->getCenter()) / float(camera->getZoomFactor());
            computeFor3DCamera(&it, eye, transformMatrix, globalViewport, currentViewport);
        } else {
            computeFor2DCamera(&it, globalViewport, currentViewport);
        }

        glMatrixMode(GL_MODELVIEW);
    }
}

void GlCPULODCalculator::computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye,
                                            const MatrixGL &transformMatrix,
                                            const Vec4i &globalViewport,
                                            const Vec4i &currentViewport) {

    uint nb = 0;
    if ((renderingEntitiesFlag & RenderingEntities) != 0) {
        nb = layerLODUnit->entitiesLODVector.size();
        TLP_PARALLEL_MAP_INDICES(nb, [&](uint i) {
            layerLODUnit->entitiesLODVector[i].lod =
                calculateAABBSize(layerLODUnit->entitiesLODVector[i].boundingBox, eye,
                                  transformMatrix, globalViewport, currentViewport);
        });
    }

    if ((renderingEntitiesFlag & RenderingNodes) != 0) {
        nb = layerLODUnit->nodesLODVector.size();
        TLP_PARALLEL_MAP_INDICES(nb, [&](uint i) {
            layerLODUnit->nodesLODVector[i].lod =
                calculateAABBSize(layerLODUnit->nodesLODVector[i].boundingBox, eye, transformMatrix,
                                  globalViewport, currentViewport);
        });
    }

    if ((renderingEntitiesFlag & RenderingEdges) != 0) {
        nb = layerLODUnit->edgesLODVector.size();

        if (computeEdgesLOD) {
            TLP_PARALLEL_MAP_INDICES(nb, [&](uint i) {
                layerLODUnit->edgesLODVector[i].lod =
                    calculateAABBSize(layerLODUnit->edgesLODVector[i].boundingBox, eye,
                                      transformMatrix, globalViewport, currentViewport);
            });
        } else {
            TLP_PARALLEL_MAP_INDICES(nb, [&](uint i) { layerLODUnit->edgesLODVector[i].lod = 10; });
        }
    }
}

void GlCPULODCalculator::computeFor2DCamera(LayerLODUnit *layerLODUnit, const Vec4i &globalViewport,
                                            const Vec4i &currentViewport) {

    for (auto &it : layerLODUnit->entitiesLODVector) {
        it.lod = calculate2DLod(it.boundingBox, globalViewport, currentViewport);
    }

    for (auto &it : layerLODUnit->nodesLODVector) {
        it.lod = calculate2DLod(it.boundingBox, globalViewport, currentViewport);
    }

    for (auto &it : layerLODUnit->edgesLODVector) {
        it.lod = calculate2DLod(it.boundingBox, globalViewport, currentViewport);
    }
}
}
