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

#include <talipot/ImportModule.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // nodes
    "This parameter defines the amount of nodes used to build the scale-free graph.",

    // k
    "Number of edges added to each node in the initial ring lattice. "
    "Be careful that #nodes > k > ln(#nodes) > 1",

    // p
    "Probability in [0,1] to rewire an edge.",

    // original model
    "Use the original model: k describes the degree of each vertex (k > 1 and even).",
};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * D. J. Watts and S. H. Strogatz.
 * Collective dynamics of "small-world" networks.
 * Nature 393, 440 (1998).
 *
 */
struct WattsStrogatzModel : public ImportModule {
  PLUGININFORMATION("Watts Strogatz Model", "Arnaud Sallaberry", "21/02/2011",
                    "Randomly generates a small world graph using the model described in<br/>D. J. "
                    "Watts and S. H. Strogatz.<br/><b>Collective dynamics of small-world "
                    "networks.</b><br/>Nature 393, 440 (1998).",
                    "1.1", "Social network")

  WattsStrogatzModel(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "500");
    addInParameter<uint>("k", paramHelp[1].data(), "25");
    addInParameter<double>("p", paramHelp[2].data(), "0.02");
    addInParameter<bool>("original model", paramHelp[3].data(), "true");
  }

  bool importGraph() override {
    uint nbNodes = 500;
    uint k = 25;
    double p = 0.02;
    bool original_model = true;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("k", k);
      dataSet->get("p", p);
      dataSet->get("original model", original_model);
    }

    // check arguments
    if (p < 0 || p > 1) {
      pluginProgress->setError("p is not a probability,\nit does not belong to [0, 1]");
      return false;
    }
    if (k >= nbNodes) {
      pluginProgress->setError("The k parameter cannot be greater than the number of nodes.");
      return false;
    }
    if (original_model && (log(float(nbNodes)) >= k)) {
      pluginProgress->setError("The k parameter must be greater than ln(nodes).");
      return false;
    }

    if (original_model) {
      if (k % 2 == 1) {
        stringstream sstr(
            "k must be an even number when used in the original model; rounding k down to ");
        sstr << k - 1 << '.';
        pluginProgress->setComment(sstr.str());
        k--;
      }

      if (k > 0) {
        k = (k - 2) / 2;
      }
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    graph->addNodes(nbNodes);
    const vector<node> &nodes = graph->nodes();

    if (original_model) {
      graph->reserveEdges(nbNodes * k / 2);
    } else {
      graph->reserveEdges(nbNodes * (k + 1));
    }

    for (uint i = 1; i < nbNodes; ++i) {
      graph->addEdge(nodes[i - 1], nodes[i]);
    }

    graph->addEdge(nodes[nbNodes - 1], nodes[0]);

    for (uint i = 0; i < nbNodes; ++i) {
      for (uint j = 0; j < k; ++j) {
        int d = i - j - 2;

        if (d < 0) {
          graph->addEdge(nodes[nbNodes + d], nodes[i]);
        } else {
          graph->addEdge(nodes[d], nodes[i]);
        }
      }
    }

    node n1, n2;

    if (original_model) {
      for (auto e : graph->edges()) {
        if (tlp::randomNumber() < p) {
          n1 = graph->source(e);

          do {
            n2 = nodes[tlp::randomNumber(nbNodes - 1)];
          } while (graph->hasEdge(n1, n2, false));

          // only reroute target; ensure to keep the graph connected
          graph->setTarget(e, n2);
        }
      }
    } else {
      for (auto e : graph->edges()) {
        if (tlp::randomNumber() < p) {
          do {
            n1 = nodes[tlp::randomNumber(nbNodes - 1)];
            n2 = nodes[tlp::randomNumber(nbNodes - 1)];
          } while (graph->hasEdge(n1, n2, false));

          graph->setEnds(e, n1, n2);
        }
      }
    }

    return true;
  }
};

PLUGIN(WattsStrogatzModel)
