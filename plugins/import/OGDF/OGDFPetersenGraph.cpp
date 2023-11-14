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

#include <ogdf/basic/graph_generators/deterministic.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes on the outer cycle",

    // m
    "the length of jumps for the inner part",
};

//=================================================================================

class OGDFPetersenGraph : public OGDFImportBase {
public:
  PLUGININFORMATION(
      "Petersen Graph (OGDF)", "Antoine Lambert", "11/2023",
      "Creates an outer cycle of nodes 1, ..., n, each of which has a direct neighbor (a "
      "corresponding inner node). For two outer nodes i, j, there is an edge between their "
      "corresponding inner nodes if the absolute difference of i and j equals the jump length m.",
      "1.0", "OGDF")

  OGDFPetersenGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "5");
    addInParameter<int>("m", paramHelp[1].data(), "2");
  }

  bool importOGDFGraph() override {
    int n = 5;
    int m = 2;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("m", m);
    }

    petersenGraph(G, n, m);
    return true;
  }
};

PLUGIN(OGDFPetersenGraph)
