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
    // d
    "the degree of each node",
};

//=================================================================================

class OGDFRandomRegularGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Regular Graph (OGDF)", "Antoine Lambert", "06/2024",
                    "Creates a random regular graph", "1.0", "OGDF")

  OGDFRandomRegularGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "1000");
    addInParameter<int>("d", paramHelp[1].data(), "4");
  }

  bool importOGDFGraph() override {
    int n = 1000;
    int d = 4;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("d", d);
    }

    if ((n * d) % 2 == 1) {
      if (pluginProgress) {
        pluginProgress->setError("(n * d) must be even");
      }
      return false;
    }

    ogdf::randomRegularGraph(G, n, d);
    return true;
  }
};

PLUGIN(OGDFRandomRegularGraph)
