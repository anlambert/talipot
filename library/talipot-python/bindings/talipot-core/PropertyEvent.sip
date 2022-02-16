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

enum class PropertyEventType {
  TLP_BEFORE_SET_NODE_VALUE = 0,
  TLP_AFTER_SET_NODE_VALUE,
  TLP_BEFORE_SET_ALL_NODE_VALUE,
  TLP_AFTER_SET_ALL_NODE_VALUE,
  TLP_BEFORE_SET_ALL_EDGE_VALUE,
  TLP_AFTER_SET_ALL_EDGE_VALUE,
  TLP_BEFORE_SET_EDGE_VALUE,
  TLP_AFTER_SET_EDGE_VALUE
};

class PropertyEvent : tlp::Event {
%Docstring
That class represents an event that happened on a :class:`tlp.PropertyInterface` derived instance.
To get the property that sent the event, use the :meth:`tlp.PropertyEvent.getProperty`.
The type of property event can be retrieved through the :meth:`tlp.PropertyEvent.getType` method.
Below is an exhaustive list of those types :

  * :const:`tlp.PropertyEventType.TLP_BEFORE_SET_NODE_VALUE` : the value of a node is about to be modified. Use :meth:`tlp.PropertyEvent.getNode()` to get the concerned node.
  * :const:`tlp.PropertyEventType.TLP_AFTER_SET_NODE_VALUE` : the value of a node has been modified. Use :meth:`tlp.PropertyEvent.getNode()` to get the concerned node.
  * :const:`tlp.PropertyEventType.TLP_BEFORE_SET_EDGE_VALUE` : the value of an edge is about to be modified. Use :meth:`tlp.PropertyEvent.getEdge()` to get the concerned edge.
  * :const:`tlp.PropertyEventType.TLP_AFTER_SET_EDGE_VALUE` : the value of an edge has been modified. Use :meth:`tlp.PropertyEvent.getEdge()` to get the concerned edge.
  * :const:`tlp.PropertyEventType.TLP_BEFORE_SET_ALL_NODE_VALUE` : the value of all nodes is about to be modified.
  * :const:`tlp.PropertyEventType.TLP_AFTER_SET_ALL_NODE_VALUE` : the value of all nodes has been modified.
  * :const:`tlp.PropertyEventType.TLP_BEFORE_SET_ALL_EDGE_VALUE` : the value of all edges is about to be modified.
  * :const:`tlp.PropertyEventType.TLP_AFTER_SET_ALL_EDGE_VALUE` : the value of all edges has been modified.

%End

%TypeHeaderCode
#include <talipot/PropertyInterface.h>
%End

%ConvertToSubClassCode
  if (dynamic_cast<tlp::PropertyEvent*>(sipCpp)) {
    sipType = sipFindType("tlp::PropertyEvent");
  } else {
    sipType = NULL;
  }
%End

public:

  PropertyEvent(const tlp::PropertyInterface& prop, PropertyEventType propEvtType,
                tlp::EventType evtType = tlp::EventType::TLP_MODIFICATION,
                uint id = UINT_MAX);

// ========================================================================================

  tlp::PropertyInterface* getProperty() const;
%Docstring
tlp.PropertyEvent.getProperty()

Returns the property concerned by the event.

:rtype: derived instances of :class:`tlp.PropertyInterface`
%End

// ========================================================================================

  tlp::node getNode() const;
%Docstring
tlp.PropertyEvent.getNode()

Returns the node concerned by the event (if any, otherwise return an invalid node)

:rtype: :class:`tlp.node`
%End

// ========================================================================================

  tlp::edge getEdge() const;
%Docstring
tlp.PropertyEvent.getEdge()

Returns the edge concerned by the event (if any, otherwise return an invalid edge)

:rtype: :class:`tlp.edge`
%End

// ========================================================================================

  PropertyEventType getType() const;
%Docstring
tlp.Graph.getType()

Returns the type of graph event.

:rtype: :const:`tlp.GraphEventType.TLP_*` (cf. class description above)
%End

};

};
