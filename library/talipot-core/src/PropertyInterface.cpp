/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Graph.h>

using namespace tlp;

PropertyInterface::PropertyInterface() : graph(nullptr) {}

PropertyInterface::~PropertyInterface() {
  // check if the current property is not registered
  // as a property of a graph
  if (graph && !name.empty() && graph->existLocalProperty(name) &&
      graph->getProperty(name) == this) {
    tlp::warning() << "Warning : " << __PRETTY_FUNCTION__
                   << " ... Serious bug; you have deleted a registered graph property named '"
                   << name.c_str() << "'" << std::endl;
    abort();
  }

  observableDeleted();
}

bool PropertyInterface::rename(const std::string &newName) {
  return graph && graph->renameLocalProperty(this, newName);
}

void PropertyInterface::notifyBeforeSetNodeValue(const node n) {
  if (hasOnlookers() && getGraph()->isElement(n)) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_BEFORE_SET_NODE_VALUE,
                            EventType::TLP_INFORMATION, n));
  }
}

void PropertyInterface::notifyAfterSetNodeValue(const node n) {
  if (hasOnlookers() && getGraph()->isElement(n)) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_AFTER_SET_NODE_VALUE,
                            EventType::TLP_MODIFICATION, n));
  }
}

void PropertyInterface::notifyBeforeSetEdgeValue(const edge e) {
  if (hasOnlookers() && getGraph()->isElement(e)) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_BEFORE_SET_EDGE_VALUE,
                            EventType::TLP_INFORMATION, e));
  }
}

void PropertyInterface::notifyAfterSetEdgeValue(const edge e) {
  if (hasOnlookers() && getGraph()->isElement(e)) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_AFTER_SET_EDGE_VALUE,
                            EventType::TLP_MODIFICATION, e));
  }
}

void PropertyInterface::notifyBeforeSetAllNodeValue() {
  if (hasOnlookers()) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_BEFORE_SET_ALL_NODE_VALUE,
                            EventType::TLP_INFORMATION));
  }
}

void PropertyInterface::notifyAfterSetAllNodeValue() {
  if (hasOnlookers()) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_AFTER_SET_ALL_NODE_VALUE,
                            EventType::TLP_MODIFICATION));
  }
}

void PropertyInterface::notifyBeforeSetAllEdgeValue() {
  if (hasOnlookers()) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_BEFORE_SET_ALL_EDGE_VALUE,
                            EventType::TLP_INFORMATION));
  }
}

void PropertyInterface::notifyAfterSetAllEdgeValue() {
  if (hasOnlookers()) {
    sendEvent(PropertyEvent(*this, PropertyEventType::TLP_AFTER_SET_ALL_EDGE_VALUE,
                            EventType::TLP_MODIFICATION));
  }
}

void PropertyInterface::notifyDestroy() {
  if (hasOnlookers()) {
    // the undo/redo mechanism has to simulate graph destruction
    Event evt(*this, EventType::TLP_MODIFICATION);
    evt._type = EventType::TLP_DELETE;
    sendEvent(evt);
  }
}

// define key function to ensure dynamic_cast on PropertyEvent type
// will work across dlopen boundaries
PropertyEvent::~PropertyEvent() {}
