/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ConnectedComponents.h"
#include <talipot/ConnectedTest.h>

PLUGIN(ConnectedComponents)

using namespace std;
using namespace tlp;

ConnectedComponents::ConnectedComponents(const tlp::PluginContext *context)
    : DoubleAlgorithm(context) {}
//======================================================
bool ConnectedComponents::run() {
  vector<vector<node>> components = ConnectedTest::computeConnectedComponents(graph);

  // assign the index of each component as value for its nodes
  uint curComponent = 0;
  for (const auto &component : components) {
    for (auto n : component) {
      result->setNodeValue(n, curComponent);
    }
    ++curComponent;
  }

  // propagate nodes computed value to edges
  for (auto e : graph->edges()) {
    result->setEdgeValue(e, result->getNodeValue(graph->source(e)));
  }

  if (dataSet != nullptr) {
    dataSet->set<unsigned>("#connected components", components.size());
  }

  return true;
}
//======================================================
