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

#ifndef TALIPOT_CONNECTED_TEST_H
#define TALIPOT_CONNECTED_TEST_H

#include <set>
#include <vector>

#include <talipot/config.h>

namespace tlp {

class Graph;
struct node;
struct edge;

/**
 * @ingroup Checks
 * @brief @brief Performs a test of connexity on the graph, and provides a function to make a graph
 *connected.
 * From Wikipedia: "A graph is said to be connected if every pair of vertices in the graph are
 *connected." (i.e. there is a path between every pair of vertices).
 **/
class TLP_SCOPE ConnectedTest {
public:
  /**
   * @brief Checks if a graph is connected (i.e. there is a path between every pair of vertices).
   *
   * @param graph The graph to check.
   * @return bool True if the graph is connected, false otherwise.
   **/
  static bool isConnected(const Graph *const graph);

  /**
   * @brief If the graph is not connected, adds edges to make it connected.
   *
   * @param graph The graph to make connected.
   * @return The edges that were added to make it connected.
   **/
  static std::vector<edge> makeConnected(Graph *graph);

  /**
   * @brief Gets the number of connected components in the graph.
   *
   * @param graph The graph in which to count the number of connected components.
   * @return uint The number of connected componments.
   **/
  static uint numberOfConnectedComponents(const Graph *const graph);

  /**
   * @brief Computes the set of connected components.
   *
   * @param graph The graph on which to compute connected components.
   * @return The components that were found as vectors of nodes.
   **/
  static std::vector<std::vector<node>> computeConnectedComponents(const Graph *graph);

  /**
   * @brief Computes the bridges of a graph.
   *
   * A bridge is defined as an edge which, when removed, makes the graph disconnected
   * (or more precisely, increases the number of connected components in the graph).
   *
   * @param graph The graph on which to compute bridges
   * @return The bridges that were found as a vector of edges.
   **/
  static std::vector<tlp::edge> computeBridges(const Graph *graph);

private:
  /**
   * @brief Makes the graph connected.
   *
   * @param graph The graph to make connected.
   * @param toLink The nodes that need to be linked so the graph is connected.
   * @return void
   **/
  static void connect(const Graph *const, std::vector<node> &toLink);
};
}

#endif // TALIPOT_CONNECTED_TEST_H
