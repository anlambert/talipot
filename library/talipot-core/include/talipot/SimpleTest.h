/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_SIMPLE_TEST_H
#define TALIPOT_SIMPLE_TEST_H

#include <vector>

#include <talipot/config.h>

namespace tlp {

class Graph;
struct edge;

/**
 * @ingroup Checks
 * @brief Performs test to check if a graph is Simple.
 * An undirected graph is simple if it has no loops and no more than one
 * edge between any unordered pair of vertices.
 * A directed graph is simple if has no loops and no more than one
 * edge between any ordered pair of vertices.
 **/
class TLP_SCOPE SimpleTest {
  public:
    /**
     * @brief Checks if the graph is simple (i.e. it contains no loops or parallel edges).
     *
     * @param graph The graph to check.
     * @param directed Whether the graph shall be considered directed or not.
     * @return bool True if the graph is simple, false otherwise.
     **/
    static bool isSimple(const Graph *graph, const bool directed = false);

    /**
     * Makes the graph simple by removing loops and parallel edges if any.
     */
    /**
     * @brief Makes the graph simple, by removing loops and parallel edges if any.
     *
     * @param graph The graph to make simple.
     * @param directed Whether the graph shall be considered directed or not.
     * @return The edges that were removed to make the graph simple.
     **/
    static std::vector<edge> makeSimple(Graph *graph, const bool directed = false);

    /**
     * @brief * Performs simple test and returns found loops and parallel edges.
     *
     * @param graph The graph to check for simplicity.
     * @param directed Whether the graph shall be considered directed or not.
     * @return A pair whose first member is the found loops and second one the found parallel edges.
     **/
    static std::pair<std::vector<edge>, std::vector<edge>>
    getLoopsAndParallelEdges(const Graph *graph, const bool directed = false);
};
}
#endif // TALIPOT_SIMPLE_TEST_H
