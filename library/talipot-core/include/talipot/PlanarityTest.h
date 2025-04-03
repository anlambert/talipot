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

#ifndef TALIPOT_PLANARITY_TEST_H
#define TALIPOT_PLANARITY_TEST_H

#include <list>
#include <talipot/config.h>

namespace tlp {

class Graph;
struct edge;

/**
 * @ingroup Checks
 * @brief performs tests to check whether a graph is planar.
 * From wikipedia: "A planar graph is a graph that can be embedded in the plane, i.e., it can be
 *drawn on the plane in such a way that its edges intersect only at their endpoints."
 **/
class TLP_SCOPE PlanarityTest {
  public:
    /*
      The set of edges of the graph is modified during the execution of
      the methods below. If you call one of them while iterating
      on the edges of the graph, be careful to use a StableIterator
      to avoid any possible invalidation of the iterator.
    */

    /**
     * @brief Checks whether the graph is planar (i.e. the graph can be drawn on the plane in such a
     *way that no edges cross each other).
     *
     * @param graph The graph to check for planarity.
     * @return bool True if the graph is planar, false otherwise.
     * @note The graph is made biconnected to ease (?) the computation.
     **/
    static bool isPlanar(Graph *graph);

    /**
     * Returns true if the current embedding of the graph is planar, false otherwise.
     */
    /**
     * @brief Checks if the graph is plane (or a planar embedding).
     * A planar graph *can* be drawn such as no edges cross each other, a plane graph *is* drawn
     *such as no edges cross each other.
     *
     * @param graph The graph to check for a planar e;bedding.
     * @return bool True if the graph is a planar embedding, false otherwise.
     **/
    static bool isPlanarEmbedding(const Graph *graph);

    /**
     * @brief Makes a planar graph a planar embedding, i.e. makes a graph so that no edges cross
     *each other if it is known to be possible. This modifies the order of the edges around the
     *nodes.
     *
     * @param graph The graph to make a planar embedding of.
     * @return bool True if the graph is planar, false otherwise.
     **/
    static bool planarEmbedding(Graph *graph);

    /**
     * @brief Computes a list of edges that prevent the graph from being planar (i.e. part of the
     *minor of K3,3 or K5).
     *
     * @param graph The graph on which to compute the obstruction edges.
     * @return :list< tlp::edge, std::allocator< tlp::edge > > The obstruction edges.
     **/
    static std::list<edge> getObstructionsEdges(Graph *graph);
};
}
#endif // TALIPOT_PLANARITY_TEST_H
