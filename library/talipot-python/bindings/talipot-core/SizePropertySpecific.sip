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

  tlp::Size getMax(const tlp::Graph *sg=0);
%Docstring
tlp.SizeProperty.getMax(subgraph=None)

Returns the maximum value on the nodes for that size property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   :class:`tlp.Size`
%End

//===========================================================================================

  tlp::Size getMin(const tlp::Graph *sg=0);
%Docstring
tlp.SizeProperty.getMin(subgraph=None)

Returns the minimum value on the nodes for that size property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   :class:`tlp.Size`
%End

//===========================================================================================

  void scale(const tlp::Size&, const tlp::Graph *sg=0);
%Docstring
tlp.SizeProperty.scale(scaleFactors, subgraph=None)

Scales the sizes of the nodes and edges according to a scale factors vector (sx, sy, sz).

:param scaleFactors:
   a vector of scales factor

:type scaleFactors:
   :class:`tlp.Size`

:param subgraph:
   a subgraph can be given in parameter, in that case scales the size of the nodes belonging to that
   subgraph.

:type subgraph:
   :class:`tlp.Graph`
%End


//===========================================================================================

  void scale(const tlp::Size&, tlp::Iterator<tlp::node> * /TypeHint="Iterator[tlp.node]"/, tlp::Iterator<tlp::edge> * /TypeHint="Iterator[tlp.edge]"/);
%Docstring
tlp.SizeProperty.scale(scaleFactors, itNodes, itEdges)

Scales the sizes of the nodes and edges provided through iterators according to a scale factors
vector (sx, sy, sz).

:param scaleFactors:
   a vector of scale factors

:type scaleFactors:
   :class:`tlp.Size`

:param itNodes:
   an iterator on graph nodes

:type itNodes:
   :class:`tlp.IteratorNode`

:param itEdges:
   an iterator on graph edges

:type itEdges:
   :class:`tlp.IteratorEdge`
%End


