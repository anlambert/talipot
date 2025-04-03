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

#ifndef TALIPOT_OUTER_PLANAR_TEST_H
#define TALIPOT_OUTER_PLANAR_TEST_H

#include <talipot/config.h>

namespace tlp {

class Graph;

/**
 * @ingroup Checks
 * @brief Provides functions to test if a graph is Outer Planar.
 *
 * From Wikipedia: "An OuterPlanar Graph is a graph that can be drawn in the plane without crossings
 *in such a way that all of the vertices belong to the unbounded face of the drawing.
 * Alternatively, a graph G is outerplanar if the graph formed from G by adding a new vertex, with
 *edges connecting it to all the other vertices, is a planar graph."
 **/
class TLP_SCOPE OuterPlanarTest {
  public:
    /**
     * Returns true if the graph is outerplanar (i.e. a graph with an embedding
     * in the plane such that all vertices belong to the unbounded face of the embedding),
     * false otherwise.
     */
    /**
     * @brief Checks if a graph is outer planar (i.e. a graph with an embedding in the plane such
     *that all vertices belong to the unbounded face of the embedding).
     *
     * @param graph The graph to check.
     * @return bool True if the graph is outer planar, false otherwise.
     * @note this cannot be const as it uses the planarity test, which is not const and would be
     *complex (impossible?) to make const.
     **/
    static bool isOuterPlanar(Graph *graph);
};
}

#endif // TALIPOT_OUTER_PLANAR_TEST_H
