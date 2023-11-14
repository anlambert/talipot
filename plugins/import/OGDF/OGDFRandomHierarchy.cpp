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

#include <ogdf/basic/graph_generators/randomHierarchy.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes",
    // m
    "the number of edges",
    // planar
    "determines if the resulting graph is (level-)planar",
    // singleSource
    "determines if the graph is a single-source graph",
    // longEdges
    "determines if the graph has long edges (spanning 2 layers or more); "
    "otherwise the graph is proper"};

//=================================================================================

class OGDFRandomHierarchy : public OGDFImportBase {
public:
  PLUGININFORMATION("Random Hierarchy (OGDF)", "Antoine Lambert", "02/2024",
                    "Creates a random hierarchical graph", "1.0", "OGDF")

  OGDFRandomHierarchy(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "1000");
    addInParameter<int>("m", paramHelp[1].data(), "2000");
    addInParameter<bool>("planar", paramHelp[2].data(), "true");
    addInParameter<bool>("singleSource", paramHelp[3].data(), "true");
    addInParameter<bool>("longEdges", paramHelp[4].data(), "false");
  }

  bool importOGDFGraph() override {
    int n = 100;
    int m = 100;
    bool planar = false;
    bool singleSource = false;
    bool longEdges = true;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("m", m);
      dataSet->get("planar", planar);
      dataSet->get("singleSource", singleSource);
      dataSet->get("longEdges", longEdges);
    }

    ogdf::randomHierarchy(G, n, m, planar, singleSource, longEdges);
    return true;
  }
};

PLUGIN(OGDFRandomHierarchy)
