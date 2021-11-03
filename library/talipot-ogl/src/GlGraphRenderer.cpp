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

#include <talipot/GlGraphRenderer.h>
#include <talipot/GlGraphRenderingParameters.h>
#include <talipot/GraphParallelTools.h>

using namespace std;

namespace tlp {

GlGraphRenderer::GlGraphRenderer(const GlGraphInputData *inputData)
    : inputData(inputData), selectionDrawActivate(false), selectionIdMap(nullptr),
      selectionCurrentId(nullptr) {}

void GlGraphRenderer::visitGraph(GlSceneVisitor *visitor, bool visitHiddenEntities) {
  Graph *graph = inputData->graph();

  if (!graph) {
    return;
  }

  uint nbNodes = graph->numberOfNodes();
  uint nbEdges = graph->numberOfEdges();
  if (!visitHiddenEntities && !inputData->renderingParameters()->isViewMetaLabel()) {
    if (!inputData->renderingParameters()->isDisplayNodes() &&
        !inputData->renderingParameters()->isViewNodeLabel()) {
      nbNodes = 0;
    }
    if (!inputData->renderingParameters()->isDisplayEdges() &&
        !inputData->renderingParameters()->isViewEdgeLabel()) {
      nbEdges = 0;
    }
  }
  visitor->reserveMemoryForGraphElts(nbNodes, nbEdges);
  if (nbNodes) {
    visitNodes(graph, visitor);
  }
  if (nbEdges) {
    visitEdges(graph, visitor);
  }
  visitor->endOfVisit();
}

void GlGraphRenderer::visitNodes(Graph *graph, GlSceneVisitor *visitor) {
  auto visitNode = [&](node n) {
    GlNode glNode(n, graph);
    visitor->visit(&glNode);
  };

  if (visitor->isThreadSafe()) {
    TLP_PARALLEL_MAP_NODES(graph, visitNode);
  } else {
    TLP_MAP_NODES(graph, visitNode);
  }
}

void GlGraphRenderer::visitEdges(Graph *graph, GlSceneVisitor *visitor) {
  auto visitEdge = [&](edge e) {
    GlEdge glEdge(e, graph);
    visitor->visit(&glEdge);
  };

  if (visitor->isThreadSafe()) {
    TLP_PARALLEL_MAP_EDGES(graph, visitEdge);
  } else {
    TLP_MAP_EDGES(graph, visitEdge);
  }
}
}
