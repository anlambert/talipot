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

namespace tlp {

class NumericProperty : tlp::PropertyInterface /Abstract/ {
%Docstring
Bases: :class:`tlp.PropertyInterface`
%End

%TypeHeaderCode
#include <talipot/NumericProperty.h>
%End

%ConvertToSubClassCode
if (dynamic_cast<tlp::DoubleProperty *>(sipCpp)) {
  sipType = sipFindType("tlp::DoubleProperty");
} else if (dynamic_cast<tlp::IntegerProperty *>(sipCpp)) {
  sipType = sipFindType("tlp::IntegerProperty");
} else {
  sipType = NULL;
}
%End

public:

//===========================================================================================

  double getNodeDoubleDefaultValue();
%Docstring
tlp.NumericProperty.getNodeDoubleDefaultValue()

Returns the default value associated to the nodes.

:rtype:
   float
%End

//===========================================================================================

  double getEdgeDoubleDefaultValue();
%Docstring
tlp.NumericProperty.getEdgeDoubleDefaultValue()

Returns the default value associated to the edges.

:rtype:
   float
%End

//===========================================================================================

  const double getNodeDoubleValue(const tlp::node node);
%Docstring
tlp.NumericProperty.getNodeDoubleValue(node)

Returns the value associated to a particular node.

:param node:
   an existing node

:type node:
   :class:`tlp.node`

:rtype:
   float

:throws:
   an exception if the node does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = sipCpp->getNodeDoubleValue(*a0);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  const double getEdgeDoubleValue(const tlp::edge edge);
%Docstring
tlp.NumericProperty.getEdgeDoubleValue(edge)

Returns the value associated to a particular edge.

:param edge:
   an existing edge

:type edge:
   :class:`tlp.edge`

:rtype:
   float

:throws:
   an exception if the edge does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = sipCpp->getEdgeDoubleValue(*a0);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  double  getNodeDoubleMin(const tlp::Graph *subgraph=0);
%Docstring
tlp.NumericProperty.getNodeDoubleMin(subgraph=None)

Returns the minimum value on the nodes for that property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End


//===========================================================================================

  double  getNodeDoubleMax(const tlp::Graph *subgraph=0);
%Docstring
tlp.NumericProperty.getNodeDoubleMax(subgraph=None)

Returns the maximum value on the nodes for that property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the nodes
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double  getEdgeDoubleMin(const tlp::Graph *subgraph=0);
%Docstring
tlp.NumericProperty.getEdgeDoubleMin(subgraph=None)

Returns the minimum value on the edges for that property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the minimum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  double  getEdgeDoubleMax(const tlp::Graph *subgraph=0);
%Docstring
tlp.NumericProperty.getEdgeDoubleMax(subgraph=None)

Returns the maximum value on the edges for that double property.

:param subgraph:
   a subgraph can be given in parameter, in that case return the maximum value on the edges
   belonging to that subgraph.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   float
%End

//===========================================================================================

  void uniformQuantification(uint n);
%Docstring
tlp.NumericProperty.uniformQuantification(n)

Performs an uniform quantification on nodes and edges based on the values stored in that property.
An uniform quantification tries to separate the nodes and edges in n classes of equivalent size
according to theirs values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on nodes and edges will be erased and replaced by the id of the class they
   belong to.
%End

//===========================================================================================

  void nodesUniformQuantification(uint n);
%Docstring
tlp.NumericProperty.nodesUniformQuantification(n)

Performs an uniform quantification on nodes based on the values stored in that property. An uniform
quantification tries to separate the nodes in n classes of equivalent size according to theirs
values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on nodes will be erased and replaced by the id of the class they belong to.
%End

//===========================================================================================

  void edgesUniformQuantification(uint n);
%Docstring
tlp.NumericProperty.edgesUniformQuantification(n)

Performs an uniform quantification on edges based on the values stored in that double property. An
uniform quantification tries to separate the edges in n classes of equivalent size according to
theirs values in increasing order.

:param n:
   the number of desired classes

:type n:
   integer

.. warning::

   All previous values on edges will be erased and replaced by the id of the class they belong to.
%End


//===========================================================================================

  tlp::Iterator<tlp::node>* getSortedNodes(const tlp::Graph *sg = NULL, bool ascendingOrder = true) /TransferBack, TypeHint="Iterator[tlp.node]"/;
%Docstring
tlp.NumericProperty.getSortedNodes(subgraph=None, ascendingOrder=True)

Gets an iterator sorting nodes according to their values in that numeric property.

:param subgraph:
   If provided, returns an iterator on the subset of nodes defined by that subgraph

:type subgraph:
   :class:`tlp.Graph`

:param ascendingOrder:
   defines the sort ordering (ascending or descending)

:type ascendingOrder:
   boolean

:rtype:
   a Talipot iterator on :class:`tlp.node` objects
%End

//===========================================================================================

  tlp::Iterator<tlp::edge>* getSortedEdges(const tlp::Graph *sg = NULL, bool ascendingOrder = true) /TransferBack, TypeHint="Iterator[tlp.edge]"/;
%Docstring
tlp.NumericProperty.getSortedEdges(subgraph=None, ascendingOrder=True)

Gets an iterator sorting edges according to their values in that numeric property.

:param subgraph:
   If provided, returns an iterator on the subset of edges defined by that subgraph

:type subgraph:
   :class:`tlp.Graph`

:param ascendingOrder:
   defines the sort ordering (ascending or descending)

:type ascendingOrder:
   boolean

:rtype:
   a Talipot iterator on :class:`tlp.edge` objects
%End

//===========================================================================================

  tlp::Iterator<tlp::edge>* getSortedEdgesBySourceValue(const tlp::Graph *sg = NULL, bool ascendingOrder = true) /TransferBack, TypeHint="Iterator[tlp.edge]"/;
%Docstring
tlp.NumericProperty.getSortedEdgesBySourceValue(subgraph=None, ascendingOrder=True)

Gets an iterator sorting edges according to the values of their source nodes in that numeric
property.

:param subgraph:
   If provided, returns an iterator on the subset of edges defined by that subgraph

:type subgraph:
   :class:`tlp.Graph`

:param ascendingOrder:
   defines the sort ordering (ascending or descending)

:type ascendingOrder:
   boolean

:rtype:
   a Talipot iterator on :class:`tlp.edge` objects
%End

//===========================================================================================

  tlp::Iterator<tlp::edge>* getSortedEdgesByTargetValue(const tlp::Graph *sg = NULL, bool ascendingOrder = true) /TransferBack, TypeHint="Iterator[tlp.edge]"/;
%Docstring
tlp.NumericProperty.getSortedEdgesByTargetValue(subgraph=None, ascendingOrder=True)

Gets an iterator sorting edges according to the values of their target nodes in that numeric
property.

:param subgraph:
   If provided, returns an iterator on the subset of edges defined by that subgraph

:type subgraph:
   :class:`tlp.Graph`

:param ascendingOrder:
   defines the sort ordering (ascending or descending)

:type ascendingOrder:
   boolean

:rtype:
   a Talipot iterator on :class:`tlp.edge` objects
%End

//===========================================================================================

  tlp::Iterator<tlp::edge>* getSortedEdgesByExtremitiesValues(const tlp::Graph *sg = NULL, bool ascendingOrder = true) /TransferBack, TypeHint="Iterator[tlp.edge]"/;
%Docstring
tlp.NumericProperty.getSortedEdgesByExtremitiesValues(subgraph=None, ascendingOrder=True)

Gets an iterator sorting edges according to the values of their extremities in that numeric
property. Vectors of two numbers (first element being the source node value, second one the target
node value) are compared in that case.

:param subgraph:
   If provided, returns an iterator on the subset of edges defined by that subgraph

:type subgraph:
   :class:`tlp.Graph`

:param ascendingOrder:
   defines the sort ordering (ascending or descending)

:type ascendingOrder:
   boolean

:rtype:
   a Talipot iterator on :class:`tlp.edge` objects
%End

//===========================================================================================

  tlp::NumericProperty* copyProperty(tlp::Graph *graph) /TransferBack/;
%Docstring
tlp.NumericProperty.copyProperty(graph)

Creates a property of the same numeric type of that property (integer or float). The new property
will be a copy of this property's values for all the elements of the graph.

:param graph:
   The graph in which to create the new property

:type graph:
   :class:`tlp.Graph`

:rtype:
   :class:`tlp.NumericProperty`
%End

};

};
