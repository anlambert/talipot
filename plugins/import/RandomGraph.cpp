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

#include <talipot/PluginHeaders.h>

#define MAX_DENSITY_FOR_LINEAR 0.85

using namespace std;
using namespace tlp;

struct edgeS {
  edgeS() = default;
  edgeS(unsigned s, unsigned t) : source(s), target(t) {}
  unsigned source, target;
};

namespace std {
struct edgeS_comp {
  edgeS_comp(bool d) : directed(d) {}
  bool operator()(const edgeS &c, const edgeS &d) const {
    if (!directed) {
      int cs, ct, ds, dt;

      if (c.source <= c.target) {
        cs = c.source;
        ct = c.target;
      } else {
        ct = c.source;
        cs = c.target;
      }

      if (d.source <= d.target) {
        ds = d.source;
        dt = d.target;
      } else {
        dt = d.source;
        ds = d.target;
      }
      return (cs < ds) || ((cs == ds) && (ct < dt));
    } else {
      return (c.source < d.source) || ((c.source == d.source) && (c.target < d.target));
    }
  }
  bool directed;
};
} // namespace std

static constexpr std::string_view paramHelp[] = {
    // nodes
    "Number of nodes in the final graph.",

    // edges
    "Number of edges in the final graph.",

    // directed
    "If True, the graph may contain edges a->b and b->a."};

/// Random Graph - Import of a random graph
/** This plugin enables to create a random graph
 *
 *  User can specify the number of nodes and the number of edges of the graph.
 *
 * <b>HISTORY</b>
 * - 16/06/2002 Version 1.0: Initial Release (David Auber)
 * - 08/04/2019 Version 2.0: Add directed parameter + fix for high-density (François Queyroi)
 */
class RandomGraph : public ImportModule {
public:
  PLUGININFORMATION("Random General Graph", "Auber", "16/06/2002",
                    "Imports a new randomly generated graph.", "2.0", "Graph")
  RandomGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "500");
    addInParameter<uint>("edges", paramHelp[1].data(), "1000");
    addInParameter<bool>("directed", paramHelp[2].data(), "False", false);
  }
  ~RandomGraph() override = default;

  bool importGraph() override {
    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    uint nbNodes = 5;
    uint nbEdges = 9;
    bool directed = false;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("edges", nbEdges);
      dataSet->get("directed", directed);
    }

    if (nbNodes == 0) {
      if (pluginProgress) {
        pluginProgress->setError(string("Error: the number of nodes cannot be null"));
      }

      return false;
    }

    double density_g = (2. * nbEdges) / double(nbNodes * (nbNodes - 1));
    if (directed) {
      density_g /= 2.;
    }

    if (density_g > 1) {
      if (directed) {
        pluginProgress->setError(string("Error: For ") + std::to_string(nbNodes) +
                                 string(" nodes, the maximum number of edges is ") +
                                 std::to_string(nbNodes * (nbNodes - 1)));
      } else {
        pluginProgress->setError(string("Error: For ") + std::to_string(nbNodes) +
                                 string(" nodes, the maximum number of edges is ") +
                                 std::to_string(nbNodes * (nbNodes - 1) / 2));
      }
      return false;
    }

    uint nb_disctint_pairs_needed = nbEdges;
    if (density_g > MAX_DENSITY_FOR_LINEAR) {
      if (directed) {
        nb_disctint_pairs_needed = nbNodes * (nbNodes - 1) - nbEdges;
      } else {
        nb_disctint_pairs_needed = nbNodes * (nbNodes - 1) / 2 - nbEdges;
      }
    }

    edgeS_comp comp_e(directed);
    set<edgeS, edgeS_comp> myGraph(comp_e);

    if (pluginProgress) {
      pluginProgress->showPreview(false);
    }

    while (myGraph.size() < nb_disctint_pairs_needed) {
      if (myGraph.size() % nbNodes == 1 &&
          (pluginProgress->progress(myGraph.size(), nb_disctint_pairs_needed) !=
           ProgressState::TLP_CONTINUE)) {
        return pluginProgress->state() != ProgressState::TLP_CANCEL;
      }

      edgeS tmp;
      tmp.source = randomUnsignedInteger(nbNodes - 1);
      tmp.target = randomUnsignedInteger(nbNodes - 1);

      while (tmp.source == tmp.target) {
        tmp.source = randomUnsignedInteger(nbNodes - 1);
        tmp.target = randomUnsignedInteger(nbNodes - 1);
      }

      if ((myGraph.find(tmp) == myGraph.end()) && (myGraph.size() < nbEdges)) {
        myGraph.insert(tmp);
      }
    }

    graph->addNodes(nbNodes);

    graph->reserveEdges(nbEdges);

    const vector<node> &nodes = graph->nodes();

    if (density_g > MAX_DENSITY_FOR_LINEAR) {
      for (unsigned i = 0; i < nodes.size(); ++i) {
        unsigned min_j = 0;
        if (!directed) {
          min_j = i + 1;
        }
        for (unsigned j = min_j; j < nodes.size(); ++j) {
          if (i != j && myGraph.find(edgeS(i, j)) == myGraph.end()) {
            graph->addEdge(nodes[i], nodes[j]);
          }
        }
      }
    } else {
      for (const auto it : myGraph) {
        graph->addEdge(nodes[it.source], nodes[it.target]);
      }
    }

    return true;
  }
};

PLUGIN(RandomGraph)

/// Random Simple Graph - Import of a random graph
/** This plugin enables to create a random simple (undirected) graph
 *
 *  User can specify the number of nodes and the number of edges of the graph.
 *
 * <b>HISTORY</b>
 * - 16/06/2002 Version 1.0: Initial Release (David Auber)
 * - 20/04/2019 Version 2.0: Call to more general plugin "Random General Graph"
 */
class RandomSimpleGraph : public ImportModule {
public:
  PLUGININFORMATION("Random Simple Graph", "Auber", "16/06/2002",
                    "Imports a new randomly generated simple graph.", "1.0", "Graph")
  RandomSimpleGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "500");
    addInParameter<uint>("edges", paramHelp[1].data(), "1000");
  }

  bool importGraph() override {
    // for backward compatibility
    return tlp::importGraph("Random General Graph", *dataSet, pluginProgress, graph) != nullptr;
  }
};

PLUGIN(RandomSimpleGraph)
