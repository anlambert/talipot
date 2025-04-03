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

#ifndef TALIPOT_TREE_TEST_H
#define TALIPOT_TREE_TEST_H

#include <talipot/config.h>

namespace tlp {

class Graph;
class PluginProgress;
struct node;

/**
 * @ingroup Checks
 * @brief Performs test to check whether a graph is a simple or rooted tree.
 * From wikipedia: "A tree is an undirected graph in which any two vertices are connected by exactly
 *one simple path."
 * Free trees have no designated root, while rooted trees do.
 **/
class TLP_SCOPE TreeTest {
  public:
    /**
     * @brief Checks if the graph is a rooted tree (i.e. one node is designated as the root).
     *
     * @param graph The graph to check is a tree.
     * @return bool True if the graph is a tree, false otherwise.
     **/
    static bool isTree(const Graph *graph);

    /**
     *  Returns true if the graph is a topological tree
     *  (i.e. if the graph was undirected, there would be no cycle),
     *  false otherwise.
     */
    /**
     * @brief Checks if the graph is a topological tree (i.e. if the graph was undirected, there
     *would be no cycle).
     *
     * @param graph The graph to check is a free tree.
     * @return bool True if the graph is a free tree, false otherwise.
     **/
    static bool isFreeTree(const Graph *graph);

    /**
     * Turns a free tree into a rooted tree.
     */
    /**
     * @brief Makes a free tree into a rooted tree.
     *
     * @param freeTree The free tree to make a rooted tree.
     * @param root The root of the tree.
     * @return void
     **/
    static void makeRootedTree(Graph *freeTree, node root);

    /**
     * @brief Computes a rooted tree from the graph.
     * If the graph is a rooted tree, the input graph is returned as is.
     * If the graphs is a free tree, a rooted clone subgraph is returned.
     * If the graph is connected, a rooted spanning tree of a clone subgraph is returned
     * If the graph is not connected, computes a tree for each of the connected components of a
     *clone subgraph, adds a simple source and returns the clone.
     *
     * @param graph The graph to compute a tree on.
     * @param pluginProgress reports progress on the computation. Defaults to 0.
     * @return :Graph* If the input graph is a rooted tree, returns it as is, otherwise a clone
     *subgraph transformed into a rooted tree.
     **/
    static Graph *computeTree(Graph *graph, PluginProgress *pluginProgress = nullptr);

    /**
     * @brief Removes subgraphs created during tree computation.
     * If graph and tree are the same graph, does nothing.
     *
     * @param graph The graph to clean from tree subgraphs.
     * @param tree The tree subgraph to remove.
     * @return void
     * @note this deletes the root of the graph from graph's root (i.e. calls
     *graph->getRoot()->delNode()).
     **/
    static void cleanComputedTree(Graph *graph, Graph *tree);
};
}
#endif // TALIPOT_TREE_TEST_H
