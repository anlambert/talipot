/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
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

static constexpr std::string_view paramHelp[] = {
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
    addInParameter<double>("d", paramHelp[0].data(), "0.85");
    addInParameter<bool>("directed", paramHelp[1].data(), "true");
    addInParameter<NumericProperty *>("weight", paramHelp[2].data(), "", false);
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
    NodeVectorProperty<double> pr(graph);
    NodeVectorProperty<double> next_pr(graph);
    uint nbNodes = graph->numberOfNodes();

    double oon = 1. / nbNodes;

    pr.setAll(oon);

    const double one_minus_d = (1 - d) / nbNodes;
    const auto kMax = uint(15 * log(nbNodes));

    NodeVectorProperty<double> deg(graph);
    tlp::degree(graph, deg, directed ? EdgeType::DIRECTED : EdgeType::UNDIRECTED, weight, false);

    for (uint k = 0; k < kMax + 1; ++k) {
      if (!weight) {
        TLP_PARALLEL_MAP_NODES(graph, [&](const node n) {
          double n_sum = 0;
          for (auto nin : getAdjacentNodesIterator(
                   graph, n, directed ? EdgeType::INV_DIRECTED : EdgeType::UNDIRECTED)) {
            n_sum += pr.getNodeValue(nin) / deg.getNodeValue(nin);
          }
          next_pr[n] = one_minus_d + d * n_sum;
        });
      } else {
        TLP_PARALLEL_MAP_NODES(graph, [&](const node n) {
          double n_sum = 0;
          for (auto e : getIncidentEdgesIterator(
                   graph, n, directed ? EdgeType::INV_DIRECTED : EdgeType::UNDIRECTED)) {
            node nin = graph->opposite(e, n);
            if (deg.getNodeValue(nin) > 0) {
              n_sum += weight->getEdgeDoubleValue(e) * pr.getNodeValue(nin) / deg.getNodeValue(nin);
            }
          }
          next_pr[n] = one_minus_d + d * n_sum;
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
