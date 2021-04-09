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

#include "StrongComponent.h"

PLUGIN(StrongComponent)

using namespace std;
using namespace tlp;

int StrongComponent::attachNumerotation(tlp::node n, std::unordered_map<tlp::node, bool> &visited,
                                        std::unordered_map<tlp::node, bool> &finished,
                                        std::unordered_map<tlp::node, int> &minAttach, int &id,
                                        std::stack<tlp::node> &renum, int &curComponent) {
  if (visited[n]) {
    return minAttach[n];
  }

  visited[n] = true;
  int myId = id;
  id++;
  minAttach[n] = myId;
  renum.push(n);
  int res = myId;

  for (auto tmpN : graph->getOutNodes(n)) {

    if (!finished[tmpN]) {
      int tmp = attachNumerotation(tmpN, visited, finished, minAttach, id, renum, curComponent);

      if (res > tmp) {
        res = tmp;
      }
    }
  }

  minAttach[n] = res;

  if (res == myId) {
    while (renum.top() != n) {
      node tmp = renum.top();
      renum.pop();
      finished[tmp] = true;
      minAttach[tmp] = res;
      result->setNodeValue(tmp, curComponent);
    }

    finished[n] = true;
    result->setNodeValue(n, curComponent);
    curComponent++;
    renum.pop();
  }

  return res;
}

StrongComponent::StrongComponent(const tlp::PluginContext *context) : DoubleAlgorithm(context) {}

StrongComponent::~StrongComponent() = default;

bool StrongComponent::run() {
  std::unordered_map<node, bool> visited(graph->numberOfNodes());
  std::unordered_map<node, bool> finished(graph->numberOfNodes());
  stack<node> renum;
  std::unordered_map<node, int> cachedValues(graph->numberOfNodes());
  int id = 1;
  int curComponent = 0;

  for (auto n : graph->nodes()) {
    if (!visited[n]) {
      attachNumerotation(n, visited, finished, cachedValues, id, renum, curComponent);
    }
  }

  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);

    if (result->getNodeValue(src) == result->getNodeValue(tgt)) {
      result->setEdgeValue(e, result->getNodeValue(src));
    } else {
      result->setEdgeValue(e, curComponent);
    }
  }

  return true;
}
