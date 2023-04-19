/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "Dijkstra.h"

using namespace tlp;
using namespace std;

//============================================================
void Dijkstra::initDijkstra(const tlp::Graph *const forbidden, tlp::node src,
                            const EdgeVectorProperty<double> &weights, const set<node> &focus) {

  assert(src.isValid());
  this->src = src;

  forbiddenNodes.setAll(false);

  if (forbidden) {
    for (auto n : forbidden->nodes()) {
      forbiddenNodes[n] = true;
    }
  }

  usedEdges.setAll(false);

  set<DijkstraElement *, LessDijkstraElement> dijkstraTable;
  set<DijkstraElement *, LessDijkstraElement> focusTable;

  mapDik.setAll(nullptr);
  NodeVectorProperty<bool> focusNode(graph);
  focusNode.setAll(false);

  for (auto n : focus) {
    focusNode[n] = true;
  }

  for (auto n : graph->nodes()) {
    if (n != src) { // init all nodes to +inf
      auto *tmp = new DijkstraElement(DBL_MAX / 2. + 10., node(), n);
      dijkstraTable.insert(tmp);

      if (focusNode[n]) {
        focusTable.insert(tmp);
      }

      mapDik[n] = tmp;
    } else { // init starting node to 0
      auto *tmp = new DijkstraElement(0, n, n);
      dijkstraTable.insert(tmp);
      mapDik[n] = tmp;
    }
  }

  nodeDistance.setAll(DBL_MAX);
  nodeDistance[src] = 0;

  while (!dijkstraTable.empty()) {
    // select the first element in the list the one with min value
    auto it = dijkstraTable.begin();
    DijkstraElement &u = *(*it);
    dijkstraTable.erase(it);

    if (!focusTable.empty()) {
      auto it = focusTable.rbegin();
      double maxDist = (*it)->dist;

      if (u.dist > maxDist) {
        break;
      }
    }

    node n = u.n;
    if (forbiddenNodes[n] && n != src) {
      continue;
    }

    for (auto e : graph->incidence(n)) {
      node v = graph->opposite(e, n);
      DijkstraElement &dEle = *mapDik[v];

      auto eWeight = weights[e];
      if (fabs((u.dist + eWeight) - dEle.dist) < 1E-9) { // path of the same length
        dEle.usedEdge.push_back(e);
      } else if ((u.dist + eWeight) < dEle.dist) {
        // we find a node closer with that path
        dEle.usedEdge.clear();
        //**********************************************
        dijkstraTable.erase(&dEle);

        if (focusNode[dEle.n]) {
          focusTable.erase(&dEle);
        }

        dEle.dist = u.dist + eWeight;
        dEle.previous = n;
        dEle.usedEdge.push_back(e);
        dijkstraTable.insert(&dEle);

        if (focusNode[dEle.n]) {
          focusTable.insert(&dEle);
        }
      }
    }
  }

  for (auto n : graph->nodes()) {
    DijkstraElement *dEle = mapDik[n];
    nodeDistance[n] = dEle->dist;

    for (auto e : dEle->usedEdge) {
      usedEdges[e] = true;
    }

    delete dEle;
  }

  resultNodes.setAll(false);
  resultEdges.setAll(false);
}
//=======================================================================
void Dijkstra::searchPaths(node n, EdgeVectorProperty<uint> &depth) {

  if (resultNodes[n]) {
    return;
  }

  resultNodes[n] = true;

  for (auto e : graph->incidence(n)) {

    if (!usedEdges[e] || resultEdges[e]) {
      continue;
    }

    node tgt = graph->opposite(e, n);

    if (nodeDistance[tgt] >= nodeDistance[n]) {
      continue;
    }

    resultEdges[e] = true;
    depth[e] += 1;

    if (!resultNodes[tgt]) {
      searchPaths(tgt, depth);
    }
  }
}
//=============================================================================
void Dijkstra::searchPath(node n, vector<node> &vNodes) {

  node tgte(n);
  resultEdges.setAll(false);
  bool ok = true;

  while (ok) {
    vNodes.push_back(n);
    ok = false;

    for (auto e : graph->incidence(n)) {
      // check if that edge does not belong to the shortest path edges
      // or if it has already been treated
      if (!usedEdges[e] || resultEdges[e]) {
        continue;
      }

      node tgt = graph->opposite(e, n);

      if (nodeDistance[tgt] >= nodeDistance[n]) {
        continue;
      }

      n = tgt;
      resultEdges[e] = ok = true;
      break;
    }
  }

  if (n != src) {
    cout << "A path does not exist between node " << src.id << " and node " << tgte.id << "!"
         << endl;
  }
}
