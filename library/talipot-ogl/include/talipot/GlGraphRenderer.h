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

#ifndef TALIPOT_GL_GRAPH_RENDERER_H
#define TALIPOT_GL_GRAPH_RENDERER_H

#include <talipot/hash.h>

#include <talipot/GlScene.h>
#include <talipot/config.h>

namespace tlp {

class Graph;
class GlGraphInputData;
class Camera;

/**
 * @ingroup OpenGL
 * @brief Class used by GlGraph to render the graph in OpenGL
 *
 * To create a graph renderer, you have to implement two functions : draw() and selectEntities()
 * @see GlGraph
 */
class TLP_GL_SCOPE GlGraphRenderer {

public:
  /**
   * @brief Constructor
   * \param inputData : GlGraphInputData used by renderer to display the graph (in input data you
   * have pointers on properties used to render nodes/edges
   * \param parameters : GlGraphRenderingParameters used by renderer to display the graph
   */
  GlGraphRenderer(const GlGraphInputData *inputData);

  /**
   * @brief Destructor
   */
  virtual ~GlGraphRenderer() = default;

  /**
   * @brief This function is call by GlGraph to draw the graph
   *
   * If you reimplement this function you have to render nodes/edges. It's the most important
   * function of GlGraphRenderer
   *
   * \param lod : lod used to this Rendering
   * \param camera : camera used to this rendering
   */
  virtual void draw(float lod, Camera *camera) = 0;

  /**
   * @brief This function is call by GlGraph to selected entities into the graph
   * \param type : type of selected entities
   * \param x : x of the selected zone
   * \param y : y of the selected zone
   * \param w : width of the selected zone
   * \param h : height of the selected zone
   * \param selectedEntities : you have to put selected entities into this vector
   */
  virtual void selectEntities(Camera *camera, RenderingEntitiesFlag type, int x, int y, int w,
                              int h, std::vector<SelectedEntity> &selectedEntities) = 0;

  /**
   * @brief You can use this function if you want to inject a visitor on the graph
   */
  virtual void visitGraph(GlSceneVisitor *visitor, bool visitHiddenEntities = false);

protected:
  void visitNodes(Graph *graph, GlSceneVisitor *visitor);
  void visitEdges(Graph *graph, GlSceneVisitor *visitor);

  const GlGraphInputData *inputData;

  bool selectionDrawActivate;
  RenderingEntitiesFlag selectionType;
  flat_hash_map<uint, SelectedEntity> *selectionIdMap;
  uint *selectionCurrentId;
};
}

#endif // TALIPOT_GL_GRAPH_RENDERER_H
