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

#include "StrongComponents.h"

PLUGIN(StrongComponents)

using namespace std;
using namespace tlp;

int StrongComponents::attachNumerotation(tlp::node n, flat_hash_map<tlp::node, bool> &visited,
                                         flat_hash_map<tlp::node, bool> &finished,
                                         flat_hash_map<tlp::node, int> &minAttach, int &id,
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

StrongComponents::StrongComponents(const tlp::PluginContext *context) : DoubleAlgorithm(context) {}

bool StrongComponents::run() {
  flat_hash_map<node, bool> visited(graph->numberOfNodes());
  flat_hash_map<node, bool> finished(graph->numberOfNodes());
  stack<node> renum;
  flat_hash_map<node, int> cachedValues(graph->numberOfNodes());
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
