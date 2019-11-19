/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ConnectedComponent.h"
#include <talipot/ConnectedTest.h>

PLUGIN(ConnectedComponent)

using namespace tlp;

ConnectedComponent::ConnectedComponent(const tlp::PluginContext *context)
    : DoubleAlgorithm(context) {}
//======================================================
bool ConnectedComponent::run() {
  std::vector<std::vector<node>> components;
  ConnectedTest::computeConnectedComponents(graph, components);

  // assign the index of each component as value for its nodes
  for (unsigned curComponent = 0; curComponent < components.size(); ++curComponent) {
    std::vector<node> &component = components[curComponent];

    for (auto n : component) {
      result->setNodeValue(n, curComponent);
    }
  }

  // propagate nodes computed value to edges
  for (auto e : graph->edges()) {
    result->setEdgeValue(e, result->getNodeValue(graph->source(e)));
  }

  if (dataSet != nullptr)
    dataSet->set<unsigned>("#connected components", components.size());

  return true;
}
//======================================================
