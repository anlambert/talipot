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

#include <talipot/GraphMeasure.h>
#include <talipot/Dijkstra.h>

using namespace std;
using namespace tlp;

//================================================================
uint tlp::maxDistance(const Graph *graph, uint nPos, tlp::NodeVectorProperty<uint> &distance,
                      EDGE_TYPE direction) {
  deque<uint> fifo;
  distance.setAll(UINT_MAX);
  fifo.push_back(nPos);
  distance[nPos] = 0;
  const std::vector<node> &nodes = graph->nodes();
  uint maxDist = 0;

  while (!fifo.empty()) {
    uint curPos = fifo.front();
    fifo.pop_front();
    uint nDist = distance[curPos] + 1;

    for (auto n : getAdjacentNodesIterator(graph, nodes[curPos], direction)) {
      nPos = graph->nodePos(n);
      if (distance[nPos] == UINT_MAX) {
        fifo.push_back(nPos);
        distance[nPos] = nDist;
        maxDist = std::max(maxDist, nDist);
      }
    }
  }

  return maxDist;
}
//================================================================
double tlp::maxDistance(const Graph *graph, const uint nPos,
                        tlp::NodeVectorProperty<double> &distance,
                        const NumericProperty *const weights, EDGE_TYPE direction) {
  if (!weights) {
    NodeVectorProperty<uint> dist_int(graph);
    dist_int.setAll(0);
    uint res = maxDistance(graph, nPos, dist_int, direction);
    for (auto n : graph->getNodes()) {
      distance[n] = double(dist_int[n]);
    }
    return double(res);
  }

  EdgeVectorProperty<double> eWeights(graph);
  eWeights.copyFromNumericProperty(weights);

  std::stack<node> queueNode;
  MutableContainer<int> nb_paths;
  Dijkstra dijkstra(graph, graph->nodes()[nPos], eWeights, distance, direction, &queueNode,
                    &nb_paths);
  // compute max distance from graph->nodes()[nPos]
  // by taking first reachable node in the queue
  while (!queueNode.empty()) {
    node n = queueNode.top();
    queueNode.pop();
    if (nb_paths.get(n.id) > 0) {
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

  TLP_PARALLEL_MAP_INDICES(nbNodes, [&](uint i) {
    tlp::NodeVectorProperty<uint> distance(graph);
    maxDistance(graph, i, distance, UNDIRECTED);

    double tmp_result = 0;

    for (uint j = 0; j < nbNodes; ++j) {
      if (j == i) {
        continue;
      }

      uint d = distance[j];
      if (d != UINT_MAX) {
        tmp_result += d;
      }
    }
    TLP_LOCK_SECTION(SUMPATH) {
      result += tmp_result;
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

  TLP_MAP_NODES_AND_INDICES(graph, [&](node n, uint i) {
    std::unordered_map<node, bool> reachables;
    markReachableNodes(graph, n, reachables, maxDepth);
    double nbEdge = 0; // e(N_v)*2$

    auto ite = reachables.end();

    for (const auto &[itn, reachable] : reachables) {
      for (auto e : graph->incidence(itn)) {
        auto [eSrc, eTgt] = graph->ends(e);

        if ((reachables.find(eSrc) != ite) && (reachables.find(eTgt) != ite)) {
          ++nbEdge;
        }
      }
    }

    double nNode = reachables.size();

    if (reachables.size() > 1) {
      clusters[i] = nbEdge / (nNode * (nNode - 1));
    } else {
      clusters[i] = 0;
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
    uint curLevel = level[current] + 1;
    for (auto child : graph->getOutNodes(current)) {
      uint childPos = graph->nodePos(child);
      uint childLevel = totreat[childPos];

      if (childLevel > 0) {
        totreat[childPos] = childLevel - 1;
      } else {
        level[childPos] = curLevel;
        fifo.push_back(child);
      }
    }
  }
}

//==================================================
void tlp::degree(const Graph *graph, tlp::NodeVectorProperty<double> &deg, EDGE_TYPE direction,
                 NumericProperty *weights, bool norm) {
  uint nbNodes = graph->numberOfNodes();

  if (!weights) {
    if (!norm) {
      switch (direction) {
      case UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph,
                                           [&](const node n, uint i) { deg[i] = graph->deg(n); });

        break;

      case INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph,
                                           [&](const node n, uint i) { deg[i] = graph->indeg(n); });
        break;

      case DIRECTED:
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
      case UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->deg(n); });
        break;

      case INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->indeg(n); });
        break;

      case DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(
            graph, [&](const node n, uint i) { deg[i] = normalization * graph->outdeg(n); });
        break;
      }
    }
  } else {
    if (!norm) {
      switch (direction) {
      case UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->incidence(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight;
        });
        break;

      case INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getInEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight;
        });

        break;

      case DIRECTED:
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
      case UNDIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->incidence(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight * normalization;
        });

        break;

      case INV_DIRECTED:
        TLP_PARALLEL_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
          double nWeight = 0.0;
          for (auto e : graph->getInEdges(n)) {
            nWeight += weights->getEdgeDoubleValue(e);
          }
          deg[i] = nWeight * normalization;
        });

        break;

      case DIRECTED:
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
