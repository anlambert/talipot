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

#include "MISFiltering.h"

using namespace tlp;
using namespace std;

MISFiltering::MISFiltering(Graph *g) {
  g_copy = g;
  removedVisit.setAll(false);
  removed.setAll(false);
  attendedVisit.setAll(false);
  visited.setAll(false);
}
//========================================
MISFiltering::~MISFiltering() = default;
//========================================
void MISFiltering::computeFiltering() {
  node firstNode = g_copy->getOneNode();

  inLastVi.setAll(true);

  for (auto n : g_copy->nodes()) {
    levelToNodes[0].insert(n);
  }

  level = 1;
  uint nb = g_copy->numberOfNodes();

  while (nb > 3) {
    nb = 0;
    visited.setAll(false);
    removedVisit.setAll(false);
    attendedVisit.setAll(false);
    attendedVisit.set(firstNode.id, true);
    toVisit.clear();
    toVisit.push_back(firstNode);
    inCurVi.setAll(false);
    inCurVi.set(firstNode.id, true);

    uint depth = 2u << (level - 1u);

    for (auto current : toVisit) {
      if (removedVisit.get(current.id)) {
        continue;
      }

      visited.set(current.id, true);
      bfsDepth(current, depth);
    }

    inLastVi.setAll(false);

    for (uint id : inCurVi.findAll(true)) {
      node n(id);
      levelToNodes[level].insert(n);
      inLastVi.set(n.id, true);
      ++nb;
    }

    ++level;
    inCurVi.setAll(false);
    removed.setAll(false);
  }

  updateVectors();
}
//========================================
void MISFiltering::bfsDepth(node n, uint depth) {
  vector<node> nextNodes;
  flat_hash_map<node, uint> nodeDepth;
  MutableContainer<bool> inNext;
  inNext.setAll(false);
  inNext.set(n.id, true);
  nextNodes.push_back(n);
  nodeDepth[n] = 0;

  for (uint i = 0; i < nextNodes.size(); ++i) {
    node current = nextNodes[i];
    for (auto v : g_copy->getInOutNodes(current)) {
      if (visited.get(v.id) || inNext.get(v.id)) {
        continue;
      }

      if (nodeDepth[current] < depth - 1) {
        inNext.set(v.id, true);
        nextNodes.push_back(v);
        removed.set(v.id, true);
      }

      nodeDepth[v] = nodeDepth[current] + 1;

      if (nodeDepth[v] == depth && inLastVi.get(v.id) && !removed.get(v.id)) {
        if (!attendedVisit.get(v.id)) {
          toVisit.push_back(v);
          attendedVisit.set(v.id, true);
          inCurVi.set(v.id, true);
        }
      } else if (nodeDepth[v] != 0) {
        if (attendedVisit.get(v.id)) {
          removedVisit.set(v.id, true);
          inCurVi.set(v.id, false);
          removed.set(v.id, true);
        }
      }
    }
  }
}
//========================================
void MISFiltering::updateVectors() {
  ordering.resize(g_copy->numberOfNodes());

  if (level == 1) {
    uint curPos = 0;
    for (auto n : g_copy->nodes()) {
      ordering[curPos++] = n;
    }
    return;
  }

  MutableContainer<bool> considered;
  considered.setAll(false);
  uint curPos = 0;

  while (level + 1) {
    auto it = levelToNodes[level].begin();

    while (it != levelToNodes[level].end()) {
      node n = *it;
      ++it;

      if (considered.get(n.id)) {
        continue;
      }

      ordering[curPos] = n;
      ++curPos;
      considered.set(n.id, true);
    }

    if (level != 0) {
      index.push_back(curPos);
    } else {
      break;
    }

    --level;
  }

  if (index[0] == 3) {
    return;
  }

  if (index.size() == 1) {
    index[0] = 3;
  } else {
    if (index[1] > 3) {
      index[0] = 3;
    } else { // index[1]<=3
      index.erase(index.begin());

      if (index.size() >= 2) {
        if (index[1] > 3) {
          index[0] = 3;
        } else {
          index.erase(index.begin());
        }
      } else {
        index[0] = 3;
      }
    }
  }
}
//========================================
void MISFiltering::getNearest(node n, vector<node> &neighbors, vector<uint> &neighbors_dist,
                              uint level, uint nbNeighbors) {
  vector<node> nextNodes;
  MutableContainer<bool> alreadyTreated;
  MutableContainer<bool> toTreat;
  flat_hash_map<node, uint> nodeDepth;
  bool found = false;
  unsigned nbFound = 0;

  neighbors_dist.clear();
  neighbors.clear();
  nodeDepth[n] = 0;
  alreadyTreated.setAll(false);
  toTreat.setAll(false);
  nextNodes.push_back(n);
  alreadyTreated.set(n.id, true);

  for (uint i = 0; i < index[level + 1]; ++i) {
    toTreat.set(ordering[i].id, true);
  }

  for (uint i = 0; !found && i < nextNodes.size(); ++i) {
    node current = nextNodes[i];

    for (auto v : g_copy->getInOutNodes(current)) {

      if (alreadyTreated.get(v.id)) {
        continue;
      }

      alreadyTreated.set(v.id, true);
      nodeDepth[v] = nodeDepth[current] + 1;
      nextNodes.push_back(v);

      if (toTreat.get(v.id)) {
        neighbors.push_back(v);
        neighbors_dist.push_back(nodeDepth[v]);
        ++nbFound;
      }

      if (nbFound == nbNeighbors) {
        found = true;
        break;
      }
    }
  }
}
//========================================
