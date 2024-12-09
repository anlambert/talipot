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

#ifndef TALIPOT_GRAPH_TOOLS_H
#define TALIPOT_GRAPH_TOOLS_H

#include <map>
#include <list>
#include <set>
#include <stack>
#include <vector>
#include <talipot/hash.h>
#include <talipot/config.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>
#include <talipot/MutableContainer.h>
#include <talipot/VectorProperty.h>
#include <talipot/Iterator.h>

namespace tlp {
class BooleanProperty;
class DoubleProperty;
class Graph;
class IntegerProperty;
class NumericProperty;
class PlanarConMap;
class PluginProgress;

enum class EdgeType { UNDIRECTED = 0, INV_DIRECTED = 1, DIRECTED = 2 };
#define IN_EDGE EdgeType::INV_DIRECTED
#define OUT_EDGE EdgeType::DIRECTED
#define INOUT_EDGE EdgeType::UNDIRECTED

/**
 * Return a function to get an Iterator on the adjacent nodes of a graph node
 * according to the given direction
 */
TLP_SCOPE Iterator<node> *getAdjacentNodesIterator(const Graph *graph, node n, EdgeType direction);

/**
 * Return a function to get an Iterator on the incident edges of a graph node
 * according to the given direction
 */
TLP_SCOPE Iterator<edge> *getIncidentEdgesIterator(const Graph *graph, node n, EdgeType direction);

/**
 *  This ordering was first introduced by C. Gutwenger and P. Mutzel in \n
 *  "Grid embeddings of biconnected planar graphs", \n
 *  "Extended Abstract, Max-Planck-Institut für Informatik," \n
 *  "Saarbrücken, Germany, 1997" \n
 *  Let n be the number of nodes, the original algorithm complexity is in O(n).\n
 *  But the implementation of the canonical ordering has not been made in O(n).\n
 */
TLP_SCOPE std::vector<std::vector<node>>
computeCanonicalOrdering(PlanarConMap *, std::vector<edge> *dummyEdges = nullptr,
                         PluginProgress *pluginProgress = nullptr);
/**
 * Find all the graph centers, that version does not manage edge weight.
 * complexity O(n * m). Only works on connected graphs.
 */
TLP_SCOPE std::vector<node> computeGraphCenters(Graph *graph);
/**
 * return a node that can be considered as the graph center.
 * It is an heuristic, thus it is not absolutely sure that this
 * node is a graph center. Only works on connected graphs.
 */
TLP_SCOPE node graphCenterHeuristic(Graph *graph, PluginProgress *pluginProgress = nullptr);
/**
 * return a new node connected to all previously
 * existing nodes which had a null indegree
 */
TLP_SCOPE node makeSimpleSource(Graph *graph);

TLP_SCOPE void makeProperDag(Graph *graph, std::list<node> &addedNodes,
                             flat_hash_map<edge, edge> &replacedEdges,
                             IntegerProperty *edgeLength = nullptr);

/**
 * Select a spanning forest of the graph,
 * i.e for all graph elements (nodes or edges) belonging to that forest
 * the selectionProperty associated value is true. The value is false
 * for the other elements
 */
TLP_SCOPE void selectSpanningForest(Graph *graph, BooleanProperty *selectionProperty,
                                    PluginProgress *pluginProgress = nullptr);

/**
 * Select a spanning tree of a graph assuming it is connected;
 * i.e for all graph elements (nodes or edges) belonging to that tree
 * the selectionProperty associated value is true. The value is false
 * for the other elements
 */
TLP_SCOPE void selectSpanningTree(Graph *graph, BooleanProperty *selection,
                                  PluginProgress *pluginProgress = nullptr);

/**
 * Select the minimum spanning tree (Kruskal algorithm) of a weighted graph,
 * i.e for all graph elements (nodes or edges) belonging to that tree
 * the selectionProperty associated value is true. The value is false
 * for the other elements
 */
TLP_SCOPE void selectMinimumSpanningTree(Graph *graph, BooleanProperty *selectionProperty,
                                         NumericProperty *weight = nullptr,
                                         PluginProgress *pluginProgress = nullptr);

/**
 * @brief Performs a breadth-first search on a graph.
 * @param graph The graph to traverse with a BFS.
 * @param root The node from whom to start the BFS. If not provided, the root
 * node will be assigned to a source node in the graph (node with input degree equals to 0).
 * If there is no source node in the graph, a random node will be picked.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with the nodes of the graph in the order they have been visited by
 * the BFS.
 */
TLP_SCOPE std::vector<node> bfs(const Graph *graph, node root, bool directed = false);

/**
 * @brief Performs a breadth-first search on a graph.
 * @param graph The graph to traverse with a BFS.
 * @param root The node from whom to start the BFS. If not provided, the root
 * node will be assigned to a source node in the graph (node with input degree equals to 0).
 * If there is no source node in the graph, a random node will be picked.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with the edges of the graph in the order they have been followed by
 * the BFS.
 */
TLP_SCOPE std::vector<edge> bfsEdges(const Graph *graph, node root, bool directed = false);

/**
 * @brief Performs a cumulative breadth-first search on every node of a graph.
 * @param graph The graph to traverse with a BFS.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with with the nodes of the graph in the order they have been visited by
 * the BFS.
 */
TLP_SCOPE std::vector<node> bfs(const Graph *graph, bool directed = false);

/**
 * @brief Performs a cumulative breadth-first search on every node of a graph.
 * @param graph The graph to traverse with a BFS.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with with the edges of the graph in the order they have been followed by
 * the BFS.
 */
TLP_SCOPE std::vector<edge> bfsEdges(const Graph *graph, bool directed = false);

/**
 * @brief Performs a depth-first search on a graph.
 * @param graph The graph to traverse with a DFS.
 * @param root The node from whom to start the DFS. If not provided, the root
 * node will be assigned to a source node in the graph (node with input degree equals to 0).
 * If there is no source node in the graph, a random node will be picked.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return A vector filled with the nodes of the graph in the order they have been visited by the
 * DFS.
 */
TLP_SCOPE std::vector<node> dfs(const Graph *graph, node root, bool directed = false);

/**
 * @brief Performs a depth-first search on a graph.
 * @param graph The graph to traverse with a DFS.
 * @param root The node from whom to start the DFS. If not provided, the root
 * node will be assigned to a source node in the graph (node with input degree equals to 0).
 * If there is no source node in the graph, a random node will be picked.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return A vector filled with the edges of the graph in the order they have been followed by the
 * DFS.
 */
TLP_SCOPE std::vector<edge> dfsEdges(const Graph *graph, node root, bool directed = false);

/**
 * @brief Performs a cumulative depth-first search on every node of a graph.
 * @param graph The graph to traverse with a DFS.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with the nodes of the graph in the order they have been visited by
 * the DFS.
 */
TLP_SCOPE std::vector<node> dfs(const Graph *graph, bool directed = false);

/**
 * @brief Performs a cumulative depth-first search on every node of a graph.
 * @param graph The graph to traverse with a DFS.
 * @param directed if true only follow output edges, follow all edges otherwise
 * @return a vector filled with the edges of the graph in the order they have been followed by
 * the DFS.
 */
TLP_SCOPE std::vector<edge> dfsEdges(const Graph *graph, bool directed = false);

/*
 * builds a uniform quantification with the NumericProperty associated values
 * of the nodes of a graph
 */
TLP_SCOPE void buildNodesUniformQuantification(const Graph *graph, const NumericProperty *prop,
                                               uint k, std::map<double, int> &mapping);

/*
 * builds a uniform quantification with the NumericProperty associated values
 * of the edges of a graph
 */
TLP_SCOPE void buildEdgesUniformQuantification(const Graph *graph, const NumericProperty *prop,
                                               uint k, std::map<double, int> &mapping);

/**
 * @brief Extends selection to have a graph (no dangling edge)
 * @param graph The graph to compute on.
 * @param selection The Boolean property to consider. The selection will be extend using this
 * property.
 * @return The number of element added to the selection property.
 */
TLP_SCOPE unsigned makeSelectionGraph(const Graph *graph, BooleanProperty *selection,
                                      bool *test = nullptr);

/**
 * @enum This Enum describes the possible types of path to select between a source and target nodes
 * It is used in tlp::selectShortestPaths. Reversed means the same than Directed from target node to
 *source node.
 **/
enum class ShortestPathType {
  OnePath = 0,
  OneDirectedPath = 1,
  OneReversedPath = 2,
  AllPaths = 3,
  AllDirectedPaths = 4,
  AllReversedPaths = 5
};

/**
 * @brief select the shortest paths between two nodes
 * @param graph The graph to compute on.
 * @param src The source node of the paths
 * @param tgt The target node of the paths
 * @param pathType The type of path to consider (chosen among tlp::ShortestPathType enumeration
 * values)
 * @param weights A Double property giving the edges weight if weighted paths have to be considered.
 * Can be set to null to select unweighted paths.
 * @param selection The Boolean property to consider as selection for which the values corresponding
 * to the nodes/edges owning to the shortests path(s) will be set to True.
 * @return true if a path exists between the src and tgt nodes; false if not.
 */
TLP_SCOPE bool selectShortestPaths(const Graph *const graph, node src, node tgt,
                                   ShortestPathType pathType, const DoubleProperty *const weights,
                                   BooleanProperty *selection);

/*
 * Return all reachable nodes, according to direction,
 * at distance less or equal to maxDistance of startNode.
 * If direction is set to EdgeType::UNDIRECTED use undirected graph,
 * EdgeType::DIRECTED use directed graph
 * and EdgeType::INV_DIRECTED use reverse directed graph (ie. all edges are reversed)
 */
TLP_SCOPE std::set<node> reachableNodes(const Graph *graph, const node startNode, uint maxDistance,
                                        EdgeType direction = EdgeType::UNDIRECTED);

TLP_SCOPE void computeDijkstra(const Graph *const graph, node src,
                               const EdgeVectorProperty<double> &weights,
                               NodeVectorProperty<double> &nodeDistance, EdgeType direction,
                               flat_hash_map<node, std::list<node>> &ancestors,
                               std::stack<node> *queueNodes = nullptr,
                               MutableContainer<int> *numberOfPaths = nullptr);
}
#endif // TALIPOT_GRAPH_TOOLS_H
