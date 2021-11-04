/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_GRAPH_COMPOSITE_H
#define TALIPOT_GL_GRAPH_COMPOSITE_H

#include <talipot/GlComposite.h>
#include <talipot/Observable.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GlGraphInputData.h>
#include <talipot/GlScene.h>

namespace tlp {

class Graph;
class GlGraphRenderer;

/**
 * @ingroup OpenGL
 * @brief Class use to visualize graph in OpenGL Tulip engine
 *
 * GlEntity specialisation used to visualize graph in GlScene system
 * @see GlEntity
 * @see GlScene
 *
 * To visualize graph you have to create a new GlGraph and add it to a GlLayer of a GlScene
 * After that you can change some visualize parameters throw GlGraphRenderingParameters class
 * @see GlGraphRenderingParameters
 * @see renderingParameters()
 *
 * To render the graph in OpenGL, GlGraph use a GlGraphRenderer. So if you want to change
 * the system to render the graph, you have to create a new GlGraphRender
 * @see GlGraphRenderer
 */
class TLP_GL_SCOPE GlGraph : public GlComposite, public Observable {

public:
  /**
   * @brief Build a GlGraph with the graph data
   *
   * You can specify a GlGraphRenderer, if you don't do this a GlGraphHighDetailsRenderer will be
   * used to display the graph
   */
  GlGraph(Graph *graph, GlGraphRenderer *graphRenderer = nullptr);

  /**
   * @brief Build a GlGraph with the graph data
   *
   * Is better to use the other one constructor
   *
   * This graph composite is associated to the scene passed in parameter
   */
  GlGraph(Graph *graph, GlScene *scene);

  /**
   * @brief Destructor
   */
  ~GlGraph() override;

  /**
   * @brief Return a copy of rendering parameters used for rendering
   *
   */
  const GlGraphRenderingParameters &renderingParameters() const;
  GlGraphRenderingParameters &renderingParameters();
  /**
   * @brief Set the rendering parameters used for rendering
   */
  void setRenderingParameters(const GlGraphRenderingParameters &parameter);

  /**
   * @brief Return the inputData use by the composite
   *
   * In GlGraphInputData you have properties used to render the graph
   */
  GlGraphInputData *inputData() const;

  /**
   * @brief Return the graph used by this GlGraph
   */
  Graph *graph() {
    return _inputData.graph();
  }

  /**
   * Function used to visit composite's children
   */
  void acceptVisitor(GlSceneVisitor *visitor) override;
  /**
   * You have to use this function if you want to visit nodes/edges of the graph composite
   */
  virtual void acceptVisitorOnGraph(GlSceneVisitor *visitor);

  void draw(float lod, Camera *camera) override;

  virtual void selectEntities(Camera *camera, RenderingEntitiesFlag type, int x, int y, int w,
                              int h, std::vector<SelectedEntity> &selectedEntities);

  /**
   * Return set of metaNodes
   */
  std::set<node> &getMetaNodes() {
    if (nodesModified) {
      metaNodes.clear();

      Graph *graph = _inputData.graph();

      for (auto n : graph->nodes()) {
        if (graph->getNodeMetaInfo(n)) {
          metaNodes.insert(n);
        }
      }
      nodesModified = false;
    }

    return metaNodes;
  }

  GlGraphRenderer *getRenderer() {
    return graphRenderer;
  }

  /**
   * @brief setRenderer Delete the old renderer and replace it by the new one. If the new renderer
   * is equal to nullptr create a GlGraphHighDetailsRenderer.
   */
  void setRenderer(tlp::GlGraphRenderer *);

  /**
   * @brief Function to export data in outString (in XML format)
   */
  void getXML(std::string &outString) override;

  /**
   * @brief Function to set data with inString (in XML format)
   */
  void setWithXML(const std::string &inString, uint &currentPosition) override;

protected:
  void treatEvent(const Event &evt) override;

  GlGraphRenderingParameters parameters;
  GlGraphInputData _inputData;
  Graph *rootGraph;

  GlGraphRenderer *graphRenderer;

  bool nodesModified;
  std::set<node> metaNodes;
};
}

#endif // TALIPOT_GL_GRAPH_COMPOSITE_H
