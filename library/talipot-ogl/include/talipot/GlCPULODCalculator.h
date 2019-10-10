/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

///@cond DOXYGEN_HIDDEN

#ifndef TALIPOT_GL_CPU_LOD_CALCULATOR_H
#define TALIPOT_GL_CPU_LOD_CALCULATOR_H

#include <vector>

#include <talipot/Coord.h>
#include <talipot/GlLODCalculator.h>
#include <talipot/Matrix.h>

namespace tlp {
class GlSimpleEntity;
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
    GlCPULODCalculator *calculator = new GlCPULODCalculator();
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
  void addSimpleEntityBoundingBox(GlSimpleEntity *entity, const BoundingBox &bb) override;
  /**
   * This function is called by GlLODSceneVisitor when a node is found
   */
  void addNodeBoundingBox(unsigned int id, unsigned int pos, const BoundingBox &bb) override;
  /**
   * This function is called by GlLODSceneVisitor when an edge is found
   */
  void addEdgeBoundingBox(unsigned int id, unsigned int pos, const BoundingBox &bb) override;

  /**
   * Reserve memory to store nodes and edges LOD
   * this function is an optimisation function
   */
  void reserveMemoryForGraphElts(unsigned int nbNodes, unsigned int nbEdges) override;

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
  void compute(const Vector<int, 4> &globalViewport,
               const Vector<int, 4> &currentViewport) override;

  /**
   * This function return the scene bounding box
   */
  BoundingBox getSceneBoundingBox() override;

  /**
   * Set if the edge LOD must be calculated
   * \Warning If not calculated, the default edge LOD is 10.
   */
  inline void setComputeEdgesLOD(bool state) {
    computeEdgesLOD = state;
  }

  /**
   * Set if the LOD is computed for out screen entities
   */
  inline void setComputeOutScreenLOD(bool state) {
    computeOutScreenLOD = state;
  }

protected:
  virtual void computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye,
                                  const Matrix<float, 4> &transformMatrix,
                                  const Vector<int, 4> &globalViewport,
                                  const Vector<int, 4> &currentViewport);

  virtual void computeFor2DCamera(LayerLODUnit *layerLODUnit, const Vector<int, 4> &globalViewport,
                                  const Vector<int, 4> &currentViewport);

  bool computeEdgesLOD;
  bool computeOutScreenLOD;

  std::vector<bool> noBBCheck;
  std::vector<BoundingBox> bbs;

  LayerLODUnit *currentLayerLODUnit;
};
}

#endif // TALIPOT_GL_CPU_LOD_CALCULATOR_H
///@endcond