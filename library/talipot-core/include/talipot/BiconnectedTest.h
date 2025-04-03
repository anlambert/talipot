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

#ifndef TALIPOT_BICONNECTED_TEST_H
#define TALIPOT_BICONNECTED_TEST_H

#include <vector>

#include <talipot/config.h>

namespace tlp {

class Graph;
struct edge;

/**
 * @ingroup Checks
 * @brief Performs a test of biconnexity on the graph, and provides a function to make a graph
 *biconnected.
 * From Wikipedia: "A biconnected graph is connected and nonseparable, meaning that if any vertex
 *were to be removed, the graph will remain connected."
 **/
class TLP_SCOPE BiconnectedTest {

  public:
    /**
     * @brief Checks whether the graph is biconnected (i.e. removing one edge does not disconnect
     *the graph, at least two must be removed).
     *
     * @param graph The graph to check for biconnectivity.
     * @return bool True if the graph is biconnected, false otherwise.
     **/
    static bool isBiconnected(const Graph *graph);

    /**
     * If the graph is not biconnected, adds edges in order to make the graph
     * biconnected. The new edges are returned.
     */
    /**
     * @brief Adds edges to make the graph biconnected.
     *
     * @param graph The graph to make biconnected.
     * @return The edges that were added in the process.
     **/
    static std::vector<edge> makeBiconnected(Graph *graph);
};
}

#endif // TALIPOT_BICONNECTED_TEST_H
