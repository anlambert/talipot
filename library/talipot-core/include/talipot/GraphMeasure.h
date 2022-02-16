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

#ifndef TALIPOT_GRAPH_MEASURE_H
#define TALIPOT_GRAPH_MEASURE_H

#include <set>

#include <talipot/Node.h>
#include <talipot/VectorProperty.h>
#include <talipot/GraphTools.h>

namespace tlp {

class Graph;
class PluginProgress;
/**
 * returns the average path length of a graph, that is the sum
 * of the shortest distances for all pair of distinct nodes in that graph
 * divided by the number of those pairs. For a pair of non connected nodes,
 * the shorted distance is set to 0.
 * see http://en.wikipedia.org/wiki/Average_path_length for more details
 */
TLP_SCOPE double averagePathLength(const Graph *g);
/*
 * return the clustering coefficient of a graph
 * as the average of the local clustering coefficients
 * (see clusteringCoefficient function) of all the nodes.
 * see http://en.wikipedia.org/wiki/Clustering_coefficient for more details.
 */
TLP_SCOPE double averageClusteringCoefficient(const Graph *);
/*
 * assign to each node its local clustering coefficient
 * that is the proportion of edges between the nodes within its neighbourhood
 * divided by the number of edges that could possibly exist between them.
 * This quantifies how close its neighbors are to being a clique.
 * see http://en.wikipedia.org/wiki/Clustering_coefficient for more details
 */
TLP_SCOPE void clusteringCoefficient(const Graph *g, tlp::NodeVectorProperty<double> &result,
                                     uint maxDepth = 1);
/*
 * assign to each node of a graph its (in/ou/inout) degree.
 * The weighted degree of a node is the sum of weights of
 * all its in/out/inout edges."
 * If no metric is specified, using a uniform metric value of 1 for all edges
 * it assigns the usual degree of nodes (number of neighbors).",
 * If norm is true, the measure is normalized in the following way:
 * unweighted case => m(n) = deg(n) / (#V - 1)
 * weighted case => m(n) = deg_w(n) / [(sum(e_w)/#E)(#V - 1)]
 */
TLP_SCOPE void degree(const Graph *graph, tlp::NodeVectorProperty<double> &deg,
                      EdgeType direction = EdgeType::UNDIRECTED, NumericProperty *weights = nullptr,
                      bool norm = false);
/*
 * assign to each node of a Directed Acyclic Graph a level such that
 * if the edge e(u,v) exists level(u) < level(v) the algorithm ensure that
 * the number of level used is minimal.
 *
 * Warning: the graph must be acyclic (no self loops).
 */
TLP_SCOPE void dagLevel(const Graph *graph, tlp::NodeVectorProperty<uint> &level);
// return the maximum value of the degree of the graph's nodes
TLP_SCOPE uint maxDegree(const Graph *);
// return the minimum value of the degree of the graph's nodes
TLP_SCOPE uint minDegree(const Graph *);
/*
 * compute the maximum distance from the n (graph->nodes[nPos]) to all the other nodes of graph
 * and store it into distance, (stored value is UINT_MAX for non connected nodes),
 * if direction is set to EdgeType::UNDIRECTED use undirected graph, EdgeType::DIRECTED use directed
 * graph and EdgeType::INV_DIRECTED use reverse directed graph (ie. all edges are reversed) all the
 * edge's weight is set to 1. (it uses a bfs thus the complexity is o(m), m = |E|).
 */
TLP_SCOPE uint maxDistance(const Graph *graph, const uint nPos,
                           tlp::NodeVectorProperty<uint> &distance,
                           EdgeType direction = EdgeType::UNDIRECTED);

/*
 * compute the maximum distance from the n (graph->nodes[nPos]) to all the other nodes of graph
 * and store it into distance, (stored value is DBL_MAX for non connected nodes),
 * if direction is set to EdgeType::UNDIRECTED use undirected graph, EdgeType::DIRECTED use directed
 * graph and EdgeType::INV_DIRECTED use reverse directed graph (ie. all edges are reversed) Edge
 * weights can be given, Dijkstra's algorithm is then used (the complexity is then o((m + n)log n))
 * otherwise all the edge's weight is set to 1. (it uses a bfs thus the complexity is o(m), m =
 * |E|).
 */
TLP_SCOPE double maxDistance(const Graph *graph, const uint nPos,
                             tlp::NodeVectorProperty<double> &distance,
                             const NumericProperty *const weights,
                             EdgeType direction = EdgeType::UNDIRECTED);
}
#endif // TALIPOT_GRAPH_MEASURE_H
