/**
 *
 * Copyright (C) 2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <unordered_map>

#include <talipot/OGDFUtils.h>

using namespace std;

tlp::Graph *convertOGDFGraphToTalipotGraph(ogdf::Graph &graph, tlp::Graph *tlpGraph) {
  if (!tlpGraph) {
    tlpGraph = tlp::newGraph();
  } else {
    tlpGraph->clear();
  }

  unordered_map<ogdf::node, tlp::node> nodesMap;

  for (ogdf::node n : graph.nodes) {
    nodesMap[n] = tlpGraph->addNode();
  }

  for (ogdf::edge e : graph.edges) {
    tlpGraph->addEdge(nodesMap[e->source()], nodesMap[e->target()]);
  }

  return tlpGraph;
}
