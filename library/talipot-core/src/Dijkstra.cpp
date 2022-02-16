/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Dijkstra.h>

using namespace tlp;
using namespace std;

//============================================================
Dijkstra::Dijkstra(const Graph *const graph, node src, const EdgeVectorProperty<double> &weights,
                   NodeVectorProperty<double> &nd, EdgeType direction, std::stack<node> *qN,
                   MutableContainer<int> *nP)
    : nodeDistance(nd), queueNodes(qN), numberOfPaths(nP) {
  assert(src.isValid());
  this->graph = graph;
  usedEdges.setAll(false);
  this->src = src;
  set<DijkstraElement *, LessDijkstraElement> dijkstraTable;
  NodeVectorProperty<DijkstraElement *> mapDik(graph);
  mapDik.setAll(nullptr);
  if (queueNodes) {
    while (!queueNodes->empty()) {
      queueNodes->pop();
    }
  }
  if (numberOfPaths) {
    numberOfPaths->setAll(0);
    numberOfPaths->set(this->src.id, 1);
  }

  uint i = 0;
  for (auto n : graph->nodes()) {
    DijkstraElement *tmp;
    if (n != src) {
      // init all nodes to +inf
      tmp = new DijkstraElement(DBL_MAX / 2. + 10., node(), n);
    } else {
      // init starting node to 0
      tmp = new DijkstraElement(0, n, n);
    }

    dijkstraTable.insert(tmp);

    mapDik[i++] = tmp;
  }

  while (!dijkstraTable.empty()) {
    // select the first element in the list the one with min value
    auto it = dijkstraTable.begin();
    DijkstraElement &u = *(*it);
    dijkstraTable.erase(it);
    if (queueNodes) {
      queueNodes->push(u.n);
    }

    for (auto e : getIncidentEdgesIterator(graph, u.n, direction)) {
      node v = graph->opposite(e, u.n);
      auto *dEle = mapDik[v];
      double eWeight = weights.getEdgeValue(e);
      assert(eWeight > 0);

      if (fabs((u.dist + eWeight) - dEle->dist) < 1E-9) { // path of the same length
        dEle->usedEdge.push_back(e);
        if (numberOfPaths) {
          numberOfPaths->set(v.id, numberOfPaths->get(v.id) + numberOfPaths->get(u.n.id));
        }
      } else if ((u.dist + eWeight) < dEle->dist) {
        // we find a node closer with that path
        dEle->usedEdge.clear();
        //**********************************************
        dijkstraTable.erase(dEle);

        dEle->dist = u.dist + eWeight;
        dEle->previous = u.n;
        dEle->usedEdge.push_back(e);
        dijkstraTable.insert(dEle);
        if (numberOfPaths) {
          numberOfPaths->set(v.id, numberOfPaths->get(u.n.id));
        }
      }
    }
  }

  usedEdges.setAll(false);
  i = 0;
  for (auto n : graph->nodes()) {
    DijkstraElement *dEle = mapDik[i++];
    nodeDistance[n] = dEle->dist;

    for (auto e : dEle->usedEdge) {
      usedEdges.set(e.id, true);
    }

    delete dEle;
  }
}
//=============================================================================
bool Dijkstra::searchPath(node n, BooleanProperty *result) {
  bool ok = true;

  while (ok) {
    result->setNodeValue(n, true);
    ok = false;
    for (auto e : graph->incidence(n)) {
      if (!usedEdges.get(e.id)) {
        continue; // edge does not belong to the shortest path
      }

      if (result->getEdgeValue(e)) {
        continue; // edge already treated
      }

      node tgt = graph->opposite(e, n);

      if (nodeDistance[tgt] >= nodeDistance[n]) {
        continue;
      }

      n = tgt;
      result->setEdgeValue(e, true);
      ok = true;
      break;
    }
  }

  if (n != src) {
#ifndef NDEBUG
    cout << "Path does not exist !" << endl;
#endif /* NDEBUG */
    result->setAllNodeValue(false);
    result->setAllEdgeValue(false);
    return false;
  }

  return true;
}
//=======================================================================
void Dijkstra::internalSearchPaths(node n, BooleanProperty *result) {
  result->setNodeValue(n, true);
  for (auto e : graph->incidence(n)) {
    if (!usedEdges.get(e.id)) {
      continue;
    }

    if (result->getEdgeValue(e)) {
      continue;
    }

    node tgt = graph->opposite(e, n);

    if (nodeDistance[tgt] >= nodeDistance[n]) {
      continue;
    }

    result->setEdgeValue(e, true);
    if (!result->getNodeValue(tgt)) {
      internalSearchPaths(tgt, result);
    }
  }
}
//========================================
bool Dijkstra::searchPaths(node n, BooleanProperty *result) {
  internalSearchPaths(n, result);
  if (!result->getNodeValue(src)) {
    result->setAllNodeValue(false);
    result->setAllEdgeValue(false);
    return false;
  }

  return true;
}

//========================================
bool Dijkstra::ancestors(unordered_map<node, std::list<node>> &result) {
  result.clear();
  result[src].push_back(src);
  for (auto n : graph->getNodes()) {
    if (n != src) {
      for (auto e : graph->incidence(n)) {
        node tgt = graph->opposite(e, n);
        if (usedEdges.get(e.id) && nodeDistance[tgt] < nodeDistance[n]) {
          result[n].push_back(tgt);
        }
      }
    }
  }
  return true;
}
