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

#include "PropertyValuesDispatcher.h"

#include <talipot/IntegerProperty.h>
#include <talipot/BooleanProperty.h>

using namespace std;

namespace tlp {

PropertyValuesDispatcher::PropertyValuesDispatcher(
    tlp::Graph *source, tlp::Graph *target, const std::set<std::string> &sourceToTargetProperties,
    const std::set<std::string> &targetToSourceProperties,
    tlp::IntegerVectorProperty *graphEntitiesToDisplayedNodes,
    tlp::BooleanProperty *displayedNodesAreNodes,
    tlp::IntegerProperty *displayedNodesToGraphEntities,
    tlp::IntegerProperty *displayedEdgesToGraphEdges, QHash<tlp::edge, tlp::edge> &edgesMap)
    : _source(source), _target(target),
      _graphEntitiesToDisplayedNodes(graphEntitiesToDisplayedNodes),
      _displayedNodesAreNodes(displayedNodesAreNodes),
      _displayedNodesToGraphEntities(displayedNodesToGraphEntities),
      _displayedEdgesToGraphEdges(displayedEdgesToGraphEdges), _edgesMap(edgesMap),
      _sourceToTargetProperties(sourceToTargetProperties),
      _targetToSourceProperties(targetToSourceProperties), _modifying(false) {
  assert(source);
  assert(target);
  assert(graphEntitiesToDisplayedNodes);
  assert(displayedNodesAreNodes);
  assert(displayedNodesToGraphEntities);

  Observable::holdObservers();
  for (const string &s : source->getProperties()) {
    addLocalProperty(source, s);
  }

  for (const string &s : target->getProperties()) {
    addLocalProperty(target, s);
  }

  Observable::unholdObservers();

  source->addListener(this);
  target->addListener(this);
}

void PropertyValuesDispatcher::afterSetNodeValue(tlp::PropertyInterface *sourceProp,
                                                 const tlp::node n) {
  if (_modifying) {
    return;
  }

  _modifying = true;

  if (sourceProp->getGraph()->getRoot() == _source->getRoot()) {
    PropertyInterface *targetProp = _target->getProperty(sourceProp->getName());
    std::string strVal = sourceProp->getNodeStringValue(n);
    const vector<int> &vect = _graphEntitiesToDisplayedNodes->getNodeValue(n);

    for (auto id : vect) {
      targetProp->setNodeStringValue(node(id), strVal);
    }
  } else if (sourceProp->getGraph()->getRoot() == _target->getRoot()) {
    PropertyInterface *targetProp = _source->getProperty(sourceProp->getName());
    uint id = _displayedNodesToGraphEntities->getNodeValue(n);

    if (_displayedNodesAreNodes->getNodeValue(n)) {
      targetProp->setNodeStringValue(node(id), sourceProp->getNodeStringValue(n));
      // update the other node
      const vector<int> &vect = _graphEntitiesToDisplayedNodes->getNodeValue(node(id));

      for (auto oid : vect) {
        node n1(oid);

        if (n1 != n) {
          sourceProp->setNodeStringValue(n1, sourceProp->getNodeStringValue(n));
        }
      }
    } else {
      targetProp->setEdgeStringValue(edge(id), sourceProp->getNodeStringValue(n));
      sourceProp->setEdgeStringValue(_edgesMap[edge(id)], sourceProp->getNodeStringValue(n));

      const vector<int> &vect = _graphEntitiesToDisplayedNodes->getEdgeValue(edge(id));

      for (auto oid : vect) {
        node n1(oid);

        if (n1 != n) {
          sourceProp->setNodeStringValue(n1, sourceProp->getNodeStringValue(n));
        }
      }
    }
  }

  _modifying = false;
}

void PropertyValuesDispatcher::afterSetEdgeValue(tlp::PropertyInterface *sourceProp,
                                                 const tlp::edge e) {
  if (_modifying) {
    return;
  }

  _modifying = true;

  if (sourceProp->getGraph()->getRoot() == _source->getRoot()) {
    PropertyInterface *targetProp = _target->getProperty(sourceProp->getName());
    std::string strVal = sourceProp->getEdgeStringValue(e);
    const vector<int> &vect = _graphEntitiesToDisplayedNodes->getEdgeValue(e);

    for (auto id : vect) {
      targetProp->setNodeStringValue(node(id), strVal);
    }

    edge ee = _edgesMap[e];

    // corresponding edge may not exist if e
    // has been added after the build of the MatrixView
    if (ee.isValid()) {
      targetProp->setEdgeStringValue(_edgesMap[e], sourceProp->getEdgeStringValue(e));
    }
  } else if (sourceProp->getGraph()->getRoot() == _target->getRoot()) {
    PropertyInterface *targetProp = _source->getProperty(sourceProp->getName());
    uint id = _displayedEdgesToGraphEdges->getEdgeValue(e);
    std::string strVal = sourceProp->getEdgeStringValue(e);
    targetProp->setEdgeStringValue(edge(id), strVal);

    const vector<int> &vect = _graphEntitiesToDisplayedNodes->getEdgeValue(edge(id));

    for (auto oid : vect) {
      sourceProp->setNodeStringValue(node(oid), strVal);
    }
  }

  _modifying = false;
}

void PropertyValuesDispatcher::afterSetAllNodeValue(tlp::PropertyInterface *sourceProp) {
  if (sourceProp->getGraph()->getRoot() == _source->getRoot()) {
    PropertyInterface *targetProp = _target->getProperty(sourceProp->getName());
    string val = sourceProp->getNodeDefaultStringValue();
    for (auto n : _displayedNodesAreNodes->getNodesEqualTo(true)) {
      targetProp->setNodeStringValue(n, val);
    }
  } else if (sourceProp->getGraph()->getRoot() == _target->getRoot()) {
    PropertyInterface *targetProp = _source->getProperty(sourceProp->getName());
    targetProp->setAllNodeStringValue(sourceProp->getNodeDefaultStringValue());
    targetProp->setAllEdgeStringValue(sourceProp->getNodeDefaultStringValue());
  }
}

void PropertyValuesDispatcher::afterSetAllEdgeValue(tlp::PropertyInterface *sourceProp) {
  if (sourceProp->getGraph()->getRoot() == _source->getRoot()) {
    PropertyInterface *targetProp = _target->getProperty(sourceProp->getName());
    string val = sourceProp->getEdgeDefaultStringValue();
    for (auto n : _displayedNodesAreNodes->getNodesEqualTo(false)) {
      targetProp->setNodeStringValue(n, val);
    }
  } else if (sourceProp->getGraph()->getRoot() == _target->getRoot()) {
    PropertyInterface *targetProp = _source->getProperty(sourceProp->getName());
    targetProp->setAllEdgeStringValue(sourceProp->getNodeDefaultStringValue());
  }
}

void PropertyValuesDispatcher::addLocalProperty(tlp::Graph *g, const std::string &name) {
  if ((g == _source && _sourceToTargetProperties.contains(name)) ||
      (g == _target && _targetToSourceProperties.contains(name))) {
    Observable::holdObservers();
    PropertyInterface *sourceProp = g->getProperty(name);
    afterSetAllNodeValue(sourceProp);
    afterSetAllEdgeValue(sourceProp);
    for (auto n : sourceProp->getNonDefaultValuatedNodes()) {
      afterSetNodeValue(sourceProp, n);
    }

    for (auto e : sourceProp->getNonDefaultValuatedEdges()) {
      afterSetEdgeValue(sourceProp, e);
    }
    Observable::unholdObservers();

    sourceProp->addListener(this);
  }
}

void PropertyValuesDispatcher::treatEvent(const tlp::Event &evt) {
  if (typeid(evt) == typeid(GraphEvent)) {
    const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);
    Graph *graph = gEvt->getGraph();

    if (gEvt->getType() == GraphEventType::TLP_ADD_LOCAL_PROPERTY) {
      addLocalProperty(graph, gEvt->getPropertyName());
    }
  } else {
    const auto *propEvt = dynamic_cast<const PropertyEvent *>(&evt);

    if (propEvt) {
      PropertyInterface *prop = propEvt->getProperty();

      switch (propEvt->getType()) {
      case PropertyEventType::TLP_AFTER_SET_NODE_VALUE:
        afterSetNodeValue(prop, propEvt->getNode());
        return;

      case PropertyEventType::TLP_AFTER_SET_ALL_NODE_VALUE:
        afterSetAllNodeValue(prop);
        return;

      case PropertyEventType::TLP_AFTER_SET_ALL_EDGE_VALUE:
        afterSetAllEdgeValue(prop);
        return;

      case PropertyEventType::TLP_AFTER_SET_EDGE_VALUE:
        afterSetEdgeValue(prop, propEvt->getEdge());
        return;

      default:
        return;
      }
    }
  }
}
}
