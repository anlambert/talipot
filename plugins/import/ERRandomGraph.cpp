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

#include <talipot/ImportModule.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

namespace {
static constexpr std::string_view paramHelp[] = {
    // nodes
    "Number of nodes in the final graph.",

    // probability
    "Probability of having an edge between each pair of vertices in the graph.",

    // self loop
    "Generate self loops (an edge with source and target on the same node) with the same "
    "probability",

    // directed
    "Generate a directed graph (arcs u->v and v->u have the same probability)"};
}

/** Random Graph - Import of a random graph based on the Erdős-Rényi Model
 *
 *  User can specify the number of nodes and the probability of having an edge between two nodes.
 */
class ERRandomGraph : public ImportModule {
public:
  PLUGININFORMATION("Erdős-Rényi Random Graph", "Bruno Pinaud", "08/09/2014",
                    "Import a randomly generated graph following the Erdős-Rényi model. Given a "
                    "positive integer n and a probability value in [0,1], define the graph G(n,p) "
                    "to be the undirected graph on n vertices whose edges are chosen as follows: "
                    "For all pairs of vertices v,w there is an edge (v,w) with probability p.",
                    "1.1", "Graph")
  ERRandomGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "50");
    addInParameter<double>("probability", paramHelp[1].data(), "0.5");
    addInParameter<bool>("self loop", paramHelp[2].data(), "false");
    addInParameter<bool>("directed", paramHelp[3].data(), "false");
  }

  bool importGraph() override {
    // initialize a random sequence according to the given seed
    tlp::initRandomSequence();

    uint nbNodes = 50;
    double proba = 0.5;
    bool self_loop = false;
    bool directed = false;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("probability", proba);
      dataSet->get("self loop", self_loop);
      dataSet->get("directed", directed);
    }

    if (nbNodes == 0) {
      if (pluginProgress) {
        pluginProgress->setError(string("Error: the number of nodes cannot be null."));
      }

      return false;
    }

    if ((proba < 0) || (proba > 1)) {
      if (pluginProgress) {
        pluginProgress->setError(string("Error: the probability must be between ]0, 1[."));
      }

      return false;
    }

    // add nodes
    graph->addNodes(nbNodes);
    const vector<node> &nodes = graph->nodes();

    unsigned i = 0;

    while (i != nbNodes) {
      ++i;
      node u = nodes[nbNodes - i];

      if (pluginProgress && pluginProgress->progress(i, nbNodes) != ProgressState::TLP_CONTINUE) {
        return pluginProgress->state() != ProgressState::TLP_CANCEL;
      }

      uint max_index_j = nbNodes - i + 1;
      if (directed) {
        max_index_j = nbNodes;
      }

      for (uint j = 0; j < max_index_j; ++j) {
        node v = nodes[j];

        if ((u == v) && (!self_loop)) {
          continue;
        }

        if (tlp::randomDouble() < proba) {
          graph->addEdge(u, v);
        }
      }
    }

    return true;
  }
};

PLUGIN(ERRandomGraph)
