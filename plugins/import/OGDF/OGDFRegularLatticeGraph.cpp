/**
 *
 * Copyright (C) 2024-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/basic/graph_generators/deterministic.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes in the graph, must be at least 4",

    // k
    "the degree of each node, must be an even number between 0 and n-2",
};

//=================================================================================

class OGDFRegularLatticeGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Regular Lattice Graph (OGDF)", "Antoine Lambert", "03/2024",
                    "Generates a cycle on n sequential nodes, where any two nodes whose"
                    " distance is at most k / 2 are connected by an additional edge.",
                    "1.0", "OGDF")

  OGDFRegularLatticeGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "50");
    addInParameter<int>("k", paramHelp[1].data(), "6");
  }

  bool importOGDFGraph() override {
    int n = 50;
    int k = 6;
    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("k", k);
    }

    if (n < 4) {
      if (pluginProgress) {
        pluginProgress->setError("n must be at least 4");
      }
      return false;
    }
    if (k < 0 || k > n - 2 || k % 2 == 1) {
      if (pluginProgress) {
        pluginProgress->setError("k must be an even number between 0 and " + to_string(k));
      }
      return false;
    }
    ogdf::regularLatticeGraph(*G, n, k);
    return true;
  }

private:
};

PLUGIN(OGDFRegularLatticeGraph)
