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

namespace tlp {

enum class GraphEventType {
  TLP_ADD_NODE = 0,
  TLP_DEL_NODE = 1,
  TLP_ADD_EDGE = 2,
  TLP_DEL_EDGE = 3,
  TLP_REVERSE_EDGE = 4,
  TLP_BEFORE_SET_ENDS = 5,
  TLP_AFTER_SET_ENDS = 6,
  TLP_ADD_NODES = 7,
  TLP_ADD_EDGES = 8,
  TLP_BEFORE_ADD_DESCENDANTGRAPH = 9,
  TLP_AFTER_ADD_DESCENDANTGRAPH = 10,
  TLP_BEFORE_DEL_DESCENDANTGRAPH = 11,
  TLP_AFTER_DEL_DESCENDANTGRAPH = 12,
  TLP_BEFORE_ADD_SUBGRAPH = 13,
  TLP_AFTER_ADD_SUBGRAPH = 14,
  TLP_BEFORE_DEL_SUBGRAPH = 15,
  TLP_AFTER_DEL_SUBGRAPH = 16,
  TLP_ADD_LOCAL_PROPERTY = 17,
  TLP_BEFORE_DEL_LOCAL_PROPERTY = 18,
  TLP_AFTER_DEL_LOCAL_PROPERTY = 19,
  TLP_ADD_INHERITED_PROPERTY = 20,
  TLP_BEFORE_DEL_INHERITED_PROPERTY = 21,
  TLP_AFTER_DEL_INHERITED_PROPERTY = 22,
  TLP_BEFORE_RENAME_LOCAL_PROPERTY = 23,
  TLP_AFTER_RENAME_LOCAL_PROPERTY = 24,
  TLP_BEFORE_SET_ATTRIBUTE = 25,
  TLP_AFTER_SET_ATTRIBUTE = 26,
  TLP_REMOVE_ATTRIBUTE = 27,
  TLP_BEFORE_ADD_LOCAL_PROPERTY = 28,
  TLP_BEFORE_ADD_INHERITED_PROPERTY = 29
};

class GraphEvent : tlp::Event {
%Docstring
That class represents an event that happened on a :class:`tlp.Graph` instance.
To get the graph that sent the event, use the :meth:`tlp.GraphEvent.getGraph`.
The type of graph event can be retrieved through the :meth:`tlp.GraphEvent.getType` method.
Below is an exhaustive list of those types :

  * :const:`tlp.GraphEventType.TLP_ADD_NODE` : a node has been added in the graph (use :meth:`tlp.GraphEvent.getNode` to get the concerned node)
  * :const:`tlp.GraphEventType.TLP_DEL_NODE` : a node has been deleted in the graph (use :meth:`tlp.GraphEvent.getNode` to get the concerned node)
  * :const:`tlp.GraphEventType.TLP_ADD_EDGE` : an edge has been added in the graph (use :meth:`tlp.GraphEvent.getEdge` to get the concerned edge)
  * :const:`tlp.GraphEventType.TLP_DEL_EDGE` : an edge has been deleted in the graph (use :meth:`tlp.GraphEvent.getEdge` to get the concerned edge)
  * :const:`tlp.GraphEventType.TLP_REVERSE_EDGE` : the direction of an edge has been reversed in the graph (use :meth:`tlp.GraphEvent.getEdge` to get the concerned edge)
  * :const:`tlp.GraphEventType.TLP_BEFORE_SET_ENDS` : the extremities of an edge is about to be modified (use :meth:`tlp.GraphEvent.getEdge` to get the concerned edge)
  * :const:`tlp.GraphEventType.TLP_AFTER_SET_ENDS` : the extremities of an edge have been modified (use :meth:`tlp.GraphEvent.getEdge` to get the concerned edge)
  * :const:`tlp.GraphEventType.TLP_ADD_NODES` : several nodes have been added in the graph (use :meth:`tlp.GraphEvent.getNodes` to get the list of concerned nodes)
  * :const:`tlp.GraphEventType.TLP_ADD_EDGES` : several edges have been added in the graph (use :meth:`tlp.GraphEvent.getEdges` to get the list of concerned edges)
  * :const:`tlp.GraphEventType.TLP_BEFORE_ADD_DESCENDANTGRAPH` : a descendant graph (i.e. not necessarily a direct subgraph) is about to be added in the subgraphs hierarchy. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_AFTER_ADD_DESCENDANTGRAPH` : a descendant graph (i.e. not necessarily a direct subgraph) has been added in the subgraphs hierarchy. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_BEFORE_DEL_DESCENDANTGRAPH` : a descendant graph (i.e. not necessarily a direct subgraph) is about to be removed in the subgraphs hierarchy. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_AFTER_DEL_DESCENDANTGRAPH` : a descendant graph (i.e. not necessarily a direct subgraph) has been removed in the subgraphs hierarchy. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_BEFORE_ADD_SUBGRAPH` : a subgraph is about to be added in the graph. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_AFTER_ADD_SUBGRAPH` : a subgraph has been added in the graph. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_BEFORE_DEL_SUBGRAPH` : a subgraph is about to be removed in the graph. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_AFTER_DEL_SUBGRAPH` : a subgraph has been removed in the graph. Use :meth:`tlp.GraphEvent.getSubGraph` to get it.
  * :const:`tlp.GraphEventType.TLP_BEFORE_ADD_LOCAL_PROPERTY` : a local property is about to be added in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_ADD_LOCAL_PROPERTY` : a local property has been added in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_BEFORE_DEL_LOCAL_PROPERTY` : a local property is about to be deleted in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_AFTER_DEL_LOCAL_PROPERTY` : a local property has been deleted in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_BEFORE_ADD_INHERITED_PROPERTY` : an inherited property (not attached to the graph itself but one of its parent) is about to be added in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_ADD_INHERITED_PROPERTY` : an inherited property (not attached to the graph itself but one of its parent) has been added in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_BEFORE_DEL_INHERITED_PROPERTY` : an inherited property (not attached to the graph itself but one of its parent) is about to be deleted in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_AFTER_DEL_INHERITED_PROPERTY` : an inherited property (not attached to the graph itself but one of its parent) has been deleted in the graph. Use :meth:`tlp.GraphEvent.getPropertyName` to get the name of the concerned property.
  * :const:`tlp.GraphEventType.TLP_BEFORE_SET_ATTRIBUTE` : an attribute of the graph is about to be set/modified. Use :meth:`tlp.GraphEvent.getAttributeName` to get the name of the concerned attribute.
  * :const:`tlp.GraphEventType.TLP_AFTER_SET_ATTRIBUTE` : an attribute of the graph has been set/modified. Use :meth:`tlp.GraphEvent.getAttributeName` to get the name of the concerned attribute.
  * :const:`tlp.GraphEventType.TLP_REMOVE_ATTRIBUTE` : an attribute of the graph has been removed. Use :meth:`tlp.GraphEvent.getAttributeName` to get the name of the concerned attribute.
  * :const:`tlp.GraphEventType.TLP_BEFORE_RENAME_LOCAL_PROPERTY` : a local property of the graph is about to be renamed. Use :meth:`tlp.GraphEvent.getProperty` to get the concerned property and :meth:`tlp.GraphEvent.getPropertyNewName` to get the new property name.
  * :const:`tlp.GraphEventType.TLP_AFTER_RENAME_LOCAL_PROPERTY` : a local property of the graph has been renamed. Use :meth:`tlp.GraphEvent.getProperty` to get the concerned property and :meth:`tlp.GraphEvent.getPropertyOldName` to get the old property name.

%End

%TypeHeaderCode
#include <talipot/Graph.h>
%End

%ConvertToSubClassCode
  if (dynamic_cast<tlp::GraphEvent*>(sipCpp)) {
    sipType = sipFindType("tlp::GraphEvent");
  } else {
    sipType = NULL;
  }
%End

public:

  GraphEvent(const tlp::Graph& g, GraphEventType graphEvtType, uint id,
             tlp::EventType evtType = tlp::EventType::TLP_MODIFICATION);

  GraphEvent(const tlp::Graph& g, GraphEventType graphEvtType, const tlp::Graph* sg);

  GraphEvent(const tlp::Graph& g, GraphEventType graphEvtType,
             const std::string& str, tlp::EventType evtType = tlp::EventType::TLP_MODIFICATION);

  ~GraphEvent() ;

// ========================================================================================

  tlp::Graph* getGraph() const;
%Docstring
tlp.GraphEvent.getGraph()

Returns the graph that sent the event.

:rtype: :class:`tlp.Graph`
%End

// ========================================================================================

  tlp::node getNode() const;
%Docstring
tlp.GraphEvent.getNode()

Returns the node concerned by the event (if any, otherwise return an invalid node)

:rtype: :class:`tlp.node`
%End

// ========================================================================================

  tlp::edge getEdge() const;
%Docstring
tlp.GraphEvent.getEdge()

Returns the edge concerned by the event (if any, otherwise return an invalid edge)

:rtype: :class:`tlp.edge`
%End

// ========================================================================================

  const std::vector<tlp::node>& getNodes() const;
%Docstring
tlp.GraphEvent.getNodes()

Returns the nodes concerned by the event (if any, otherwise return an empty list)

:rtype: list of :class:`tlp.node`
%End

// ========================================================================================

  const std::vector<tlp::edge>& getEdges() const;
%Docstring
tlp.GraphEvent.getEdges()

Returns the edges concerned by the event (if any, otherwise return an empty list)

:rtype: list of :class:`tlp.edge`
%End

// ========================================================================================

  const tlp::Graph* getSubGraph() const;
%Docstring
tlp.GraphEvent.getSubGraph()

Returns the subgraph or descendant graph concerned by the event (if any, return :const:`None` otherwise)

:rtype: :class:`tlp.Graph`
%End

// ========================================================================================

  const std::string& getAttributeName() const;
%Docstring
tlp.GraphEvent.getAttributeName()

Returns the name of the attribute concerned by the event (if any, otherwise returns an empty string)

:rtype: string
%End

// ========================================================================================

    const std::string& getPropertyName() const;
%Docstring
tlp.GraphEvent.getPropertyName()

Returns the name of the property concerned by the event (if any, otherwise returns an empty string)

:rtype: string
%End

// ========================================================================================

    GraphEventType getType() const;
%Docstring
tlp.GraphEvent.getType()

Returns the type of graph event.

:rtype: :const:`tlp.GraphEventType.TLP_*` (cf. class description above)
%End

// ========================================================================================

  tlp::PropertyInterface* getProperty() const;
%Docstring
tlp.GraphEvent.getProperty()

Returns the property that is about to be or has been renamed.

:rtype: :class:`tlp.PropertyInterface`
%End

// ========================================================================================

    const std::string& getPropertyOldName() const;
%Docstring
tlp.GraphEvent.getPropertyOldName()

Returns the old name of the property when it has been renamed.

:rtype: string
%End

// ========================================================================================

  const std::string& getPropertyNewName() const;
%Docstring
tlp.GraphEvent.getPropertyNewName()

Returns the new name of the property that it is about to be renamed.

:rtype: string
%End

};

};
