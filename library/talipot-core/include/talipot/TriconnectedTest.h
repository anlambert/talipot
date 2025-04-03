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

#ifndef TALIPOT_TRICONNECTED_TEST_H
#define TALIPOT_TRICONNECTED_TEST_H

#include <talipot/config.h>

namespace tlp {

class Graph;

/**
 * @ingroup Checks
 * @brief Provides functions to test if a graph is triconnected.
 **/
class TLP_SCOPE TriconnectedTest {
  public:
    /**
     * @brief Checks if the graph is triconnected.
     * Creates a clone sugraph in which to operate, then iterates over the nodes, and deletes them.
     * Once the node is deleted, checks if the graph is biconnected.
     * If it is not, then the graph is not triconnected.
     * If it is, adds back the node and its edges.
     *
     * @param graph The graph to check is triconnected.
     * @return bool True if the graph is triconnected, false otherwise.
     **/
    static bool isTriconnected(Graph *graph);
};
}
#endif // TALIPOT_TRICONNECTED_TEST_H
