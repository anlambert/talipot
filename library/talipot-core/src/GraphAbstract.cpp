/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
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
#include <talipot/StableIterator.h>
#include <talipot/GraphView.h>
#include <talipot/ConcatIterator.h>
#include <talipot/GraphTools.h>

using namespace std;
using namespace tlp;

const string metaGraphPropertyName = "viewMetaGraph";

//=========================================================================
GraphAbstract::GraphAbstract(Graph *supergraph, uint sgId)
    : supergraph(supergraph ? supergraph : this),
      root((supergraph == this) ? this : supergraph->getRoot()), subGraphToKeep(nullptr),
      metaGraphProperty(nullptr) {
  // get id
  if (supergraph != this) {
    id = static_cast<GraphImpl *>(getRoot())->getSubGraphId(sgId);
  }

  propertyContainer = new PropertyManager(this);
}
//=========================================================================
GraphAbstract::~GraphAbstract() {

  for (Graph *sg : subgraphs) {

    // avoid double free
    // in a push context, a 'deleted' graph (see delSubGraph)
    // may still have a non empty list of subgraphs
    if (sg->getSuperGraph() == this) {
      if (id == 0) {
        // indicates root destruction (see below)
        sg->id = 0;
      }

      delete sg;
    }
  }

  delete propertyContainer;

  if (id != 0) {
    static_cast<GraphImpl *>(getRoot())->freeSubGraphId(id);
  }
}
//=========================================================================
void GraphAbstract::clear() {
  delAllSubGraphs();

  vector<node> vnodes(nodes());
  for (auto n : vnodes) {
    delNode(n);
  }
}
//=========================================================================
void GraphAbstract::restoreSubGraph(Graph *sg) {
  subgraphs.push_back(sg);
  sg->setSuperGraph(this);
  if (sg == subGraphToKeep) {
    static_cast<GraphImpl *>(getRoot())->getSubGraphId(sg->getId());
    subGraphToKeep = nullptr;
  }
}
//=========================================================================
void GraphAbstract::setSubGraphToKeep(Graph *sg) {
  subGraphToKeep = sg;
}
//=========================================================================
Graph *GraphAbstract::addSubGraph(uint id, BooleanProperty *selection, const std::string &name) {
  Graph *tmp = new GraphView(this, selection, id);

  if (!name.empty()) {
    tmp->setAttribute("name", name);
  }

  notifyBeforeAddSubGraph(tmp);
  subgraphs.push_back(tmp);
  notifyAfterAddSubGraph(tmp);
  return tmp;
}
//=========================================================================
Graph *GraphAbstract::getNthSubGraph(uint n) const {

  if (n >= subgraphs.size()) {
    return nullptr;
  }

  return subgraphs[n];
}
//=========================================================================
uint GraphAbstract::numberOfDescendantGraphs() const {
  uint result = numberOfSubGraphs();

  for (auto *sg : subgraphs) {
    result += sg->numberOfDescendantGraphs();
  }

  return result;
}
//=========================================================================
void GraphAbstract::delSubGraph(Graph *toRemove) {
  // look for the graph we want to remove in the subgraphs
  auto it = std::find(subgraphs.begin(), subgraphs.end(), toRemove);

  assert(it != subgraphs.end());

  if (it != subgraphs.end()) {
    subGraphToKeep = nullptr;

    // remove from subgraphs
    notifyBeforeDelSubGraph(toRemove);
    subgraphs.erase(it);

    // add toRemove subgraphs
    for (Graph *sg : toRemove->subGraphs()) {
      restoreSubGraph(sg);
    }

    notifyAfterDelSubGraph(toRemove);

    // subGraphToKeep may have change on notifyAfterDelSubGraph
    // see GraphUpdatesRecorder::delSubGraph
    // in GraphUpdatesRecorder.cpp
    if (toRemove != subGraphToKeep) {
      // avoid deletion of toRemove subgraphs
      toRemove->clearSubGraphs();
      delete toRemove;
    } else {
      // toRemove is not deleted,
      // and its subgraphs list is not erased;
      // because it is registered into a GraphUpdatesRecorder
      // in order it can be restored on undo or redo
      toRemove->notifyDestroy();
      // free subgraph id which will be restored in case of undo
      // see restoreSubGraph
      static_cast<GraphImpl *>(getRoot())->freeSubGraphId(toRemove->getId());
      subGraphToKeep = nullptr;
    }
  }
}
//=========================================================================
void GraphAbstract::removeSubGraph(Graph *toRemove) {
  auto it = std::find(subgraphs.begin(), subgraphs.end(), toRemove);

  if (it != subgraphs.end()) {
    subgraphs.erase(it);
  }
  if (toRemove == subGraphToKeep) {
    // free subgraph id which will be restored in case of undo
    // see restoreSubGraph
    static_cast<GraphImpl *>(getRoot())->freeSubGraphId(toRemove->getId());
    subGraphToKeep = nullptr;
  }
}
//=========================================================================
void GraphAbstract::delAllSubGraphs() {
  while (subgraphs.size()) {
    auto *sg = subgraphs[0];
    static_cast<GraphAbstract *>(sg)->delAllSubGraphs();
    delSubGraph(sg);
  }
}
//=========================================================================
void GraphAbstract::delAllSubGraphs(Graph *toRemove) {
  if (this != toRemove->getSuperGraph() || this == toRemove) {
    // this==toRemove : root graph
    return;
  }

  static_cast<GraphAbstract *>(toRemove)->delAllSubGraphs();

  delSubGraph(toRemove);
}
//=========================================================================
void GraphAbstract::clearSubGraphs() {
  subgraphs.clear();
}
//=========================================================================
void GraphAbstract::setSuperGraph(Graph *sg) {
  supergraph = sg;
}
//=========================================================================
Iterator<Graph *> *GraphAbstract::getSubGraphs() const {
  return stlIterator(subgraphs);
}
//=========================================================================
bool GraphAbstract::isSubGraph(const Graph *sg) const {
  return (std::find(subgraphs.begin(), subgraphs.end(), sg) != subgraphs.end());
}
//=========================================================================
bool GraphAbstract::isDescendantGraph(const Graph *g) const {
  if (isSubGraph(g)) {
    return true;
  }

  for (auto *sg : subgraphs) {
    if (sg->isDescendantGraph(g)) {
      return true;
    }
  }

  return false;
}
//=========================================================================
Graph *GraphAbstract::getSubGraph(uint sgId) const {
  for (auto *sg : subgraphs) {
    if (sg->getId() == sgId) {
      return sg;
    }
  }

  return nullptr;
}
//=========================================================================
Graph *GraphAbstract::getSubGraph(const std::string &name) const {
  for (auto *sg : subgraphs) {
    if (sg->getName() == name) {
      return sg;
    }
  }

  return nullptr;
}
//=========================================================================
Graph *GraphAbstract::getDescendantGraph(uint sgId) const {
  Graph *sg = getSubGraph(sgId);

  if (sg) {
    return sg;
  }

  for (auto *sg : subgraphs) {
    if ((sg = sg->getDescendantGraph(sgId))) {
      return sg;
    }
  }

  return nullptr;
}
//=========================================================================
Graph *GraphAbstract::getDescendantGraph(const string &name) const {
  Graph *sg = getSubGraph(name);

  if (sg) {
    return sg;
  }

  for (auto *sg : subgraphs) {
    if ((sg = sg->getDescendantGraph(name))) {
      return sg;
    }
  }

  return nullptr;
}
//=========================================================================
node GraphAbstract::getOneNode() const {
  const std::vector<node> &vNodes = nodes();
  return (vNodes.size() > 0) ? vNodes[0] : node();
}
//=========================================================================
node GraphAbstract::getRandomNode() const {
  const std::vector<node> &vNodes = nodes();

  if (!vNodes.empty()) {
    return vNodes[randomUnsignedInteger(vNodes.size() - 1)];
  }

  return node();
}
//=========================================================================
edge GraphAbstract::getOneEdge() const {
  const std::vector<edge> &vEdges = edges();
  return (vEdges.size() > 0) ? vEdges[0] : edge();
}
//=========================================================================
edge GraphAbstract::getRandomEdge() const {
  const std::vector<edge> &vEdges = edges();

  if (!vEdges.empty()) {
    return vEdges[randomUnsignedInteger(vEdges.size() - 1)];
  }

  return edge();
}
//=========================================================================
node GraphAbstract::getInNode(const node n, uint i) const {
  assert(i <= indeg(n) && i > 0);
  node result;
  uint cpt = 0;
  for (auto n : getInNodes(n)) {
    if (++cpt == i) {
      result = n;
      break;
    }
  }
  return result;
}
//=========================================================================
node GraphAbstract::getOutNode(const node n, uint i) const {
  assert(i <= outdeg(n) && i > 0);
  node result;
  uint cpt = 0;
  for (auto n : getOutNodes(n)) {
    if (++cpt == i) {
      result = n;
      break;
    }
  }
  return result;
}
//=========================================================================
void GraphAbstract::delNodes(const std::vector<node> &nodes, bool deleteInAllGraphs) {
  for (auto n : nodes) {
    delNode(n, deleteInAllGraphs);
  }
}
//=========================================================================
void GraphAbstract::delEdges(const std::vector<edge> &edges, bool deleteInAllGraphs) {
  for (auto e : edges) {
    delEdge(e, deleteInAllGraphs);
  }
}
//=========================================================================
bool GraphAbstract::existProperty(const std::string &name) const {
  return propertyContainer->existProperty(name);
}
//=========================================================================
bool GraphAbstract::existLocalProperty(const std::string &name) const {
  return propertyContainer->existLocalProperty(name);
}
//=========================================================================
PropertyInterface *GraphAbstract::getProperty(const string &str) const {
  return propertyContainer->getProperty(str);
}
//=========================================================================
void GraphAbstract::delLocalProperty(const std::string &name) {
  std::string nameCopy = name; // the name is copied to ensure that the notifyBeforeDel event will
                               // not use an invalid reference
  assert(existLocalProperty(nameCopy));
  notifyBeforeDelLocalProperty(nameCopy);
  propertyContainer->delLocalProperty(nameCopy);
  notifyAfterDelLocalProperty(nameCopy);
}
//=========================================================================
void GraphAbstract::addLocalProperty(const std::string &name, PropertyInterface *prop) {
  assert(!existLocalProperty(name));
  notifyBeforeAddLocalProperty(name);
  propertyContainer->setLocalProperty(name, prop);

  if (name == metaGraphPropertyName) {
    metaGraphProperty = static_cast<GraphProperty *>(prop);
  }

  notifyAddLocalProperty(name);
}
//=========================================================================
bool GraphAbstract::renameLocalProperty(PropertyInterface *prop, const std::string &newName) {
  return propertyContainer->renameLocalProperty(prop, newName);
}
//=========================================================================
void GraphAbstract::notifyAddInheritedProperty(const std::string &propName) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_ADD_INHERITED_PROPERTY, propName));
  }
}
void GraphAbstract::notifyBeforeAddInheritedProperty(const std::string &propName) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_ADD_INHERITED_PROPERTY, propName));
  }
}
//=========================================================================
void GraphAbstract::notifyBeforeDelInheritedProperty(const std::string &propName) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_DEL_INHERITED_PROPERTY, propName,
                         Event::TLP_INFORMATION));
  }
}
//=========================================================================
void GraphAbstract::notifyAfterDelInheritedProperty(const std::string &name) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_DEL_INHERITED_PROPERTY, name));
  }
}
//=========================================================================
void GraphAbstract::notifyBeforeRenameLocalProperty(PropertyInterface *prop,
                                                    const std::string &newName) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_BEFORE_RENAME_LOCAL_PROPERTY, prop, newName));
  }
}
//=========================================================================
void GraphAbstract::notifyAfterRenameLocalProperty(PropertyInterface *prop,
                                                   const std::string &oldName) {
  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEvent::TLP_AFTER_RENAME_LOCAL_PROPERTY, prop, oldName));
  }
}
//=========================================================================
Iterator<std::string> *GraphAbstract::getLocalProperties() const {
  return propertyContainer->getLocalProperties();
}
//=========================================================================
Iterator<std::string> *GraphAbstract::getInheritedProperties() const {
  return propertyContainer->getInheritedProperties();
}
//=========================================================================
Iterator<std::string> *GraphAbstract::getProperties() const {
  return concatIterator(propertyContainer->getLocalProperties(),
                        propertyContainer->getInheritedProperties());
}
//=========================================================================
Iterator<PropertyInterface *> *GraphAbstract::getLocalObjectProperties() const {
  return propertyContainer->getLocalObjectProperties();
}
//=========================================================================
Iterator<PropertyInterface *> *GraphAbstract::getInheritedObjectProperties() const {
  return propertyContainer->getInheritedObjectProperties();
}
//=========================================================================
Iterator<PropertyInterface *> *GraphAbstract::getObjectProperties() const {
  return concatIterator(propertyContainer->getLocalObjectProperties(),
                        propertyContainer->getInheritedObjectProperties());
}
//=========================================================================
bool GraphAbstract::isMetaNode(const node n) const {
  assert(isElement(n));
  return metaGraphProperty && metaGraphProperty->hasNonDefaultValue(n);
}
//----------------------------------------------------------------
bool GraphAbstract::isMetaEdge(const edge e) const {
  assert(isElement(e));
  return metaGraphProperty && metaGraphProperty->hasNonDefaultValue(e);
}
//=========================================================================
Graph *GraphAbstract::getNodeMetaInfo(const node n) const {
  if (metaGraphProperty) {
    return (*metaGraphProperty)[n];
  }

  return nullptr;
}

// only used to return a reference on an empty vector of edges
static set<edge> noReferencedEdges;
//=========================================================================
const set<edge> &GraphAbstract::getReferencedEdges(const edge e) const {
  if (metaGraphProperty) {
    return metaGraphProperty->getReferencedEdges(e);
  } else {
    return noReferencedEdges;
  }
}

Iterator<edge> *GraphAbstract::getEdgeMetaInfo(const edge e) const {
  return stlIterator(getReferencedEdges(e));
}

GraphProperty *GraphAbstract::getMetaGraphProperty() {
  if (metaGraphProperty) {
    return metaGraphProperty;
  }

  return metaGraphProperty = getRoot()->getGraphProperty(metaGraphPropertyName);
}

void GraphAbstract::setName(const std::string &name) {
  setAttribute("name", name);
}

std::string GraphAbstract::getName() const {
  std::string name;
  getAttribute("name", name);
  return name;
}

Iterator<node> *GraphAbstract::bfs(const node root, bool directed) const {
  return stableIterator(tlp::bfs(this, root, directed));
}

Iterator<node> *GraphAbstract::dfs(const node root, bool directed) const {
  return stableIterator(tlp::dfs(this, root, directed));
}

Iterator<edge> *GraphAbstract::bfsEdges(const node root, bool directed) const {
  return stableIterator(tlp::bfsEdges(this, root, directed));
}

Iterator<edge> *GraphAbstract::dfsEdges(const node root, bool directed) const {
  return stableIterator(tlp::dfsEdges(this, root, directed));
}
