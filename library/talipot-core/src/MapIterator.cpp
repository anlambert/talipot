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

#include <talipot/MapIterator.h>
#include <talipot/Graph.h>
using namespace std;
using namespace tlp;

edge tlp::nextFaceEdge(Graph *g, edge e, node n) {
  EdgeMapIterator it(g, e, n);
  edge result;

  if (it.hasNext()) {
    result = it.next();
  }

  return result;
}

NodeMapIterator::NodeMapIterator(Graph *sg, node source, node target) {
  assert(cloneIt.empty());
  bool start = true;

  for (auto tmp : sg->getInOutNodes(target)) {

    if ((start) && (tmp == source)) {
      start = false;
      itStl = cloneIt.begin();
    } else {
      if (start) {
        cloneIt.push_back(tmp);
      } else {
        cloneIt.insert(itStl, tmp);
      }
    }
  }

  itStl = cloneIt.begin();
}

NodeMapIterator::~NodeMapIterator() = default;

node NodeMapIterator::next() {
  node tmp = *itStl;
  ++itStl;
  return tmp;
}

bool NodeMapIterator::hasNext() {
  return (itStl != cloneIt.end());
}

//=========================================
EdgeMapIterator::EdgeMapIterator(const Graph *sg, edge source, node target) {
  adj.resize(sg->deg(target));
  finished = false;
  treat = 0;
  pos = 0;

  for (auto e : sg->incidence(target)) {
    if (e == source) {
      pos = treat + 1;
    }

    adj[treat++] = e;
  }
}
//=========================================

edge EdgeMapIterator::next() {
  --treat;
  pos %= adj.size();
  return adj[pos++];
}
//=========================================
bool EdgeMapIterator::hasNext() {
  return (treat > 0);
}
//=========================================
