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

#include <ogdf/basic/graph_generators/deterministic.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // n
    "the number of nodes of the tree",

    // children
    "the number of children per node, if number of nodes does not allow a regular node, the last "
    "node will have fewer children",
};

//=================================================================================

class OGDFRegularTree : public OGDFImportBase {
public:
  PLUGININFORMATION("Regular Tree (OGDF)", "Antoine Lambert", "03/2024",
                    "Generates a regular tree where each node has the same number of children.",
                    "1.0", "OGDF")

  OGDFRegularTree(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "106");
    addInParameter<int>("children", paramHelp[1].data(), "5");
  }

  bool importOGDFGraph() override {
    int n = 106;
    int children = 5;
    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("children", children);
    }
    ogdf::regularTree(G, n, children);
    return true;
  }

private:
};

PLUGIN(OGDFRegularTree)
