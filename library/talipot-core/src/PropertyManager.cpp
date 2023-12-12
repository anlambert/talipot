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

#include <talipot/GraphAbstract.h>
#include <talipot/PropertyManager.h>
#include <talipot/GraphProperty.h>

namespace tlp {

using namespace std;
using namespace tlp;

const string metaGraphPropertyName = "viewMetaGraph";

//==============================================================
PropertyManager::PropertyManager(Graph *g) : graph(g) {
  // get inherited properties
  if (graph != graph->getSuperGraph()) {

    for (PropertyInterface *prop : graph->getSuperGraph()->getObjectProperties()) {
      inheritedProperties[prop->getName()] = prop;

      if (prop->getName() == metaGraphPropertyName) {
        static_cast<GraphAbstract *>(graph)->metaGraphProperty = static_cast<GraphProperty *>(prop);
      }
    }
  }
}
//==============================================================
PropertyManager::~PropertyManager() {
  for (const auto &[name, property] : localProperties) {
    property->graph = nullptr;
    delete property;
  }
}
//==============================================================
bool PropertyManager::existProperty(const string &str) const {
  return existLocalProperty(str) || existInheritedProperty(str);
}
//==============================================================
bool PropertyManager::existLocalProperty(const string &str) const {
  return localProperties.contains(str);
}
//==============================================================
bool PropertyManager::existInheritedProperty(const string &str) const {
  return inheritedProperties.contains(str);
}
//==============================================================
void PropertyManager::setLocalProperty(const string &str, PropertyInterface *p) {
  bool hasInheritedProperty = false;

  if (existLocalProperty(str)) {
    // delete previously existing local property
    delete localProperties[str];
  } else {
    // remove previously existing inherited property
    auto it = inheritedProperties.find(str);
    hasInheritedProperty = it != inheritedProperties.end();

    if (hasInheritedProperty) {
      // Notify property destruction old state.
      notifyBeforeDelInheritedProperty(str);
      // Erase old inherited property
      inheritedProperties.erase(it);
    }
  }

  // register property as local
  localProperties[str] = p;

  // If we had an inherited property notify it's destruction.
  if (hasInheritedProperty) {
    static_cast<GraphAbstract *>(graph)->notifyAfterDelInheritedProperty(str);
  }

  // loop on subgraphs
  for (Graph *sg : graph->subGraphs()) {
    // to set p as inherited property
    static_cast<GraphAbstract *>(sg)->propertyContainer->setInheritedProperty(str, p);
  }
}
//==============================================================
bool PropertyManager::renameLocalProperty(PropertyInterface *prop, const string &newName) {
  assert(prop && prop->getGraph() == graph);

  if (existLocalProperty(newName)) {
    return false;
  }

  std::string propName = prop->getName();
  auto it = localProperties.find(propName);

  if (it == localProperties.end()) {
    return false;
  }

  assert(it->second == prop);

  // before rename notification
  static_cast<GraphAbstract *>(graph)->notifyBeforeRenameLocalProperty(prop, newName);

  // loop in the ascendant hierarchy to get
  // an inherited property
  PropertyInterface *newProp = nullptr;
  Graph *g = graph;

  while (g != g->getSuperGraph()) {
    g = g->getSuperGraph();

    if (g->existLocalProperty(propName)) {
      newProp = g->getProperty(propName);
      break;
    }
  }

  // Warn subgraphs for deletion.
  for (Graph *sg : graph->subGraphs()) {
    static_cast<GraphAbstract *>(sg)->propertyContainer->notifyBeforeDelInheritedProperty(propName);
  }

  // Remove property from map.
  localProperties.erase(it);
  // Set the inherited property in this graph and all it's subgraphs.
  static_cast<GraphAbstract *>(graph)->propertyContainer->setInheritedProperty(propName, newProp);

  // remove previously existing inherited property
  bool hasInheritedProperty =
      ((it = inheritedProperties.find(newName)) != inheritedProperties.end());

  if (hasInheritedProperty) {
    // Notify property destruction old state.
    notifyBeforeDelInheritedProperty(newName);
    // Erase old inherited property
    inheritedProperties.erase(it);
  }

  // register property as local
  localProperties[newName] = prop;

  // If we had an inherited property notify it's destruction.
  if (hasInheritedProperty) {
    static_cast<GraphAbstract *>(graph)->notifyAfterDelInheritedProperty(newName);
  }

  // loop on subgraphs
  for (Graph *sg : graph->subGraphs()) {
    // to set p as inherited property
    static_cast<GraphAbstract *>(sg)->propertyContainer->setInheritedProperty(newName, prop);
  }

  // update property name
  prop->name = newName;

  // after renaming notification
  static_cast<GraphAbstract *>(graph)->notifyAfterRenameLocalProperty(prop, propName);

  return true;
}
//==============================================================
void PropertyManager::setInheritedProperty(const string &str, PropertyInterface *p) {
  if (!existLocalProperty(str)) {
    bool hasInheritedProperty = inheritedProperties.contains(str);

    if (p != nullptr) {
      static_cast<GraphAbstract *>(graph)->notifyBeforeAddInheritedProperty(str);
      inheritedProperties[str] = p;

      if (str == metaGraphPropertyName) {
        static_cast<GraphAbstract *>(graph)->metaGraphProperty = static_cast<GraphProperty *>(p);
      }
    } else {
      // no need for notification
      // already done through notifyBeforeDelInheritedProperty(str);
      // see setLocalProperty
      inheritedProperties.erase(str);
    }

    if (hasInheritedProperty) {
      static_cast<GraphAbstract *>(graph)->notifyAfterDelInheritedProperty(str);
    }

    // graph observers notification
    if (p != nullptr) {
      static_cast<GraphAbstract *>(graph)->notifyAddInheritedProperty(str);
    }

    // loop on subgraphs
    for (Graph *sg : graph->subGraphs()) {
      // to set p as inherited property
      static_cast<GraphAbstract *>(sg)->propertyContainer->setInheritedProperty(str, p);
    }
  }
}
//==============================================================
PropertyInterface *PropertyManager::getProperty(const string &str) const {
  assert(existProperty(str));

  if (existLocalProperty(str)) {
    return getLocalProperty(str);
  }

  if (existInheritedProperty(str)) {
    return getInheritedProperty(str);
  }

  return nullptr;
}
//==============================================================
PropertyInterface *PropertyManager::getLocalProperty(const string &str) const {
  assert(existLocalProperty(str));
  return const_cast<PropertyManager *>(this)->localProperties[str];
}
//==============================================================
PropertyInterface *PropertyManager::getInheritedProperty(const string &str) const {
  assert(existInheritedProperty(str));
  return const_cast<PropertyManager *>(this)->inheritedProperties[str];
}
//==============================================================
void PropertyManager::delLocalProperty(const string &str) {
  // if found remove from local properties
  if (const auto it = localProperties.find(str); it != localProperties.end()) {
    auto [name, oldProp] = *it;

    // loop in the ascendant hierarchy to get
    // an inherited property
    PropertyInterface *newProp = nullptr;
    Graph *g = graph;

    while (g != g->getSuperGraph()) {
      g = g->getSuperGraph();

      if (g->existLocalProperty(str)) {
        newProp = g->getProperty(str);
        break;
      }
    }

    // Warn subgraphs.
    for (Graph *sg : graph->subGraphs()) {
      static_cast<GraphAbstract *>(sg)->propertyContainer->notifyBeforeDelInheritedProperty(str);
    }

    // Remove property from map.
    localProperties.erase(it);
    // Set the inherited property in this graph and all it's subgraphs.
    static_cast<GraphAbstract *>(graph)->propertyContainer->setInheritedProperty(str, newProp);

    // Delete property
    // Need to be done after subgraph notification.
    if (graph->canDeleteProperty(graph, oldProp)) {
      // if (!graph->canPop())
      delete oldProp;
    } else {
      oldProp->notifyDestroy();
    }
  }
}
//==============================================================
void PropertyManager::notifyBeforeDelInheritedProperty(const string &str) {
  // if found remove from inherited properties
  if (const auto it = inheritedProperties.find(str); it != inheritedProperties.end()) {
    // graph observers notification
    static_cast<GraphAbstract *>(graph)->notifyBeforeDelInheritedProperty(str);
    // loop on subgraphs
    for (Graph *sg : graph->subGraphs()) {
      // to remove as inherited property
      static_cast<GraphAbstract *>(sg)->propertyContainer->notifyBeforeDelInheritedProperty(str);
    }
  }
}

Iterator<string> *PropertyManager::getLocalProperties() {
  return stlMapKeyIterator(localProperties);
}
Iterator<string> *PropertyManager::getInheritedProperties() {
  return stlMapKeyIterator(inheritedProperties);
}
Iterator<PropertyInterface *> *PropertyManager::getLocalObjectProperties() {
  return stlMapValueIterator(localProperties);
}
Iterator<PropertyInterface *> *PropertyManager::getInheritedObjectProperties() {
  return stlMapValueIterator(inheritedProperties);
}
//===============================================================
void PropertyManager::erase(const node n) {
  for (const auto &[name, property] : localProperties) {
    property->erase(n);
  }
}
//===============================================================
void PropertyManager::erase(const edge e) {
  for (const auto &[name, property] : localProperties) {
    property->erase(e);
  }
}
}
