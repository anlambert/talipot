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
    // signature
    "contains the positive values k1, k2, ..., kn.",
};

//=================================================================================

class OGDFCompleteKPartiteGraph : public OGDFImportBase {
public:
  PLUGININFORMATION("Complete K-partite Graph (OGDF)", "Antoine Lambert", "05/2024",
                    "Creates the complete k-partite graph K_{k1,k2,...,kn}.", "1.0", "OGDF")

  OGDFCompleteKPartiteGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<vector<int>>("signature", paramHelp[0].data(), "(10, 20, 30, 40)");
  }

  bool importOGDFGraph() override {
    vector<int> signature = {10, 20, 30, 40};
    if (dataSet != nullptr) {
      dataSet->get("signature", signature);
    }
    ogdf::completeKPartiteGraph(*G, vectorToOGDFArray(signature));
    return true;
  }
};

PLUGIN(OGDFCompleteKPartiteGraph)
