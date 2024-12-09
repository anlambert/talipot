/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_QUAD_TREE_LOD_CALCULATOR_H
#define TALIPOT_GL_QUAD_TREE_LOD_CALCULATOR_H

#include <talipot/hash.h>
#include <vector>

#include <talipot/GlCPULODCalculator.h>
#include <talipot/Observable.h>
#include <talipot/GlGraphRenderingParameters.h>

namespace tlp {

class Camera;
template <class TYPE>
class QuadTreeNode;
class GlScene;
class PropertyInterface;
class Graph;
class GlLayer;

/**
 * Class used to compute bounding box of a vector of GlEntity
 */
class TLP_GL_SCOPE GlQuadTreeLODCalculator : public GlCPULODCalculator, private Observable {

public:
  GlQuadTreeLODCalculator();
  ~GlQuadTreeLODCalculator() override;

  /**
   * Set the SlScene used by this calculator
   */
  void setScene(GlScene &scene) override;

  /**
   * To know if the calculator need to have entities returned by a visitor in GlScene
   */
  bool needEntities() override;
  /**
   * Set if the calculator need to have entities
   */
  void setNeedEntities(bool) override;

  /**
   * This function is call by GlLODSceneVisitor when a simple entity is found
   */
  void addEntityBoundingBox(GlEntity *entity, const BoundingBox &bb) override;
  /**
   * This function is call by GlLODSceneVisitor when an edge is found
   */
  void addEdgeBoundingBox(Graph *graph, edge e, const BoundingBox &bb) override;

  /**
   * This function compute LOD
   * See compute function of GlCPULODCalculator for more details
   * This function do some computation and after call computeFor2DCamera() or computeFor3DCamera()
   */
  void compute(const Vec4i &globalViewport, const Vec4i &currentViewport) override;

  /**
   * Specific function to compute LOD for 3D cameras
   */
  void computeFor3DCamera(LayerLODUnit *layerLODUnit, const Coord &eye,
                          const MatrixGL &transformMatrix, const Vec4i &globalViewport,
                          const Vec4i &currentViewport) override;

  /**
   * Change the input data used by this LOD calculator
   */
  void setInputData(const GlGraphInputData *newInputData) override;

  /**
   * Clone the calculator
   */
  GlLODCalculator *clone() override {
    auto *newCalculator = new GlQuadTreeLODCalculator();
    newCalculator->setScene(*glScene);
    newCalculator->setInputData(inputData);
    return newCalculator;
  }

protected:
  void update(PropertyInterface *property);
  void treatEvent(const Event &ev) override;

  void removeObservers();
  void addObservers();

  void initCamerasObservers();
  void clearCamerasObservers();

  void setHaveToCompute();

  std::vector<QuadTreeNode<uint> *> nodesQuadTree;
  std::vector<QuadTreeNode<uint> *> edgesQuadTree;
  std::vector<QuadTreeNode<GlEntity *> *> entitiesQuadTree;
  std::vector<std::vector<EntityLODUnit>> entities;

  bool haveToCompute;
  bool haveToInitObservers;

  // index of simple entities bounding in bbs (see CPULODCalculator.h)
  const uint seBBIndex;
  // offset of edge entities bounding in bbs
  const uint eBBOffset;

  std::vector<Camera *> cameras;
  flat_hash_map<GlLayer *, Camera> layerToCamera;
  Camera *currentCamera;
  Graph *currentGraph;
  PropertyInterface *layoutProperty;
  PropertyInterface *sizeProperty;
  PropertyInterface *selectionProperty;
  GlGraphRenderingParameters oldParameters;

  int quadTreesVectorPosition;
  int entitiesVectorPosition;
};
}

#endif // TALIPOT_GL_QUAD_TREE_LOD_CALCULATOR_H
