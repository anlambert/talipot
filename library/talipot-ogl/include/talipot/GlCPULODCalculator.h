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

#ifndef TALIPOT_GL_CPU_LOD_CALCULATOR_H
#define TALIPOT_GL_CPU_LOD_CALCULATOR_H

#include <vector>

#include <talipot/Coord.h>
#include <talipot/GlLODCalculator.h>
#include <talipot/Matrix.h>
#include <talipot/GlTools.h>

namespace tlp {
class GlEntity;
class Camera;

/**
 * \brief Class used to compute LOD of GlEntities with OpenMP parallelization
 *
 * This class perform LOD computation of GlEntities based on screen projection of entities bounding
 * boxes
 * \warning By default this class don't compute LOD for edges (for optimisation) and return a lod of
 * 10. to these edges, if you want to compute edges' LOD call setComputeEdgesLOD(true)
 */
class TLP_GL_SCOPE GlCPULODCalculator : public GlLODCalculator {

  public:
    GlCPULODCalculator();
    ~GlCPULODCalculator() override;
    GlLODCalculator *clone() override {
        auto *calculator = new GlCPULODCalculator();
        calculator->setComputeOutScreenLOD(computeOutScreenLOD);
        return calculator;
    }

    /**
     * Begin a new camera (use to render next entities)
     */
    void beginNewCamera(Camera *camera) override;
    /**
     * This function is called by GlLODSceneVisitor when a simple entity is found
     */
    void addEntityBoundingBox(GlEntity *entity, const BoundingBox &bb) override;
    /**
     * This function is called by GlLODSceneVisitor when a node is found
     */
    void addNodeBoundingBox(Graph *graph, node n, const BoundingBox &bb) override;
    /**
     * This function is called by GlLODSceneVisitor when an edge is found
     */
    void addEdgeBoundingBox(Graph *graph, edge e, const BoundingBox &bb) override;

    /**
     * Reserve memory to store nodes and edges LOD
     * this function is an optimisation function
     */
    void reserveMemoryForGraphElts(uint nbNodes, uint nbEdges) override;

    /**
     * Compute all bounding boxes
     * If you want to compute LOD for a simple scene, you just have to call this function with the
     * same value for globalViewport and currentViewport
     * But if you want to perform a sub screen part selection you have to call this function with:
     * globalViewport the viewport of the visualisation and currentViewport the viewport of the
     * selection
     * \param globalViewport is used to compute LOD
     * \param currentViewport return -1 for all entities outside this viewport
     */
    void compute(const Vec4i &globalViewport, const Vec4i &currentViewport) override;

    /**
     * This function return the scene bounding box
     */
    BoundingBox getSceneBoundingBox() override;

    /**
     * Set if the edge LOD must be calculated
     * \Warning If not calculated, the default edge LOD is 10.
     */
    void setComputeEdgesLOD(bool state) {
        computeEdgesLOD = state;
    }

    /**
     * Set if the LOD is computed for out screen entities
     */
    void setComputeOutScreenLOD(bool state) {
        computeOutScreenLOD = state;
    }

  protected:
    virtual void computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye,
                                    const MatrixGL &transformMatrix, const Vec4i &globalViewport,
                                    const Vec4i &currentViewport);

    virtual void computeFor2DCamera(LayerLODUnit *layerLODUnit, const Vec4i &globalViewport,
                                    const Vec4i &currentViewport);

    bool computeEdgesLOD;
    bool computeOutScreenLOD;

    std::vector<BoundingBox> bbs;

    LayerLODUnit *currentLayerLODUnit;
};
}

#endif // TALIPOT_GL_CPU_LOD_CALCULATOR_H
