/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

%ModuleHeaderCode
#include <talipot/GraphMeasure.h>
#include <talipot/VectorProperty.h>
%End

namespace tlp {

  enum class EdgeType {DIRECTED = 0, INV_DIRECTED = 1, UNDIRECTED = 2};

//===========================================================================================

  double averagePathLength(const tlp::Graph* g);
%Docstring
tlp.averagePathLength(graph)

Returns the average path length of a graph, that is the sum of the shortest distances for all pair
of distinct nodes in that graph divided by the number of those pairs. For a pair of non connected
nodes, the shorted distance is set to 0.

:param graph:
   the graph on which to compute the average path length

:type graph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double averageClusteringCoefficient(const tlp::Graph *graph);
%Docstring
tlp.averageClusteringCoefficient(graph)

Returns the clustering coefficient of a graph as the average of the local clustering coefficients
(see :func:`tlp.clusteringCoefficient`) of all the nodes.

:param graph:
   the graph on which to compute the average clustering coefficient

:type graph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  void clusteringCoefficient(const tlp::Graph *g, tlp::DoubleProperty *result, uint maxDepth = 1);
%Docstring
tlp.clusteringCoefficient(graph, result, maxDepth=1)

Assigns to each node its local clustering coefficient that is the proportion of edges between the
nodes within its neighbourhood divided by the number of edges that could possibly exist between
them. This quantifies how close its neighbors are to being a clique.

:param graph:
   the graph on which to compute the clustering coefficient for each node

:type graph:
   :class:`tlp.Graph`

:param result:
   a graph property in which the results will be stored

:type result:
   :class:`tlp.DoubleProperty`

:param maxDepth:
   the maximum distance between each node and its neighbours.

:type maxDepth:
   integer
%End

%MethodCode
tlp::NodeVectorProperty<double> result(a0);
tlp::clusteringCoefficient(a0, result, a2);
result.copyToProperty(a1);
%End

//===========================================================================================

  void dagLevel(const tlp::Graph *graph, tlp::IntegerProperty* result);
%Docstring
tlp.dagLevel(graph, result)

Assigns to each node of a Directed Acyclic Graph a level such that if the edge e(u,v) exists
level(u) < level(v). The algorithm ensures that the number of level used is minimal.

:param graph:
   the graph on which to compute the dag level

:type graph:
   :class:`tlp.Graph`

:param result:
   a graph property in which the results will be stored

:type result:
   :class:`tlp.IntegerProperty`

.. warning::

   The graph must be acyclic (no self loops).
%End

%MethodCode
tlp::NodeVectorProperty<uint> result(a0);
tlp::dagLevel(a0, result);
result.copyToProperty(a1);
%End

//===========================================================================================

  uint maxDegree(const tlp::Graph *graph);
%Docstring
tlp.maxDegree(graph)

Returns the maximum degree of the graph's nodes.

:param graph:
   the graph on which to compute the maximum degree

:type graph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

//===========================================================================================

  uint minDegree(const tlp::Graph *graph);
%Docstring
tlp.minDegree(graph)

Returns the minimum degree of the graph's nodes.

:param graph:
   the graph on which to compute the minimum degree

:type graph:
   :class:`tlp.Graph`

:rtype:
   integer
%End

//===========================================================================================

  uint maxDistance(const tlp::Graph *graph, const tlp::node node, tlp::IntegerProperty *result, tlp::EdgeType direction = tlp::EdgeType::UNDIRECTED);
%Docstring
tlp.maxDistance(graph, node, result, direction)

Computes the distances from a node to all the other nodes of a graph and return the maximum one.

:param graph:
   the graph on which to compute the maximum distance

:type graph:
   :class:`tlp.Graph`

:param node:
   an existing graph node

:type node:
   :class:`tlp.node`

:param result:
   a graph property in which the results will be stored

:type result:
   :class:`tlp.IntegerProperty`

:param direction:
   specify if the graph must be directed or not

:type direction:
   tlp.EdgeType::DIRECTED, tlp.EdgeType::INV_DIRECTED, tlp.EdgeType::UNDIRECTED

:rtype:
   integer

:throws:
   an exception if the node does not belong to the graph
%End

%MethodCode
if (a0->isElement(*a1)) {
  tlp::NodeVectorProperty<uint> result(a0);
  sipRes = tlp::maxDistance(a0, *a1, result, a3);
  result.copyToProperty(a2);
} else {
  sipIsErr = throwInvalidNodeException(a0, *a1);
}
%End

//===========================================================================================

  double maxDistanceWeighted(const tlp::Graph *graph, const tlp::node node, tlp::DoubleProperty *result, tlp::DoubleProperty *weight, tlp::EdgeType direction = tlp::EdgeType::UNDIRECTED);
%Docstring
tlp.maxDistanceWeighted(graph, node, result, weight, direction)

Computes the distances from a node to all the other nodes of a weighted graph and return the maximum
one.

:param graph:
   the graph on which to compute the maximum distance

:type graph:
   :class:`tlp.Graph`

:param node:
   an existing graph node

:type node:
   :class:`tlp.node`

:param result:
   a graph property in which the results will be stored

:type result:
   :class:`tlp.DoubleProperty`

:param result:
   a graph property in which edges lengths are stored

:type result:
   :class:`tlp.DoubleProperty`

:param direction:
   specify if the graph must be directed or not

:type direction:
   tlp.EdgeType::DIRECTED, tlp.EdgeType::INV_DIRECTED, tlp.EdgeType::UNDIRECTED

:rtype:
   double

:throws:
   an exception if the node does not belong to the graph
%End

%MethodCode
if (a0->isElement(*a1)) {
  tlp::NodeVectorProperty<double> result(a0);
  sipRes = tlp::maxDistance(a0, *a1, result, a3, a4);
  result.copyToProperty(a2);
} else {
  sipIsErr = throwInvalidNodeException(a0, *a1);
}
%End


//===========================================================================================

  void reachableNodes(const tlp::Graph *graph, const tlp::node startNode, std::set<tlp::node> &result /Out/, uint maxDistance, tlp::EdgeType direction = tlp::EdgeType::UNDIRECTED);
%Docstring
tlp.reachableNodes(graph, startNode, maxDistance, direction)

Returns the nodes that can be reached from a node, according to a direction, at distance less or
equal to a maximum one.

:param graph:
   the graph to work on

:type graph:
   :class:`tlp.Graph`

:param startNode:
   an existing graph node

:type startNode:
   :class:`tlp.node`

:param maxDistance:
   the maximum distance to reach nodes

:type maxDistance:
   integer

:param direction:
   specify if the graph must be directed or not

:type direction:
   tlp.EdgeType::DIRECTED, tlp.EdgeType::INV_DIRECTED, tlp.EdgeType::UNDIRECTED

:rtype:
   set of :class:`tlp.node`

:throws:
   an exception if the node does not belong to the graph
%End

%MethodCode
if (a0->isElement(*a1)) {
  std::unordered_map<tlp::node, bool> hResult;
  markReachableNodes(a0, *a1, hResult, a3, a4);
  for (const auto &itr : hResult) {
    a2->insert(itr.first);
  }
} else {
  sipIsErr = throwInvalidNodeException(a0, *a1);
}
%End
};
