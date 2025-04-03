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

#ifndef TALIPOT_GRAPH_PARALLEL_TOOLS_H
#define TALIPOT_GRAPH_PARALLEL_TOOLS_H

#include <talipot/Graph.h>
#include <talipot/ParallelTools.h>

namespace tlp {

// ===================================================================================

template <typename NodeFunction>
void inline TLP_MAP_NODES(const tlp::Graph *graph, const NodeFunction &nodeFunction) {
    for (auto n : graph->nodes()) {
        nodeFunction(n);
    }
}

template <typename NodeIndexFunction>
void inline TLP_MAP_NODES_AND_INDICES(const tlp::Graph *graph,
                                      const NodeIndexFunction &nodeIndexFunction) {
    uint i = 0;
    for (auto n : graph->nodes()) {
        nodeIndexFunction(n, i++);
    }
}

// ===================================================================================

/**
 * Template function to ease the creation of OpenMP parallel jobs taking
 * a graph node as parameter.
 *
 * @param graph the graph on which to run job on the nodes
 * @param nodeFunction callable object (e.g. lambda function) taking a tlp::node as parameter
 *
 * Example of use:
 *
 * @code
 * auto computationIntensiveTask = [&](const tlp::node &n) {
 *  double result = 0;
 *   ...
 *  return result;
 * };
 * tlp::NodeVectorProperty<double> result(graph);
 * TLP_PARALLEL_MAP_NODES(graph, [&](const tlp::node &n) {
 *   // run task in a thread
 *   result[n] = computationIntensiveTask(n);
 * });
 * @endcode
 */
template <typename NodeFunction>
void inline TLP_PARALLEL_MAP_NODES(const tlp::Graph *graph, const NodeFunction &nodeFunction) {
    TLP_PARALLEL_MAP_VECTOR<tlp::node, NodeFunction>(graph->nodes(), nodeFunction);
}

// ===================================================================================

/**
 * Template function to ease the creation of OpenMP parallel jobs taking
 * a graph node and its iteration index as parameter.
 *
 * @param graph the graph on which to run job on the nodes
 * @param nodeIndexFunction callable object (e.g. lambda function) taking a tlp::node and
 *    and unsigned integer as parameter
 *
 * Example of use:
 *
 * @code
 * auto computationIntensiveTask = [&](const tlp::node &n, uint i) {
 *  double result = 0;
 *   ...
 *  return result;
 * };
 * tlp::NodeVectorProperty<double> result(graph);
 * TLP_PARALLEL_MAP_EDGES(graph, [&](const tlp::node &n, uint i) {
 *   // run task in a thread
 *   result[n] = computationIntensiveTask(n, i);
 * });
 * @endcode
 */
template <typename NodeFunction>
void inline TLP_PARALLEL_MAP_NODES_AND_INDICES(const tlp::Graph *graph,
                                               const NodeFunction &nodeFunction) {
    TLP_PARALLEL_MAP_VECTOR_AND_INDICES<tlp::node, NodeFunction>(graph->nodes(), nodeFunction);
}

// ===================================================================================

template <typename EdgeFunction>
void inline TLP_MAP_EDGES(const tlp::Graph *graph, const EdgeFunction &edgeFunction) {
    for (auto e : graph->edges()) {
        edgeFunction(e);
    }
}

template <typename EdgeIndexFunction>
void inline TLP_MAP_EDGES_AND_INDICES(const tlp::Graph *graph,
                                      const EdgeIndexFunction &edgeIndexFunction) {
    uint i = 0;
    for (auto e : graph->edges()) {
        edgeIndexFunction(e, i++);
    }
}

// ===================================================================================

/**
 * Template function to ease the creation of OpenMP parallel jobs taking
 * a graph edge as parameter.
 *
 * @param graph the graph on which to run job on the edges
 * @param edgeFunction callable object (e.g. lambda function) taking a tlp::edge as parameter
 *
 * Example of use:
 *
 * @code
 * auto computationIntensiveTask = [&](const tlp::edge &e) {
 *  double result = 0;
 *   ...
 *  return result;
 * };
 * tlp::EdgeVectorProperty<double> result(graph);
 * TLP_PARALLEL_MAP_EDGES(graph, [&](const tlp::edge &e) {
 *   // run task in a thread
 *   result[e] = computationIntensiveTask(e);
 * });
 * @endcode
 */
template <typename EdgeFunction>
void inline TLP_PARALLEL_MAP_EDGES(const tlp::Graph *graph, const EdgeFunction &edgeFunction) {
    TLP_PARALLEL_MAP_VECTOR<tlp::edge, EdgeFunction>(graph->edges(), edgeFunction);
}

// ===================================================================================

/**
 * Template function to ease the creation of OpenMP parallel jobs taking
 * a graph edge and its iteration index as parameter.
 *
 * @param graph the graph on which to run job on the edges
 * @param edgeIndexFunction callable object (e.g. lambda function) taking a tlp::edge and
 *    and unsigned integer as parameter
 *
 * Example of use:
 *
 * @code
 * auto computationIntensiveTask = [&](const tlp::edge &e, uint i) {
 *  double result = 0;
 *   ...
 *  return result;
 * };
 * tlp::EdgeVectorProperty<double> result(graph);
 * TLP_PARALLEL_MAP_EDGES(graph, [&](const tlp::edge &e, uint i) {
 *   // run task in a thread
 *   result[e] = computationIntensiveTask(e, i);
 * });
 * @endcode
 */
template <typename EdgeFunction>
void inline TLP_PARALLEL_MAP_EDGES_AND_INDICES(const tlp::Graph *graph,
                                               const EdgeFunction &edgeFunction) {
    TLP_PARALLEL_MAP_VECTOR_AND_INDICES<tlp::edge, EdgeFunction>(graph->edges(), edgeFunction);
}
}

#endif // TALIPOT_GRAPH_PARALLEL_TOOLS_H
