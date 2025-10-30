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
    "the number of nodes of the generated graph",

    // jumps
    "the array of distances for edges to be created",
};

//=================================================================================

class OGDFCirculantGraph : public OGDFImportBase {
public:
  PLUGININFORMATION(
      "Circulant Graph (OGDF)", "Antoine Lambert", "05/2024",
      "Generates a simple, undirected graph on n nodes V := v_0,v_1,...,v_{n-1} that contains "
      "exactly the edges {v_iv_{i+d}; v_i ∈ V, d ∈ jumps} where node indices are to be understood "
      "modulo n. The order of nodes induced by G is the sequence V previously given.",
      "1.0", "OGDF")

  OGDFCirculantGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "100");
    addInParameter<vector<int>>("jumps", paramHelp[1].data(), "(10, 20, 30, 40)");
  }

  bool importOGDFGraph() override {
    int n = 100;
    vector<int> jumps = {10, 20, 30, 40};
    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("jumps", jumps);
    }
    ogdf::circulantGraph(*G, n, vectorToOGDFArray(jumps));
    return true;
  }
};

PLUGIN(OGDFCirculantGraph)
