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

#include <ogdf/basic/graph_generators/randomized.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes",
    // m
    "the number of edges",
};

//=================================================================================

class OGDFRandomSimpleGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Simple Graph (OGDF)", "Antoine Lambert", "06/2024",
                    "Creates a random simple graph", "1.0", "OGDF")

  OGDFRandomSimpleGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "500");
    addInParameter<int>("m", paramHelp[1].data(), "1500");
  }

  bool importOGDFGraph() override {
    int n = 500;
    int m = 1500;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("m", m);
    }

    ogdf::randomSimpleGraph(*G, n, m);
    return true;
  }
};

PLUGIN(OGDFRandomSimpleGraph)
