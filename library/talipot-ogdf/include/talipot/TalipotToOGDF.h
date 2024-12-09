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

#ifndef TALIPOT_TALIPOT_TO_OGDF_H
#define TALIPOT_TALIPOT_TO_OGDF_H

#include <talipot/config.h>
#include <talipot/Coord.h>
#include <talipot/Graph.h>

#include <ogdf/basic/GraphAttributes.h>

#include <talipot/hash.h>
#include <vector>

namespace tlp {
class NumericProperty;
class SizeProperty;

class TLP_OGDF_SCOPE TalipotToOGDF {
public:
  TalipotToOGDF(Graph *g, bool importEdgeBends = true);

  Graph *getTalipotGraph() {
    return talipotGraph;
  }

  ogdf::GraphAttributes &getOGDFGraphAttr() {
    return ogdfGraphAttributes;
  }

  ogdf::Graph &getOGDFGraph() {
    return ogdfGraph;
  }

  ogdf::node getOGDFGraphNode(node nTlp) {
    return ogdfNodes[nTlp];
  }

  ogdf::edge getOGDFGraphEdge(edge eTlp) {
    return ogdfEdges[eTlp];
  }

  void makeOGDFGraphSimple();

  void enable3DLayout(bool enable3D);

  Coord getNodeCoordFromOGDFGraphAttr(node n);
  std::vector<Coord> getEdgeCoordFromOGDFGraphAttr(edge e);

  void copyTlpNumericPropertyToOGDFEdgeLength(NumericProperty *);
  void copyTlpNumericPropertyToOGDFNodeWeight(NumericProperty *);
  void copyTlpNodeSizeToOGDF(SizeProperty *);

private:
  Graph *talipotGraph;
  flat_hash_map<node, ogdf::node> ogdfNodes;
  flat_hash_map<edge, ogdf::edge> ogdfEdges;
  ogdf::Graph ogdfGraph;
  ogdf::GraphAttributes ogdfGraphAttributes;
};
}
#endif // TALIPOT_TALIPOT_TO_OGDF_H
