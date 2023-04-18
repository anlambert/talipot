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

class PropertyInterface : tlp::Observable /Abstract/ {
%TypeHeaderCode
#include <talipot/PropertyInterface.h>
%End

public:

  tlp::Graph * getGraph();
%Docstring
tlp.PropertyInterface.getGraph()

Returns the graph on which the property has been defined.

:rtype:
   :class:`tlp.Graph`

.. warning::

   If the property is inherited the graph could be different that the one used to get that property.
%End

 //===========================================================================================

  virtual bool hasNonDefaultValuatedNodes(const tlp::Graph* subgraph= NULL) const = 0;
%Docstring
   tlp.PropertyInterface.hasNonDefaultValuatedNodes(subgraph=None)

   Returns whether it exists nodes whose value is different from the default value.

   :param subgraph:
      a subgraph can be given in parameter. In that case, only the nodes belonging to this subgraph
      are considered.

   :type subgraph:
      :class:`tlp.Graph`

   :rtype:
      boolean
%End

 //===========================================================================================

  virtual uint numberOfNonDefaultValuatedNodes(const tlp::Graph* subgraph= NULL) const = 0;
%Docstring
   tlp.PropertyInterface.numberOfNonDefaultValuatedNodes(subgraph=None)

   Returns the number of nodes whose value is different from the default value.

   :param subgraph:
      a subgraph can be given in parameter. In that case, only the number of nodes in this subgraph
      whose value is different from the default one is returned.

   :type subgraph:
      :class:`tlp.Graph`

   :rtype:
      integer
%End

//===========================================================================================

  virtual tlp::Iterator<tlp::node>* getNonDefaultValuatedNodes(const tlp::Graph* subgraph= NULL) const = 0 /TransferBack, TypeHint="Iterator[tlp.node]"/;
%Docstring
tlp.PropertyInterface.getNonDefaultValuatedNodes(subgraph=None)

Returns an iterator on all nodes whose value is different from the default value.

:param subgraph:
   a subgraph can be given in parameter. In that case, only the nodes owned by this subgraph are
   returned by the iterator.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   :class:`tlp.IteratorNode`
%End

%MethodCode
sipRes = tlp::stableIterator(sipCpp->getNonDefaultValuatedNodes(a0));
%End

 //===========================================================================================

  virtual bool hasNonDefaultValuatedEdges(const tlp::Graph* subgraph= NULL) const = 0;
%Docstring
   tlp.PropertyInterface.hasNonDefaultValuatedEdges(subgraph=None)

   Returns whether it exists edges whose value is different from the default value.

   :param subgraph:
      a subgraph can be given in parameter. In that case, only the edges belonging to this subgraph
      are considered.

   :type subgraph:
      :class:`tlp.Graph`

   :rtype:
      boolean
%End

 //===========================================================================================

  virtual uint numberOfNonDefaultValuatedEdges(const tlp::Graph* subgraph= NULL) const = 0;
%Docstring
   tlp.PropertyInterface.numberOfNonDefaultValuatedEdges(subgraph=None)

   Returns the number of edges whose value is different from the default value.

   :param subgraph:
      a subgraph can be given in parameter. In that case, only the number of edges in this subgraph
      whose value is different from the default one is returned.

   :type subgraph:
      :class:`tlp.Graph`

   :rtype:
      integer
%End

//===========================================================================================

  virtual tlp::Iterator<tlp::edge>* getNonDefaultValuatedEdges(const tlp::Graph* subgraph= NULL) const = 0 /TransferBack, TypeHint="Iterator[tlp.edge]"/;
%Docstring
tlp.PropertyInterface.getNonDefaultValuatedEdges(subgraph=None)

Returns an iterator on all edges whose value is different from the default value.

:param subgraph:
   a subgraph can be given in parameter. In that case, only the edges owned by this subgraph are
   returned by the iterator.

:type subgraph:
   :class:`tlp.Graph`

:rtype:
   :class:`tlp.IteratorEdge`
%End

%MethodCode
sipRes = tlp::stableIterator(sipCpp->getNonDefaultValuatedEdges(a0));
%End

//===========================================================================================

  virtual const std::string& getTypename() const = 0;
%Docstring
tlp.PropertyInterface.getTypename()

Returns a string describing the type of the property, i.e. "graph", "double", "layout", "string",
"integer", "color", "size", ...

:rtype:
   string
%End

//===========================================================================================

  const std::string& getName() const = 0;
%Docstring
tlp.PropertyInterface.getName()

Returns the name of the property.

:rtype:
   string
%End

//===========================================================================================

  virtual std::string getNodeDefaultStringValue() const = 0;
%Docstring
tlp.PropertyInterface.getNodeDefaultStringValue()

Returns a string representation of the node default value.

:rtype:
   string
%End

//===========================================================================================

  virtual std::string getEdgeDefaultStringValue() const = 0;
%Docstring
tlp.PropertyInterface.getEdgeDefaultStringValue()

Returns a string representation of the edge default value.

:rtype:
   string
%End
//===========================================================================================

  virtual tlp::PropertyInterface* clonePrototype(tlp::Graph* g, const std::string & name) const = 0;
%Docstring
tlp.PropertyInterface.clonePrototype(g, name)

Returns a newly created property of the same type in the graph g.

:param g:
   the graph in with the new property will be added

:type g:
   :class:`tlp.Graph`

:param name:
   the name of the property to create and return

:type name:
   string

:rtype:
   :class:`tlp.PropertyInterface`
%End

//===========================================================================================

virtual bool setNodeDefaultStringValue( const std::string & strVal ) = 0;
%Docstring
tlp.PropertyInterface.setNodeDefaultStringValue(str)

Sets the value assigned as the default one to the future added nodes from a string representation.

Returns :const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param str:
   a string representing the new value to set on future added nodes.

:type str:
   string

:rtype:
   boolean
%End

//===========================================================================================

  virtual bool setAllNodeStringValue( const std::string & strVal , const tlp::Graph *graph = nullptr) = 0;
%Docstring
tlp.PropertyInterface.setAllNodeStringValue(str, graph = None)

Sets the same value for all nodes in a graph in converting the given string representation of a
value.

All previous values are lost and the represented value is assigned as the default one to the future
added nodes.

An optional descendant graph from the one associated to that property can also be provided. In that
case only the nodes from that graph will have their value modified and the default node value will
not be modified.

.. warning::

   If the provided graph is not a descendant of the one associated to that property, no node value
   will be modified in it.

Returns :const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param str:
   a string representation of a value

:type str:
   string

:param graph:
   an optional descendant graph

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

//===========================================================================================

virtual bool setEdgeDefaultStringValue( const std::string & strVal ) = 0;
%Docstring
tlp.PropertyInterface.setEdgeDefaultStringValue(str)

Sets the value assigned as the default one to the future added edges from a string representation.

Returns :const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param str:
   a string representing the new value to set on future added edges.

:type str:
   string

:rtype:
   boolean
%End

//===========================================================================================

 virtual bool setAllEdgeStringValue( const std::string & strVal , const tlp::Graph *graph = nullptr) = 0;
%Docstring
tlp.PropertyInterface.setAllEdgeStringValue(str, graph = None)

Sets the same value for all edges in a graph in converting the given string representation of a
value.

All previous values are lost and the represented value is assigned as the default one to the future
added edges.

An optional descendant graph from the one associated to that property can also be provided. In that
case only the edges from that graph will have their value modified and the default edge value will
not be modified.

.. warning::

   If the provided graph is not a descendant of the one associated to that property, no edge value
   will be modified in it.

Returns :const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param str:
   a string representation of a value

:type str:
   string

:param graph:
   an optional descendant graph

:type graph:
   :class:`tlp.Graph`

:rtype:
   boolean
%End

  virtual void erase(const tlp::node node) = 0;
%Docstring
tlp.PropertyInterface.erase(node)

Removes the value stored for the node given in parameter. The new value for the node is the default
one.

:param node:
   an existing node

:type node:
   :class:`tlp.node`

:throws:
   an exception if the node does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->erase(*a0);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  virtual void erase(const tlp::edge edge) = 0;
%Docstring
tlp.PropertyInterface.erase(edge)

Removes the value stored for the edge given in parameter. The new value for the edge is the default
one.

:param edge:
   an existing edge

:type edge:
   :class:`tlp.edge`

:throws:
   an exception if the edge does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->erase(*a0);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  virtual std::string getNodeStringValue( const tlp::node node ) const = 0;
%Docstring
tlp.PropertyInterface.getNodeStringValue(node)

Returns a string conversion of the value registered for the given node.

:param node:
   an existing node

:type node:
   :class:`tlp.node`

:rtype:
   string

:throws:
   an exception if the node does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::string(sipCpp->getNodeStringValue(*a0));
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  virtual std::string getEdgeStringValue( const tlp::edge edge ) const = 0;
%Docstring
tlp.PropertyInterface.getEdgeStringValue(edge)

Returns a string conversion of the value registered for the given edge.

:param edge:
   an existing edge

:type edge:
   :class:`tlp.edge`

:rtype:
   string

:throws:
   an exception if the edge does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::string(sipCpp->getEdgeStringValue(*a0));
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  virtual bool setNodeStringValue( const tlp::node node, const std::string & strVal ) = 0;
%Docstring
tlp.PropertyInterface.setNodeStringValue(node, str)

Registers a new value for the given node in converting the given string representation. Returns
:const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param node:
   an existing node

:type node:
   :class:`tlp.node`

:param str:
   a string representation of a value

:type str:
   string

:rtype:
   boolean

:throws:
   an exception if the node does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setNodeStringValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  virtual bool setEdgeStringValue( const tlp::edge edge, const std::string & strVal) = 0;
%Docstring
tlp.PropertyInterface.setEdgeStringValue(edge, str)

Registers a new value for the given edge in converting the given string representation. Returns
:const:`True` if the given string representation has been successfully converted to a value,
:const:`False` otherwise.

:param edge:
   an existing edge

:type edge:
   :class:`tlp.edge`

:param str:
   a string representation of a value

:type str:
   string

:rtype:
   boolean

:throws:
   an exception if the edge does not belong to the graph attached to the property
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setEdgeStringValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  std::string __getitem__(const tlp::node node);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::string(sipCpp->getNodeStringValue(*a0));
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  void __setitem__(const tlp::node node, const std::string &strVal);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setNodeStringValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

//===========================================================================================

  std::string __getitem__(const tlp::edge edge);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::string(sipCpp->getEdgeStringValue(*a0));
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End


//===========================================================================================

  void __setitem__(const tlp::edge edge, const std::string &strVal);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setEdgeStringValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

};

};
