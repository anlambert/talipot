/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <stack>

#include <talipot/AcyclicTest.h>
#include <talipot/GraphTools.h>

#include "LeafMetric.h"

PLUGIN(LeafMetric)

using namespace std;
using namespace tlp;

//=======================================================================
LeafMetric::LeafMetric(const PluginContext *context) : DoubleAlgorithm(context) {}
//=======================================================================
bool LeafMetric::run() {
  result->setAllNodeValue(0);
  for (auto n : reversed(dfs(graph, true))) {
    double val = 1.0;
    if (graph->outdeg(n) > 0) {
      val = iteratorReduce(graph->getOutNodes(n), 0.0, [this](double curVal, const node m) {
        return curVal + result->getNodeValue(m);
      });
    }
    result->setNodeValue(n, val);
  }
  return true;
}
//=======================================================================
bool LeafMetric::check(string &erreurMsg) {
  if (!AcyclicTest::isAcyclic(graph)) {
    erreurMsg = "The graph must be a acyclic.";
    return false;
  }

  return true;
}
//=======================================================================
