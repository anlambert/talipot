/**
 *
 * Copyright (C) 2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/basic/graph_generators/randomized.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes",
    // m
    "the number of edges, has a lower bound of n and an upper bound of 3*n-6. The supplied values "
    "are adjusted if they are out of these bounds.",
    // multiEdges
    "determines if the generated graph may contain multi-edges.",
};

//=================================================================================

class OGDFRandomPlanarBiconnectedGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Planar Biconnected Graph (OGDF)", "Antoine Lambert", "011/2024",
                    "Creates a random planar biconnected (embedded) graph.", "1.0", "OGDF")

  OGDFRandomPlanarBiconnectedGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "1000");
    addInParameter<int>("m", paramHelp[1].data(), "2000");
    addInParameter<bool>("multiEdges", paramHelp[2].data(), "false");
  }

  bool importOGDFGraph() override {
    int n = 1000;
    int m = 2000;
    bool multiEdges = false;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("m", m);
      dataSet->get("multiEdges", multiEdges);
    }

    ogdf::randomPlanarBiconnectedGraph(G, n, m, multiEdges);
    return true;
  }
};

PLUGIN(OGDFRandomPlanarBiconnectedGraph)
