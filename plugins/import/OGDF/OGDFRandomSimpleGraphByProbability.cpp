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
    // pEdge
    "the probability for each edge to be added into the graph (must be in [0, 1])",
};

//=================================================================================

class OGDFRandomSimpleGraphByProbability : public OGDFImportBase {
public:
  PLUGININFORMATION(
      "Random Simple Graph By Probability (OGDF)", "Antoine Lambert", "06/2024",
      "Creates a random simple graph. Algorithm based on PreZER/LogZER from:"
      "Sadegh Nobari, Xuesong Lu, Panagiotis Karras, and Stéphane Bressan. 2011. Fast random graph "
      "generation. In Proceedings of the 14th International Conference on Extending Database "
      "Technology (EDBT/ICDT '11),ACM, New York, NY, USA, 331-342. "
      "DOI=http://dx.doi.org/10.1145/1951365.1951406",
      "1.0", "OGDF")

  OGDFRandomSimpleGraphByProbability(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<int>("n", paramHelp[0].data(), "100");
    addInParameter<double>("pEdge", paramHelp[1].data(), "0.25");
  }

  bool importOGDFGraph() override {
    int n = 100;
    double pEdge = 0.25;

    if (dataSet != nullptr) {
      dataSet->get("n", n);
      dataSet->get("pEdge", pEdge);
    }

    if (pEdge < 0 || pEdge > 1) {
      if (pluginProgress) {
        pluginProgress->setError("pEdge must be in [0, 1]");
      }
      return false;
    }

    ogdf::randomSimpleGraphByProbability(*G, n, pEdge);
    return true;
  }
};

PLUGIN(OGDFRandomSimpleGraphByProbability)
