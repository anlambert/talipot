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

class OGDFRandomGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Graph (OGDF)", "Antoine Lambert", "06/2024", "Creates a random graph",
                    "1.0", "OGDF")

  OGDFRandomGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "1000");
    addInParameter<int>("m", paramHelp[1].data(), "2000");
  }

  bool importOGDFGraph() override {
    int n = 1000;
    int m = 2000;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("m", m);
    }

    ogdf::randomGraph(*G, n, m);
    return true;
  }
};

PLUGIN(OGDFRandomGraph)
