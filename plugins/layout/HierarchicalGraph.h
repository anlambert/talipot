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

#ifndef HIERARCHICAL_GRAPH_H
#define HIERARCHICAL_GRAPH_H

#include <memory>
#include <talipot/hash.h>

#include <talipot/PluginHeaders.h>

class LessThanNode2 {
public:
  tlp::DoubleProperty *metric;
  bool operator()(tlp::node n1, tlp::node n2) const {
    return (metric->getNodeValue(n1) < metric->getNodeValue(n2));
  }
};

/**
 * \file
 * \brief An implementation of hierarchical graph drawing algorithm
 *
 * This plugin is an implementation of the hierarchical layout algorithm
 * first published as:
 *
 *  D. Auber, \n
 *  "Tulip - A Huge Graph Visualization Framework", \n
 *  "Book. Graph Drawing Software. (Ed. Michael Junger & Petra Mutzel)", \n
 *  "2004", \n
 *  pages 105 - 126.
 *
 */
class HierarchicalGraph : public tlp::LayoutAlgorithm {

public:
  PLUGININFORMATION("Hierarchical Graph", "David Auber", "23/05/2000",
                    "Implements the hierarchical layout algorithm  first published as:<br/>"
                    "<b>Tulip - A Huge Graph Visualization Framework</b>, D. Auber, Book. Graph "
                    "Drawing Software. (Ed. Michael Junger & Petra Mutzel) pages 105--126. (2004).",
                    "1.0", "Hierarchical")
  HierarchicalGraph(const tlp::PluginContext *context);
  ~HierarchicalGraph() override;
  bool run() override;

private:
  void DagLevelSpanningTree(tlp::Graph *sg, tlp::DoubleProperty *embedding);
  std::vector<std::vector<tlp::node>> grid;
  std::unique_ptr<tlp::DoubleProperty> embedding;
  void twoLayerCrossReduction(tlp::Graph *sg, uint freeLayer);
  void crossReduction(tlp::Graph *sg);
  void computeEdgeBends(const tlp::Graph *mySGraph, tlp::LayoutProperty &tmpLayout,
                        const flat_hash_map<tlp::edge, tlp::edge> &replacedEdges,
                        const std::vector<tlp::edge> &reversedEdges);
  void computeSelfLoops(tlp::Graph *mySGraph, tlp::LayoutProperty &tmpLayout,
                        std::vector<tlp::SelfLoops> &listSelfLoops);
  void buildGrid(tlp::Graph *);
  uint degree(tlp::Graph *sg, tlp::node n, bool sense);
  void initCross(tlp::Graph *sg, tlp::node n, tlp::MutableContainer<bool> &visited, int id);

  LessThanNode2 lessNode;
  std::string orientation;
  float spacing;
  float nodeSpacing;
};

#endif // HIERARCHICAL_GRAPH_H
