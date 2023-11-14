/**
 *
 * Copyright (C) 2023-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <ogdf/basic/graph_generators/randomGeographicalThresholdGraph.h>

#include <random>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes in the graph",

    // threshold
    "threshold for edge insertion",
};

//=================================================================================

class OGDFRandomGeographicalGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Geographical Graph (OGDF)", "Antoine Lambert", "11/2023",
                    "Creates a random geometric graph where edges are created based on "
                    "their distance and the weight of nodes",
                    "1.0", "OGDF")

  OGDFRandomGeographicalGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "100");
    addInParameter<double>("threshold", paramHelp[1].data(), "0.7");
  }

  bool importOGDFGraph() override {
    int n = 100;
    double threshold = 0.7;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("threshold", threshold);
    }

    ogdf::Array<int> weights = ogdf::Array<int>(n);
    for (int &w : weights) {
      w = tlp::randomNumber(n);
    }
    uniform_int_distribution<int> dist(0, n);
    ogdf::randomGeographicalThresholdGraph(G, weights, dist, 0.7);
    return true;
  }
};

PLUGIN(OGDFRandomGeographicalGraph)
