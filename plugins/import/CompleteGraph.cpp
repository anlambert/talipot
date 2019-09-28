/**
 *
 * Copyright (C) 2019  The Talipot developers
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
    // nodes
    "Number of nodes in the final graph.",

    // directed
    "If false, the generated graph is undirected. If true, two edges are created between each pair "
    "of nodes."};

class CompleteGraph : public ImportModule {
public:
  PLUGININFORMATION("Complete General Graph", "Auber", "16/12/2002",
                    "Imports a new complete graph.", "1.2", "Graph")
  CompleteGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<unsigned int>("nodes", paramHelp[0], "5");
    addInParameter<bool>("directed", paramHelp[1], "false");
  }

  bool importGraph() override {
    unsigned int nbNodes = 5;
    bool directed = false;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      if (!dataSet->get("directed", directed) &&
          // for compatibility with version 1.1
          dataSet->get("undirected", directed))
        directed = !directed;
    }

    if (nbNodes == 0) {
      if (pluginProgress)
        pluginProgress->setError("Error: number of nodes must be greater than 0");

      return false;
    }

    if (pluginProgress)
      pluginProgress->showPreview(false);

    vector<node> nodes(nbNodes);

    graph->reserveNodes(nbNodes);

    for (size_t j = 0; j < nbNodes; ++j)
      nodes[j] = graph->addNode();

    if (!directed)
      graph->reserveEdges(nbNodes - 1);
    else
      graph->reserveEdges(2 * (nbNodes - 1));

    for (size_t i = 0; i < nbNodes - 1; ++i)
      for (size_t j = i + 1; j < nbNodes; ++j) {
        graph->addEdge(nodes[i], nodes[j]);

        if (directed)
          graph->addEdge(nodes[j], nodes[i]);
      }

    return true;
  }
};

PLUGIN(CompleteGraph)
