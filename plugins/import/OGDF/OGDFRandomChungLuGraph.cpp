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

#include <numeric>

#include <ogdf/basic/graph_generators/randomized.h>

#include "OGDFImportBase.h"

using namespace std;

static constexpr string_view paramHelp[] = {
    // expectedDegreeDistribution
    "a list of expected degrees, or weights, for the individual nodes. Its length defines the "
    "number of nodes n.",
};

//=================================================================================

class OGDFRandomChungLuGraph : public OGDFImportBase {
public:
  PLUGININFORMATION(
      "Random Chung Lu Graph (OGDF)", "Antoine Lambert", "06/2024",
      "Creates a graph where edges are inserted based on given weights. Implements the algorithm "
      "described in: \"The average distance in a random graph with given expected degrees, Fang "
      "Chung and Linyuan Lu, https://www.math.ucsd.edu/~fan/wp/aveflong.pdf\". Given an expected "
      "degree distribution of length n: (w_1, ..., w_n) with 0 < w_k < n. Let S be the sum over "
      "all expected degrees. Consider each edge independently and insert it with probability "
      "p_{ij} = (w_i * w_j) / S. Each degree must be strictly between 0 and n, and the square of "
      "the maximal expected degree must be lower than the sum of all expected degrees.",
      "1.0", "OGDF")

  OGDFRandomChungLuGraph(tlp::PluginContext *context) : OGDFImportBase(context) {
    addInParameter<vector<int>>("expectedDegreeDistribution", paramHelp[0].data(),
                                "(1, 2, 2, 3, 3, 3, 4)");
  }

  bool importOGDFGraph() override {
    vector<int> expectedDegreeDistribution = {1, 2, 2, 3, 3, 3, 4};

    if (dataSet != nullptr) {
      dataSet->get("expectedDegreeDistribution", expectedDegreeDistribution);
    }

    int S = reduce(expectedDegreeDistribution.begin(), expectedDegreeDistribution.end());

    for (auto w : expectedDegreeDistribution) {
      if (w <= 0 || w >= expectedDegreeDistribution.size()) {
        pluginProgress->setError("each degree must be strictly between 0 and n");
        return false;
      }
      if (w * w >= S) {
        pluginProgress->setError("the square of the maximal expected degree must be lower than the "
                                 "sum of all expected degrees");
        return false;
      }
    }

    ogdf::randomChungLuGraph(*G, vectorToOGDFArray(expectedDegreeDistribution));
    return true;
  }
};

PLUGIN(OGDFRandomChungLuGraph)
