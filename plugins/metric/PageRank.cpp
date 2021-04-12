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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

static const char *paramHelp[] = {
    // d
    "Enables to choose a damping factor in ]0,1[.",

    // directed
    "Indicates if the graph should be considered as directed or not.",

    // weight
    "An existing edge weight metric property."};

/** \file
 *  \brief  An implementation of the PageRank metric
 *
 * This plugin is an implementation of the PageRank metric.
 * First designed by Larry Page and Sergey Brin, it is a link analysis
 * algorithm that assigns a measure to each node of an 'hyperlinked' graph.
 *
 * (see http://en.wikipedia.org/wiki/PageRank for more details)
 *
 *  <b>HISTORY</b>
 *
 *  - 2007 Version 1.0: Initial release
 *  by Mohamed Bouklit, LaBRI, University Bordeaux I, France
 *  - 2010 Version 1.1: rewrite to work properly with subgraph
 *  by David Auber, LaBRI, University Bordeaux I, France
 *  - 2011 Version 2.0: fix computation for undirected graph
 *  by François Queyroi, LaBRI, University Bordeaux I, France
 *  - 2019 Version 2.1: add edge weight as parameter
 *  by François Queyroi, LS2N, University of Nantes, France
 *
 *
 */
struct PageRank : public DoubleAlgorithm {

  PLUGININFORMATION("Page Rank", "Mohamed Bouklit & David Auber", "16/12/10",
                    "Nodes measure used for links analysis.<br/>"
                    "First designed by Larry Page and Sergey Brin, it is a link analysis algorithm "
                    "that assigns a measure to each node of an 'hyperlinked' graph.",
                    "2.1", "Graph")

  PageRank(const PluginContext *context) : DoubleAlgorithm(context) {
    addInParameter<double>("d", paramHelp[0], "0.85");
    addInParameter<bool>("directed", paramHelp[1], "true");
    addInParameter<NumericProperty *>("weight", paramHelp[2], "", false);
  }

  bool run() override {
    double d = 0.85;
    bool directed = true;
    NumericProperty *weight = nullptr;

    if (dataSet != nullptr) {
      dataSet->get("d", d);
      dataSet->get("directed", directed);
      dataSet->get("weight", weight);
    }

    if (d <= 0 || d >= 1) {
      return false;
    }

    // Initialize the PageRank
    NodeStaticProperty<double> pr(graph);
    NodeStaticProperty<double> next_pr(graph);
    unsigned int nbNodes = graph->numberOfNodes();

    double oon = 1. / nbNodes;

    pr.setAll(oon);

    const double one_minus_d = (1 - d) / nbNodes;
    const auto kMax = uint(15 * log(nbNodes));

    NodeStaticProperty<double> deg(graph);
    tlp::degree(graph, deg, directed ? DIRECTED : UNDIRECTED, weight, false);

    auto getNodes = getNodesIterator(directed ? INV_DIRECTED : UNDIRECTED);
    auto getEdges = getEdgesIterator(directed ? INV_DIRECTED : UNDIRECTED);

    for (unsigned int k = 0; k < kMax + 1; ++k) {
      if (!weight) {
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, unsigned int i) {
          double n_sum = 0;
          for (auto nin : getNodes(graph, n)) {
            n_sum += pr.getNodeValue(nin) / deg.getNodeValue(nin);
          }
          next_pr[i] = one_minus_d + d * n_sum;
        });
      } else {
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, unsigned int i) {
          double n_sum = 0;
          for (auto e : getEdges(graph, n)) {
            node nin = graph->opposite(e, n);
            if (deg.getNodeValue(nin) > 0) {
              n_sum += weight->getEdgeDoubleValue(e) * pr.getNodeValue(nin) / deg.getNodeValue(nin);
            }
          }
          next_pr[i] = one_minus_d + d * n_sum;
        });
      }

      // swap pr and next_pr
      pr.swap(next_pr);
    }

    // store the pr values
    pr.copyToProperty(result);

    return true;
  }
};

PLUGIN(PageRank)
