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

#ifndef TALIPOT_DIJKSTRA_H
#define TALIPOT_DIJKSTRA_H

#include <vector>
#include <stack>
#include <list>
#include <talipot/hash.h>
#include <climits>
#include <functional>
#include <talipot/Graph.h>
#include <talipot/BooleanProperty.h>
#include <talipot/VectorProperty.h>
#include <talipot/MutableContainer.h>
#include <talipot/GraphTools.h>

namespace tlp {

class Dijkstra {
public:
  //============================================================
  Dijkstra(const Graph *const graph, node src, const EdgeVectorProperty<double> &weights,
           NodeVectorProperty<double> &nodeDistance, EdgeType direction,
           std::stack<node> *qN = nullptr, MutableContainer<int> *nP = nullptr);
  //========================================================
  bool searchPaths(node n, BooleanProperty *result);
  //=========================================================
  bool searchPath(node n, BooleanProperty *result);
  //=============================================================
  bool ancestors(flat_hash_map<node, std::list<node>> &result);

private:
  void internalSearchPaths(node n, BooleanProperty *result);
  //=========================================================
  struct DijkstraElement {
    DijkstraElement(const double dist = DBL_MAX, const node previous = node(),
                    const node n = node())
        : dist(dist), previous(previous), n(n) {}
    bool operator==(const DijkstraElement &b) const {
      return n == b.n;
    }
    bool operator!=(const DijkstraElement &b) const {
      return n != b.n;
    }
    double dist;
    node previous;
    node n;
    std::vector<edge> usedEdge;
  };

  struct LessDijkstraElement {
    bool operator()(const DijkstraElement *const a, const DijkstraElement *const b) const {
      if (fabs(a->dist - b->dist) > 1.E-9) {
        return (a->dist < b->dist);
      } else {
        return (a->n.id < b->n.id);
      }
    }
  };

  Graph const *graph;
  node src;
  MutableContainer<bool> usedEdges;
  NodeVectorProperty<double> &nodeDistance;
  std::stack<node> *queueNodes;
  MutableContainer<int> *numberOfPaths;
};
}

#endif // TALIPOT_DIJKSTRA_H
