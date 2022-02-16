/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
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
#include <talipot/GraphTools.h>
%End

namespace tlp {

std::vector<tlp::node> bfs(tlp::Graph *graph);
%Docstring
tlp.bfs(graph)

Performs a cumulative breadth-first search on every node of a graph and returns a list
containing the nodes of the graph in the order they have been visited.

:param graph: The graph to traverse with a BFS.
:type graph: :class:`tlp.Graph`
:rtype: list of :class:`tlp.node`
%End

 //========================================================================================

 std::vector<tlp::edge> bfsEdges(tlp::Graph *graph);
%Docstring
tlp.bfsEdges(graph)

Performs a cumulative breadth-first search on every node of a graph and returns a list
containing the edges of the graph in the order they have been followed.

:param graph: The graph to traverse with a BFS.
:type graph: :class:`tlp.Graph`
:rtype: list of :class:`tlp.edge`
%End

 //========================================================================================

std::vector<tlp::node> bfs(tlp::Graph *graph, tlp::node root, bool directed = false);
%Docstring
tlp.bfs(graph, root[, directed])

Performs a breadth-first search on a graph and returns a list
containing the nodes of the graph in the order they have been visited.

:param graph: The graph to traverse with a BFS.
:type graph: :class:`tlp.Graph`
:param root: The node from whom to start the BFS.
:type root: :class:`tlp.node`
:param directed: If :const:`True` only follow output edges, follow all edges otherwise
:type directed: bool
:rtype: list of :class:`tlp.node`
:throws: an exception if the provided root node does not belong to the graph.
%End

%MethodCode
  if (a0->isElement(*a1)) {
    sipRes = new std::vector<tlp::node>(tlp::bfs(a0, *a1, a2));
  } else {
    sipIsErr = throwInvalidNodeException(a0, *a1);
  }
%End

//========================================================================================

std::vector<tlp::edge> bfsEdges(tlp::Graph *graph, tlp::node root, bool directed = false);
%Docstring
tlp.bfsEdges(graph, root[, directed])

Performs a breadth-first search on a graph and returns a list
containing the edges of the graph in the order they have been followed.

:param graph: The graph to traverse with a BFS.
:type graph: :class:`tlp.Graph`
:param root: The node from whom to start the BFS.
:type root: :class:`tlp.node`
:param directed: If :const:`True` only follow output edges, follow all edges otherwise
:type directed: bool
:rtype: list of :class:`tlp.edge`
:throws: an exception if the provided root node does not belong to the graph.
%End

%MethodCode
  if (a0->isElement(*a1)) {
    sipRes = new std::vector<tlp::edge>(tlp::bfsEdges(a0, *a1, a2));
  } else {
    sipIsErr = throwInvalidNodeException(a0, *a1);
  }
%End

//========================================================================================

  std::vector<tlp::node> dfs(tlp::Graph *graph);
%Docstring
tlp.dfs(graph)

Performs a cumulative depth-first search on every node of a graph and returns a list
containing the nodes of the graph in the order they have been visited.

:param graph: The graph to traverse with a DFS.
:type graph: :class:`tlp.Graph`
:rtype: list of :class:`tlp.node`
%End

//========================================================================================

  std::vector<tlp::edge> dfsEdges(tlp::Graph *graph);
%Docstring
tlp.dfsEdges(graph)

Performs a cumulative depth-first search on every node of a graph and returns a list
containing the edges of the graph in the order they have been followed.

:param graph: The graph to traverse with a DFS.
:type graph: :class:`tlp.Graph`
:rtype: list of :class:`tlp.edge`
%End

//========================================================================================

  std::vector<tlp::node> dfs(tlp::Graph *graph, tlp::node root, bool directed = false);
%Docstring
tlp.dfs(graph, root[, directed])

Performs a depth-first search on a graph and returns a list
containing the nodes of the graph in the order they have been visited.

:param graph: The graph to traverse with a DFS.
:type graph: :class:`tlp.Graph`
:param root: The node from whom to start the DFS.
:type root: :class:`tlp.node`
:param directed: If :const:`True` only follow output edges, follow all edges otherwise
:type directed: bool
:rtype: list of :class:`tlp.node`
:throws: an exception if the provided root node does not belong to the graph.
%End

%MethodCode
  if (a0->isElement(*a1)) {
    sipRes = new std::vector<tlp::node>(tlp::dfs(a0, *a1, a2));
  } else {
    sipIsErr = throwInvalidNodeException(a0, *a1);
  }
%End

//========================================================================================

  std::vector<tlp::edge> dfsEdges(tlp::Graph *graph, tlp::node root, bool directed = false);
%Docstring
tlp.dfsEdges(graph, root[, directed])

Performs a depth-first search on a graph and returns a list
containing the edges of the graph in the order they have been followed.

:param graph: The graph to traverse with a DFS.
:type graph: :class:`tlp.Graph`
:param root: The node from whom to start the DFS.
:type root: :class:`tlp.node`
:param directed: If :const:`True` only follow output edges, follow all edges otherwise
:type directed: bool
:rtype: list of :class:`tlp.edge`
:throws: an exception if the provided root node does not belong to the graph.
%End

%MethodCode
  if (a0->isElement(*a1)) {
    sipRes = new std::vector<tlp::edge>(tlp::dfsEdges(a0, *a1, a2));
  } else {
    sipIsErr = throwInvalidNodeException(a0, *a1);
  }
%End

//========================================================================================

enum class ShortestPathType { OnePath = 0, OneDirectedPath = 1 , OneReversedPath = 2,
			AllPaths = 3, AllDirectedPaths = 4, AllReversedPaths = 5 };

bool selectShortestPaths(const tlp::Graph *graph, tlp::node src, tlp::node tgt, tlp::ShortestPathType pathType,
                         const tlp::DoubleProperty *weights, tlp::BooleanProperty* selection);
%Docstring
tlp.selectShortestPaths(graph, src, tgt, pathType, weights, selection)

Select the shortest paths between two nodes.
Returns :const:`True` if a path exists between the src and tgt nodes.

One usage would be::

  tlp.selectShortestPaths(graph, tlp.node(1), tlp.node(2), tlp.ShortestPathType.AllPaths,
                          graph['viewMetric'], graph['viewSelection'])

Then the existing shortest paths will correspond to the edges (and connected nodes)
of graph for which ``graph['viewSelection']`` value is equal to :const:`True`.

:param graph: The graph to compute on.
:type graph: :class:`tlp.Graph`
:param src: The source node of the paths
:type src: :class:`tlp.node`
:param tgt: The target node of the paths
:type tgt: :class:`tlp.node`
:param pathType: The type of path to consider (:const:`Reversed` is the same than
  :const:`Directed` from target node to source node)
:type pathType: :const:`tlp.ShortestPathType.OnePath`, :const:`tlp.ShortestPathType.OneDirectedPath`,
  :const:`tlp.ShortestPathType.OneReversedPath`, :const:`tlp.ShortestPathType.AllPaths`,
  :const:`tlp.ShortestPathType.AllDirectedPaths`, :const:`tlp.ShortestPathType.AllReversedPaths`
:param weights: A graph property giving the edges weight if weighted paths
  have to be considered. Can be set to null to select unweighted paths.
:type weights: :class:`tlp.DoubleProperty`
:param selection: The graph property for which the values corresponding to
  the nodes/edges owning to the shortests path(s) will be set to True.
:type selection: :class:`tlp.BooleanProperty`
:rtype: boolean
:throws: an exception if one of the source or target nodes does not belong to the graph.
%End

%MethodCode
  if (a0->isElement(*a1)) {
    if (a0->isElement(*a2)) {
     sipRes = tlp::selectShortestPaths(a0, *a1, *a2, a3, a4, a5);
    } else {
      sipIsErr = throwInvalidNodeException(a0, *a2);
    }
  } else {
      sipIsErr = throwInvalidNodeException(a0, *a1);
  }
%End
};
