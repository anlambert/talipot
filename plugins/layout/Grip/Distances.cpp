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

#include "Distances.h"

using namespace tlp;
using namespace std;

uint getDist(Graph *g, node n1, node n2) {
  vector<node> nextNodes;
  flat_hash_map<node, uint> nodeDepth;
  MutableContainer<bool> alreadyTreated;
  bool found = false;
  alreadyTreated.setAll(false);
  nextNodes.push_back(n1);
  nodeDepth[n1] = 0;
  alreadyTreated.set(n1.id, true);

  for (uint i = 0; !found && i < nextNodes.size(); ++i) {
    node current = nextNodes[i];

    for (auto v : g->getInOutNodes(current)) {

      if (alreadyTreated.get(v.id)) {
        continue;
      }

      alreadyTreated.set(v.id, true);
      nextNodes.push_back(v);
      nodeDepth[v] = nodeDepth[current] + 1;

      if (v == n2) {
        found = true;
        break;
      }
    }
  }

  return nodeDepth[n2];
}
