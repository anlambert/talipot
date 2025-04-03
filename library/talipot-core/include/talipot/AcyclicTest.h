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

#ifndef TALIPOT_ACYCLIC_TEST_H
#define TALIPOT_ACYCLIC_TEST_H

#include <vector>

#include <talipot/config.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>

namespace tlp {

class Graph;

/**
 * @ingroup Checks
 *
 * @brief Stores all the added information on self loops.
 *
 * Self loops are removed by adding two nodes and three edges.
 *
 * These are stores here, along with the old self looping edge.
 *
 * From Wikipedia: "A directed acyclic graph (DAG), is a directed graph with no directed cycles."
 **/
struct SelfLoops {
  public:
    SelfLoops(node n1, node n2, edge e1, edge e2, edge e3, edge old)
        : n1(n1), n2(n2), e1(e1), e2(e2), e3(e3), old(old) {}
    node n1, n2;
    edge e1, e2, e3, old;
};

/**
 * @ingroup Checks
 *
 * @brief This class provides tests for acyclicity on a graph.
 * Results are cached in a map of graphs and result.
 * This class observes the graphs that have been tested to remove the result from this graph if it
 *is modified.
 * This forces the use of the singleton pattern instead of simply using static functions/members.
 **/
class TLP_SCOPE AcyclicTest {
  public:
    /**
     * @brief Checks whether the graph is acyclic or not.
     * The result is cached so subsequent calls are in O(1).
     *
     * @param graph The graph on which to perform the acyclicity test.
     * @return bool True if the graph is acyclic, false otherwise.
     **/
    static bool isAcyclic(const Graph *graph);

    /**
     * @brief Makes the graph acyclic by removing edges.
     *
     * @param graph The graph to make acyclic.
     * @param reversed The edges that were reversed during the process.
     * @param selfLoops Sets of two nodes and three edges that were added instead of self loops.
     * @return void
     **/
    static void makeAcyclic(Graph *graph, std::vector<edge> &reversed,
                            std::vector<tlp::SelfLoops> &selfLoops);

    /**
     * @brief Returns whether the graph is acyclic.
     * Collection of obstruction edges takes a bit of time, as iteration over the graph must
     *continue even when it has been found cyclic.
     *
     * @param graph the graph to test for acyclicity
     * @param obstructionEdges If not null, will be filled with edges that cause the graph to be
     *cyclic. Defaults to 0.
     * @return bool
     **/
    static bool acyclicTest(const Graph *graph, std::vector<edge> *obstructionEdges = nullptr);
};
}

#endif // TALIPOT_ACYCLIC_TEST_H
