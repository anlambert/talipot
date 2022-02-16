/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/GlGraph.h>
#include <talipot/GraphProperty.h>
#include <talipot/GlXMLTools.h>
#include <talipot/GlBoundingBoxSceneVisitor.h>
#include <talipot/GlGraphHighDetailsRenderer.h>

using namespace std;

namespace tlp {

GlGraph::GlGraph(Graph *graph, GlGraphRenderer *graphRenderer)
    : _inputData(graph, &parameters), graphRenderer(graphRenderer), nodesModified(true) {
  if (graphRenderer == nullptr) {
    this->graphRenderer = new GlGraphHighDetailsRenderer(&_inputData);
  }

  if (!graph) {
    rootGraph = nullptr;
  } else {
    rootGraph = graph->getRoot();
    graph->addListener(this);
    graph->getRoot()->getGraphProperty("viewMetaGraph")->addListener(this);

    for (auto n : graph->nodes()) {
      if (graph->getNodeMetaInfo(n)) {
        metaNodes.insert(n);
      }
    }
  }
}

GlGraph::GlGraph(Graph *graph, GlScene *scene)
    : _inputData(graph, &parameters), nodesModified(true) {
  this->graphRenderer = new GlGraphHighDetailsRenderer(&_inputData, scene);

  if (!graph) {
    rootGraph = nullptr;
  } else {
    rootGraph = graph->getRoot();
    graph->addListener(this);
    graph->getRoot()->getGraphProperty("viewMetaGraph")->addListener(this);

    for (auto n : graph->nodes()) {
      if (graph->getNodeMetaInfo(n)) {
        metaNodes.insert(n);
      }
    }
  }
}

GlGraph::~GlGraph() {
  delete graphRenderer;
}

void GlGraph::acceptVisitor(GlSceneVisitor *visitor) {
  GlBoundingBoxSceneVisitor bbVisitor(&_inputData);
  graphRenderer->visitGraph(&bbVisitor);
  boundingBox = bbVisitor.getBoundingBox();

  if (visitor && boundingBox.isValid()) {
    visitor->visit(this);
  }
}

void GlGraph::acceptVisitorOnGraph(GlSceneVisitor *visitor) {
  graphRenderer->visitGraph(visitor);
}

//===================================================================
void GlGraph::draw(float lod, Camera *camera) {
  graphRenderer->draw(lod, camera);
}
//===================================================================
void GlGraph::selectEntities(Camera *camera, RenderingEntitiesFlag type, int x, int y, int w, int h,
                             vector<SelectedEntity> &selectedEntities) {
  graphRenderer->selectEntities(camera, type, x, y, w, h, selectedEntities);
}
//===================================================================
const GlGraphRenderingParameters &GlGraph::renderingParameters() const {
  return parameters;
}
//===================================================================
void GlGraph::setRenderingParameters(const GlGraphRenderingParameters &parameter) {
  parameters = parameter;
}
//===================================================================
GlGraphRenderingParameters &GlGraph::renderingParameters() {
  return parameters;
}
//===================================================================
GlGraphInputData *GlGraph::inputData() const {
  return const_cast<GlGraphInputData *>(&_inputData);
}
//====================================================
void GlGraph::getXML(string &outString) {
  GlXMLTools::createProperty(outString, "type", "GlGraph", "GlEntity");
}
//====================================================
void GlGraph::setWithXML(const string &, uint &) {}

void GlGraph::treatEvent(const Event &evt) {
  const auto *graphEvent = dynamic_cast<const GraphEvent *>(&evt);

  if (graphEvent) {
    switch (graphEvent->getType()) {

    case GraphEventType::TLP_ADD_NODE:
    case GraphEventType::TLP_DEL_NODE:
      nodesModified = true;
      break;

    case GraphEventType::TLP_ADD_EDGE:
    case GraphEventType::TLP_DEL_EDGE:
    case GraphEventType::TLP_REVERSE_EDGE:
    case GraphEventType::TLP_AFTER_SET_ENDS:
      break;

    default:
      break;
    }
  } else if (evt.type() == EventType::TLP_DELETE) {
    auto *g = dynamic_cast<Graph *>(evt.sender());

    if (g && _inputData.graph() == g) {
      _inputData.setGraph(nullptr);
    }
  } else {
    const auto *propertyEvent = dynamic_cast<const PropertyEvent *>(&evt);

    if (propertyEvent && propertyEvent->getType() == PropertyEventType::TLP_AFTER_SET_NODE_VALUE) {
      nodesModified = true;
    }
  }
}

void GlGraph::setRenderer(tlp::GlGraphRenderer *renderer) {
  delete graphRenderer;

  if (renderer == nullptr) {
    graphRenderer = new GlGraphHighDetailsRenderer(&_inputData);
  } else {
    graphRenderer = renderer;
  }
}
}
