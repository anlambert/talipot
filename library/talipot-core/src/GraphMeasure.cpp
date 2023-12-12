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

#include <talipot/Dijkstra.h>
#include <talipot/GraphMeasure.h>

using namespace std;
using namespace tlp;

//================================================================
uint tlp::maxDistance(const Graph *graph, node n, tlp::NodeVectorProperty<uint> &distance,
                      EdgeType direction) {
  deque<node> fifo;
  distance.setAll(UINT_MAX);
  fifo.push_back(n);
  distance[n] = 0;
  uint maxDist = 0;

  while (!fifo.empty()) {
    node curNode = fifo.front();
    fifo.pop_front();
    uint nDist = distance[curNode] + 1;

    for (auto n : getAdjacentNodesIterator(graph, curNode, direction)) {
      if (distance[n] == UINT_MAX) {
        fifo.push_back(n);
        distance[n] = nDist;
        maxDist = std::max(maxDist, nDist);
      }
    }
  }

  return maxDist;
}
//================================================================
double tlp::maxDistance(const Graph *graph, node n, tlp::NodeVectorProperty<double> &distance,
                        const NumericProperty *const weights, EdgeType direction) {
  if (!weights) {
    NodeVectorProperty<uint> dist_int(graph);
    dist_int.setAll(0);
    uint res = maxDistance(graph, n, dist_int, direction);
    for (auto n : graph->nodes()) {
      distance[n] = double(dist_int[n]);
    }
    return double(res);
  }

  EdgeVectorProperty<double> eWeights(graph);
  eWeights.copyFromNumericProperty(weights);

  std::stack<node> queueNode;
  MutableContainer<int> nbPaths;
  Dijkstra dijkstra(graph, n, eWeights, distance, direction, &queueNode, &nbPaths);
  // compute max distance from graph->nodes()[nPos]
  // by taking first reachable node in the queue
  while (!queueNode.empty()) {
    node n = queueNode.top();
    queueNode.pop();
    if (nbPaths.get(n.id) > 0) {
      return distance[n];
    }
  }
  return 0.;
}
//================================================================
// Warning the algorithm is not optimal
double tlp::averagePathLength(const Graph *graph) {
  double result = 0;

  uint nbNodes = graph->numberOfNodes();

  if (nbNodes < 2) {
    return result;
  }

  TLP_PARALLEL_MAP_NODES(graph, [&](node n) {
    tlp::NodeVectorProperty<uint> distance(graph);
    maxDistance(graph, n, distance, EdgeType::UNDIRECTED);

    double tmp = 0;

    for (auto nn : graph->nodes()) {
      if (nn == n) {
        continue;
      }
      uint d = distance[nn];
      if (d != UINT_MAX) {
        tmp += d;
      }
    }

    TLP_LOCK_SECTION(SUMPATH) {
      result += tmp;
    }
    TLP_UNLOCK_SECTION(SUMPATH);
  });

  result /= (nbNodes * (nbNodes - 1.));
  return result;
}
//================================================================
double tlp::averageClusteringCoefficient(const Graph *graph) {
  tlp::NodeVectorProperty<double> clusters(graph);
  tlp::clusteringCoefficient(graph, clusters, UINT_MAX);
  uint nbNodes = graph->numberOfNodes();
  double sum = 0;

  for (uint i = 0; i < nbNodes; ++i) {
    sum += clusters[i];
  }

  return sum / nbNodes;
}
//================================================================
uint tlp::maxDegree(const Graph *graph) {
  uint maxdeg = 0;
  for (auto n : graph->nodes()) {
    maxdeg = std::max(maxdeg, graph->deg(n));
  }
  return maxdeg;
}
//================================================================
uint tlp::minDegree(const Graph *graph) {
  uint mindeg = graph->numberOfNodes();
  for (auto n : graph->nodes()) {
    mindeg = std::min(mindeg, graph->deg(n));
  }
  return mindeg;
}
//=================================================
void tlp::clusteringCoefficient(const Graph *graph, tlp::NodeVectorProperty<double> &clusters,
                                uint maxDepth) {

  TLP_PARALLEL_MAP_NODES(graph, [&](node n) {
    set<node> reachables = reachableNodes(graph, n, maxDepth);
    double nbEdges = graph->deg(n);
    for (const auto r : reachables) {
      for (auto e : graph->incidence(r)) {
        auto [eSrc, eTgt] = graph->ends(e);
        if (reachables.contains(eSrc) && reachables.contains(eTgt)) {
          ++nbEdges;
        }
      }
    }

    double nbNodes = reachables.size() + 1;
    if (nbNodes > 1) {
      clusters[n] = nbEdges / ((nbNodes * (nbNodes - 1)) / 2);
    } else {
      clusters[n] = 0;
    }
  });
}
//==================================================
void tlp::dagLevel(const Graph *graph, tlp::NodeVectorProperty<uint> &level) {
  tlp::NodeVectorProperty<uint> totreat(graph);
  deque<node> fifo;
  TLP_MAP_NODES_AND_INDICES(graph, [&](node n, uint i) {
    uint indegree = graph->indeg(n);

    if (indegree == 0) {
      fifo.push_back(n);
      level[i] = 0;
    } else {
      totreat[i] = indegree - 1;
    }
  });

  //==============================================
  while (!fifo.empty()) {
    node current = fifo.front();
    fifo.pop_front();
    uint curLevel = level.getNodeValue(current) + 1;
    for (auto child : graph->getOutNodes(current)) {
      uint childLevel = totreat[child];

      if (childLevel > 0) {
        totreat[child] = childLevel - 1;
      } else {
        level[child] = curLevel;
        fifo.push_back(child);
      }
    }
  }
}

//==================================================
void tlp::degree(const Graph *graph, tlp::NodeVectorProperty<double> &deg, EdgeType direction,
                 NumericProperty *weights, bool norm) {
  uint nbNodes = graph->numberOfNodes();

  if (!weights) {
    if (!norm) {
      switch (direction) {
      case EdgeType::UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph,
                                           [&](const node n, uint i) { deg[i] = graph->deg(n); });

        break;

      case EdgeType::INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph,
                                           [&](const node n, uint i) { deg[i] = graph->indeg(n); });
        break;

      case EdgeType::DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = graph->outdeg(n); });

        break;
      }
    } else {
      double normalization = 1.0;

      if (nbNodes > 1 && graph->numberOfEdges()) {
        normalization = 1. / (nbNodes - 1);
      }

      switch (direction) {
      case EdgeType::UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->deg(n); });
        break;

      case EdgeType::INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->indeg(n); });
        break;

      case EdgeType::DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->outdeg(n); });
        break;
      }
    }
  } else {
    if (!norm) {
      switch (direction) {
      case EdgeType::UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->incidence(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight;
        });
        break;

      case EdgeType::INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getInEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight;
        });

        break;

      case EdgeType::DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getOutEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight;
        });

        break;
      }
    } else {
      double normalization = 1.0;
      uint nbEdges = graph->numberOfEdges();

      if (nbNodes > 1 && nbEdges > 0) {
        double sum = 0;

        for (auto e : graph->edges()) {
          sum += fabs(weights->getEdgeDoubleValue(e));
        }

        normalization = (sum / nbEdges) * (nbNodes - 1);

        if (fabs(normalization) < 1E-9) {
          normalization = 1.0;
        } else {
          normalization = 1.0 / normalization;
        }
      }

      switch (direction) {
      case EdgeType::UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->incidence(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight * normalization;
        });

        break;

      case EdgeType::INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getInEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight * normalization;
        });

        break;

      case EdgeType::DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getOutEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight * normalization;
        });

        break;
      }
    }
  }
}
