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

#include <talipot/GraphUpdatesRecorder.h>
#include <talipot/GraphImpl.h>

using namespace std;
using namespace tlp;

GraphUpdatesRecorder::GraphUpdatesRecorder(bool allowRestart,
                                           const GraphStorageIdsMemento *prevIdsMemento)
    :
#if !defined(NDEBUG)
      recordingStopped(true),
#endif
      updatesReverted(false), restartAllowed(allowRestart), newValuesRecorded(false),
      oldIdsStateRecorded(prevIdsMemento == nullptr), oldIdsState(prevIdsMemento),
      newIdsState(nullptr) {
}

GraphUpdatesRecorder::~GraphUpdatesRecorder() {
  deleteDeletedObjects();
  deleteValues(oldValues);
  deleteValues(newValues);
  deleteDefaultValues(oldNodeDefaultValues);
  deleteDefaultValues(newNodeDefaultValues);
  deleteDefaultValues(oldEdgeDefaultValues);
  deleteDefaultValues(newEdgeDefaultValues);

  if (oldIdsStateRecorded) {
    delete oldIdsState;
  }

  delete newIdsState;
}

void GraphUpdatesRecorder::treatEvent(const Event &ev) {
  if (typeid(ev) == typeid(GraphEvent)) {
    const auto *gEvt = static_cast<const GraphEvent *>(&ev);
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_ADD_NODE:
      addNode(graph, gEvt->getNode());
      break;

    case GraphEventType::TLP_DEL_NODE:
      delNode(graph, gEvt->getNode());
      break;

    case GraphEventType::TLP_ADD_EDGE:
      addEdge(graph, gEvt->getEdge());
      break;

    case GraphEventType::TLP_DEL_EDGE:
      delEdge(graph, gEvt->getEdge());
      break;

    case GraphEventType::TLP_REVERSE_EDGE:
      reverseEdge(graph, gEvt->getEdge());
      break;

    case GraphEventType::TLP_BEFORE_SET_ENDS:
      beforeSetEnds(graph, gEvt->getEdge());
      break;

    case GraphEventType::TLP_AFTER_SET_ENDS:
      afterSetEnds(graph, gEvt->getEdge());
      break;

    case GraphEventType::TLP_ADD_NODES: {
      const std::vector<node> &nodes = graph->nodes();

      for (uint i = nodes.size() - gEvt->getNumberOfNodes(); i < nodes.size(); ++i) {
        addNode(graph, nodes[i]);
      }

      break;
    }

    case GraphEventType::TLP_ADD_EDGES:
      addEdges(graph, gEvt->getNumberOfEdges());
      break;

    case GraphEventType::TLP_AFTER_ADD_SUBGRAPH:
      addSubGraph(graph, const_cast<Graph *>(gEvt->getSubGraph()));
      break;

    case GraphEventType::TLP_AFTER_DEL_SUBGRAPH:
      delSubGraph(graph, const_cast<Graph *>(gEvt->getSubGraph()));
      break;

    case GraphEventType::TLP_ADD_LOCAL_PROPERTY:
      addLocalProperty(graph, gEvt->getPropertyName());
      break;

    case GraphEventType::TLP_BEFORE_DEL_LOCAL_PROPERTY:
      delLocalProperty(graph, gEvt->getPropertyName());
      break;

    case GraphEventType::TLP_BEFORE_RENAME_LOCAL_PROPERTY:
      propertyRenamed(gEvt->getProperty());
      break;

    case GraphEventType::TLP_BEFORE_SET_ATTRIBUTE:
      beforeSetAttribute(graph, gEvt->getAttributeName());
      break;

    case GraphEventType::TLP_REMOVE_ATTRIBUTE:
      removeAttribute(graph, gEvt->getAttributeName());

    default:
      break;
    }
  } else {
    const auto *propEvt = dynamic_cast<const PropertyEvent *>(&ev);

    if (propEvt) {
      PropertyInterface *prop = propEvt->getProperty();

      switch (propEvt->getType()) {
      case PropertyEventType::TLP_BEFORE_SET_NODE_VALUE:
        beforeSetNodeValue(prop, propEvt->getNode());
        break;

      case PropertyEventType::TLP_BEFORE_SET_ALL_NODE_VALUE:
        beforeSetAllNodeValue(prop);
        break;

      case PropertyEventType::TLP_BEFORE_SET_ALL_EDGE_VALUE:
        beforeSetAllEdgeValue(prop);
        break;

      case PropertyEventType::TLP_BEFORE_SET_EDGE_VALUE:
        beforeSetEdgeValue(prop, propEvt->getEdge());
        break;

      default:
        break;
      }
    }
  }
}

// delete the objects collected as to be deleted
void GraphUpdatesRecorder::deleteDeletedObjects() {

  auto &propertiesToDelete = updatesReverted ? addedProperties : deletedProperties;

  auto &subGraphsToDelete = updatesReverted ? addedSubGraphs : deletedSubGraphs;

  // loop on properties
  for (const auto &[graph, properties] : propertiesToDelete) {
    for (auto *property : properties) {
      delete property;
    }
  }

  // loop on sub graphs
  for (const auto &[g, sg] : subGraphsToDelete) {
    sg->clearSubGraphs();
    delete sg;
  }
}

// clean up all the MutableContainers
void GraphUpdatesRecorder::deleteValues(
    std::unordered_map<PropertyInterface *, RecordedValues> &values) {
  for (const auto &[property, rvalues] : values) {
    delete rvalues.values;
    delete rvalues.recordedNodes;
    delete rvalues.recordedEdges;
  }

  values.clear();
}

// delete all the DataMem referenced by a std::unordered_map
void GraphUpdatesRecorder::deleteDefaultValues(
    std::unordered_map<PropertyInterface *, DataMem *> &values) {
  for (const auto &[property, value] : values) {
    delete value;
  }

  values.clear();
}

void GraphUpdatesRecorder::recordIncidence(std::unordered_map<node, std::vector<edge>> &incidences,
                                           GraphImpl *g, node n, edge e) {
  if (!incidences.contains(n)) {
    auto &incidence = incidences[n] = g->storage.incidence(n);
    // if we got a valid edge, this means that we must record
    // the node incidence before that edge was added (see addEdge)
    if (e.isValid()) {
      // as the edge is the last added
      // it must be at the last incidence index
      auto size = incidence.size() - 1;
      if (g->opposite(e, n) == n) {
        // handle loop special case
        size -= 1;
      }
      assert(e == incidence[size]);
      incidence.resize(size);
    }
  }
}

void GraphUpdatesRecorder::recordIncidence(std::unordered_map<node, std::vector<edge>> &incidences,
                                           GraphImpl *g, node n, const vector<edge> &edges,
                                           uint nbAdded) {
  if (!incidences.contains(n)) {
    auto &incidence = incidences[n] = g->storage.incidence(n);
    // we must ensure that the last edges added in gEdges
    // are previously removed from the current node incidence,
    // so we look (in reverse order because they must be at the end)
    // for the elts of incidence that are in the last edges added and remove them
    uint edgeAdded = 0;
    for (auto e : reversed(incidence)) {
      if (std::find(edges.end() - nbAdded, edges.end(), e) != edges.end()) {
        ++edgeAdded;
      } else {
        break;
      }
    }
    assert(edgeAdded);
    incidence.resize(incidence.size() - edgeAdded);
  }
}

void GraphUpdatesRecorder::removeFromIncidence(
    std::unordered_map<node, std::vector<edge>> &incidences, edge e, node n) {

  if (const auto itIncidence = incidences.find(n); itIncidence != incidences.end()) {
    auto &[n, incidence] = *itIncidence;

    if (const auto &it = std::find(incidence.begin(), incidence.end(), e); it != incidence.end()) {
      incidence.erase(it);
    }
  }
}

void GraphUpdatesRecorder::recordNewValues(GraphImpl *g) {
  assert(restartAllowed);

  if (!newValuesRecorded) {
    // from now on it will be done
    newValuesRecorded = true;

    // get ids memento
    auto *root = static_cast<GraphImpl *>(g);
    assert(newIdsState == nullptr);

    // record ids memento only if needed
    if ((graphAddedNodes.contains(g)) || (graphAddedEdges.contains(g))) {
      newIdsState = root->storage.getIdsMemento();
    }

    // record new incidences
    for (const auto &[e, ends] : addedEdgesEnds) {
      // e may have been deleted (see delEdge)
      if (root->isElement(e)) {
        auto [src, tgt] = ends;
        recordIncidence(newIncidences, root, src);
        recordIncidence(newIncidences, root, tgt);
      }
    }

    // record new properties default values & new values
    // loop on oldNodeDefaultValues
    for (const auto &[property, defaultValue] : oldNodeDefaultValues) {
      newNodeDefaultValues[property] = property->getNodeDefaultDataMemValue();
      recordNewNodeValues(property);
    }

    // loop on node oldValues
    for (const auto &[property, recordedValues] : oldValues) {
      if (recordedValues.recordedNodes && (!oldNodeDefaultValues.contains(property))) {
        recordNewNodeValues(property);
      }
    }

    // loop on updatedPropsAddedNodes
    for (const auto &[property, nodes] : updatedPropsAddedNodes) {

      PropertyInterface *nv;
      MutableContainer<bool> *rn;
      bool hasNewValues = false;

      const auto itnv = newValues.find(property);

      if (itnv == newValues.end()) {
        nv = property->clonePrototype(property->getGraph(), "");
        rn = new MutableContainer<bool>();
      } else {
        nv = itnv->second.values;
        rn = itnv->second.recordedNodes;

        if (!rn) {
          rn = itnv->second.recordedNodes = new MutableContainer<bool>();
        }
      }

      for (auto n : nodes) {
        if (nv->copy(n, n, property)) {
          rn->set(n, true);
          hasNewValues = true;
        }
      }

      if (itnv == newValues.end()) {
        if (hasNewValues) {
          newValues[property] = RecordedValues(nv, rn);
        } else {
          delete nv;
          delete rn;
        }
      }
    }

    // loop on oldEdgeDefaultValues
    for (const auto &[property, oldDefaultValue] : oldEdgeDefaultValues) {
      newEdgeDefaultValues[property] = property->getEdgeDefaultDataMemValue();
      recordNewEdgeValues(property);
    }

    // loop on edge oldValues
    for (const auto &[property, recordedValues] : oldValues) {
      if (recordedValues.recordedEdges && (!oldEdgeDefaultValues.contains(property))) {
        recordNewEdgeValues(property);
      }
    }

    // loop on updatedPropsAddedEdges
    for (const auto &[property, edges] : updatedPropsAddedEdges) {

      PropertyInterface *nv;
      MutableContainer<bool> *re;
      bool hasNewValues = false;

      const auto itnv = newValues.find(property);

      if (itnv == newValues.end()) {
        nv = property->clonePrototype(property->getGraph(), "");
        re = new MutableContainer<bool>();
      } else {
        nv = itnv->second.values;
        re = itnv->second.recordedEdges;

        if (!re) {
          re = itnv->second.recordedEdges = new MutableContainer<bool>();
        }
      }

      for (auto e : edges) {
        if (nv->copy(e, e, property)) {
          re->set(e, true);
          hasNewValues = true;
        }
      }

      if (itnv == newValues.end()) {
        if (hasNewValues) {
          newValues[property] = RecordedValues(nv, nullptr, re);
        } else {
          delete nv;
          delete re;
        }
      }
    }

    // record graph attribute new values
    for (const auto &[graph, attributes] : oldAttributeValues) {
      const DataSet &gAttValues = graph->getAttributes();
      DataSet &nAttValues = newAttributeValues[graph];

      for (const auto &[key, val] : attributes.getValues()) {
        DataType *data = gAttValues.getData(key);
        nAttValues.setData(key, data);
        delete data;
      }
    }
  }
}

void GraphUpdatesRecorder::recordNewNodeValues(PropertyInterface *p) {
  auto itnv = newValues.find(p);
  assert(itnv == newValues.end() || (itnv->second.recordedNodes == nullptr));

  PropertyInterface *nv;
  auto *rn = new MutableContainer<bool>();

  if (itnv == newValues.end()) {
    nv = p->clonePrototype(p->getGraph(), "");
  } else {
    nv = itnv->second.values;
  }

  bool hasNewValues = false;

  // record updated nodes new values
  if (oldNodeDefaultValues.contains(p)) {
    // loop on non default valuated nodes
    for (auto n : p->getNonDefaultValuatedNodes()) {
      nv->copy(n, n, p);
      rn->set(n, true);
      hasNewValues = true;
    }
  } else {

    if (const auto itp = oldValues.find(p); itp != oldValues.end() && itp->second.recordedNodes) {

      for (uint id : itp->second.recordedNodes->findAll(true)) {
        node n(id);

        if (nv->copy(n, n, p)) {
          rn->set(n, true);
          hasNewValues = true;
        }
      }
    }
  }

  if (hasNewValues) {
    if (itnv == newValues.end()) {
      newValues[p] = RecordedValues(nv, rn);
    } else {
      itnv->second.recordedNodes = rn;
    }
  } else {
    delete rn;

    if (itnv == newValues.end()) {
      delete nv;
    }
  }
}

void GraphUpdatesRecorder::recordNewEdgeValues(PropertyInterface *p) {

  PropertyInterface *nv;
  auto *re = new MutableContainer<bool>();

  const auto itnv = newValues.find(p);

  assert(itnv == newValues.end() || (itnv->second.recordedEdges == nullptr));

  if (itnv == newValues.end()) {
    nv = p->clonePrototype(p->getGraph(), "");
  } else {
    nv = itnv->second.values;
  }

  bool hasNewValues = false;

  // record updated edges new values
  if (oldEdgeDefaultValues.contains(p)) {
    // loop on non default valuated edges
    for (auto e : p->getNonDefaultValuatedEdges()) {
      nv->copy(e, e, p);
      re->set(e, true);
      hasNewValues = true;
    }
  } else {
    if (const auto itp = oldValues.find(p); itp != oldValues.end() && itp->second.recordedEdges) {
      for (uint id : itp->second.recordedEdges->findAll(true)) {
        edge e(id);
        if (nv->copy(e, e, p)) {
          re->set(e, true);
          hasNewValues = true;
        }
      }
    }
  }

  if (hasNewValues) {
    if (itnv == newValues.end()) {
      newValues[p] = RecordedValues(nv, nullptr, re);
    } else {
      itnv->second.recordedEdges = re;
    }
  } else {
    delete re;

    if (itnv == newValues.end()) {
      delete nv;
    }
  }
}

void GraphUpdatesRecorder::startRecording(GraphImpl *g) {
  if (g->getSuperGraph() == g) {
    if (oldIdsState == nullptr) {
      oldIdsState = static_cast<GraphImpl *>(g)->storage.getIdsMemento();
    }
  }

  restartRecording(g);
}

void GraphUpdatesRecorder::restartRecording(Graph *g) {
#if !defined(NDEBUG)

  if (g->getSuperGraph() == g) {
    assert(recordingStopped);
    recordingStopped = false;
  }

#endif

  if (newValuesRecorded) {
    deleteValues(newValues);
    deleteValues(newValues);
    deleteDefaultValues(newNodeDefaultValues);
    deleteDefaultValues(newEdgeDefaultValues);

    delete newIdsState;

    newIdsState = nullptr;
    newValuesRecorded = false;
  }

  g->addListener(this);

  // add self as a PropertyObserver for all previously
  // existing properties

  const set<PropertyInterface *> *newProps = nullptr;
  if (const auto itp = addedProperties.find(g); itp != addedProperties.end()) {
    newProps = &(itp->second);
  }

  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    if (newProps && (newProps->contains(prop))) {
      continue;
    }

    prop->addListener(this);
  }

  // add self as a GraphObserver for all previously
  // existing subgraphs
  const set<Graph *> *newSubGraphs = nullptr;
  set<Graph *> sgSet;

  for (const auto &[graph, sg] : addedSubGraphs) {
    if (graph == g) {
      sgSet.insert(sg);
    }
  }

  if (!sgSet.empty()) {
    newSubGraphs = &sgSet;
  }

  for (Graph *sg : g->subGraphs()) {
    if (!newSubGraphs || !newSubGraphs->contains(sg)) {
      restartRecording(sg);
    }
  }
}

void GraphUpdatesRecorder::stopRecording(Graph *g) {
#if !defined(NDEBUG)

  if (g->getSuperGraph() == g) {
    assert(!recordingStopped);
    recordingStopped = true;
  }

#endif
  g->removeListener(this);
  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    prop->removeListener(this);
  }
  for (Graph *sg : g->subGraphs()) {
    stopRecording(sg);
  }
}

void GraphUpdatesRecorder::doUpdates(GraphImpl *g, bool undo) {
  assert(updatesReverted != undo);
  updatesReverted = undo;

  Observable::holdObservers();
  // loop on propsToDel
  auto &propsToDel = undo ? addedProperties : deletedProperties;

  for (const auto &[graph, properties] : propsToDel) {
    for (auto *property : properties) {
      graph->delLocalProperty(property->getName());
    }
  }

  // loop on subGraphsToDel
  auto &subGraphsToDel = undo ? addedSubGraphs : deletedSubGraphs;

  for (const auto &[g, sg] : subGraphsToDel) {
    // remove from list of subgraphs + notify observers
    g->notifyBeforeDelSubGraph(sg);
    g->setSubGraphToKeep(sg);
    g->removeSubGraph(sg);

    if (!undo) {
      // restore its subgraphs as subgraph of its supergraph
      // only if we are redoing its deletion
      for (Graph *ssg : sg->subGraphs()) {
        g->restoreSubGraph(ssg);
      }
    }

    g->notifyAfterDelSubGraph(sg);
    sg->notifyDestroy();
  }

  // loop on edgesToDel
  auto &edgesToDel = undo ? graphAddedEdges : graphDeletedEdges;

  // edges must be removed in the decreasing order of the graphs ids
  // because for a coherent observation of deleted edges
  // they must be first deleted from a graph before being deleted
  // from its supergraph
  for (const auto &[graph, edges] : reversed(edgesToDel)) {
    // loop on graph's recorded edges
    for (edge e : edges) {
      if (graph->isElement(e)) {
        graph->removeEdge(e);
      }
    }
  }

  // loop on nodesToDel
  auto &nodesToDel = undo ? graphAddedNodes : graphDeletedNodes;

  for (const auto &[graph, nodes] : nodesToDel) {
    // loop on graph's recorded nodes
    for (node n : nodes) {
      graph->removeNode(n);
    }
  }

  // loop on subGraphsToAdd
  std::list<std::pair<Graph *, Graph *>> &subGraphsToAdd = undo ? deletedSubGraphs : addedSubGraphs;

  for (const auto &[g, sg] : subGraphsToAdd) {
    // notify its addition
    g->notifyBeforeAddSubGraph(sg);
    // restore sg as subgraph of g
    if (undo) {
      g->setSubGraphToKeep(sg);
    }
    g->restoreSubGraph(sg);

    // and sg subgraphs are no longer subgraphs of g
    for (Graph *ssg : sg->subGraphs()) {
      g->removeSubGraph(ssg);
      ssg->setSuperGraph(sg);
    }

    // notify its addition
    g->notifyAfterAddSubGraph(sg);
  }

  // loop on nodesToAdd
  auto &nodesToAdd = undo ? graphDeletedNodes : graphAddedNodes;

  for (const auto &[graph, nodes] : nodesToAdd) {
    // loop on graph's recorded nodes
    for (node n : nodes) {
      graph->restoreNode(n);
    }
  }

  // now restore ids manager state
  // this is done before the loop on the edges to add
  // because of some assertion in debug mode
  // while calling the restoreEdge method
  const GraphStorageIdsMemento *idsState = undo ? oldIdsState : newIdsState;

  if (idsState) {
    g->storage.restoreIdsMemento(idsState);
  }

  // loop on revertedEdges
  for (auto e : revertedEdges) {
    g->reverse(e);
  }

  // loop on edgesEnds
  auto &updatedEdgesEnds = undo ? oldEdgesEnds : newEdgesEnds;

  for (const auto &[e, ends] : updatedEdgesEnds) {
    const auto &[src, tgt] = ends;
    g->setEnds(e, src, tgt);
  }

  // loop on incidences
  auto &incidences = undo ? oldIncidences : newIncidences;
  for (const auto &[n, edges] : incidences) {
    // n may have been deleted as a previously added node
    // restore its incidence
    if (g->isElement(n)) {
      g->storage.setEdgeOrder(n, edges);
    }
  }

  // loop on edgesToAdd
  auto &edgesToAdd = undo ? graphDeletedEdges : graphAddedEdges;
  auto &edgesEnds = undo ? deletedEdgesEnds : addedEdgesEnds;

  for (const auto &[g, edges] : edgesToAdd) {
    // loop on graph's recorded edges
    for (edge e : edges) {
      if (const auto itEnds = edgesEnds.find(e); itEnds != edgesEnds.end()) {
        auto [src, tgt] = itEnds->second;
        g->restoreEdge(e, src, tgt);
      } else {
        // restoration of an edge in a subgraph that was already an element of the root graph
        // (i.e., not a newly added edge)
        g->restoreEdge(e, g->getRoot()->source(e), g->getRoot()->target(e));
      }
    }
  }

  // loop on propsToAdd
  auto &propsToAdd = undo ? deletedProperties : addedProperties;

  for (const auto &[g, properties] : propsToAdd) {
    for (auto *property : properties) {
      g->addLocalProperty(property->getName(), property);
    }
  }

  // loop on renamedProperties
  if (!renamedProperties.empty()) {
    for (auto &[prop, name] : renamedProperties) {
      std::string newName = prop->getName();
      // switch names
      prop->rename(name);
      name = newName;
    }
  }

  // loop on nodeDefaultValues
  auto &nodeDefaultValues = undo ? oldNodeDefaultValues : newNodeDefaultValues;

  for (const auto &[property, defaultValue] : nodeDefaultValues) {
    property->setAllNodeDataMemValue(defaultValue);
  }

  // loop on edgeDefaultValues
  auto &edgeDefaultValues = undo ? oldEdgeDefaultValues : newEdgeDefaultValues;

  for (const auto &[property, defaultValue] : edgeDefaultValues) {
    property->setAllEdgeDataMemValue(defaultValue);
  }

  // loop on recorded values
  auto &rvalues = undo ? oldValues : newValues;

  for (const auto &[property, recordedValues] : rvalues) {
    PropertyInterface *nv = recordedValues.values;

    if (recordedValues.recordedNodes) {

      for (uint id : recordedValues.recordedNodes->findAllValues(false, false)) {
        node n(id);
        property->copy(n, n, nv);
      }
    }

    if (recordedValues.recordedEdges) {

      for (uint id : recordedValues.recordedEdges->findAllValues(false, false)) {
        edge e(id);
        property->copy(e, e, nv);
      }
    }
  }

  // loop on attribute values to restore
  auto &attValues = undo ? oldAttributeValues : newAttributeValues;

  for (const auto &[g, attributes] : attValues) {
    for (const auto &[key, val] : attributes.getValues()) {
      if (val) {
        g->getNonConstAttributes().setData(key, val);
      } else {
        g->getNonConstAttributes().remove(key);
      }
    }
  }

  Observable::unholdObservers();
}

bool GraphUpdatesRecorder::hasUpdates() {
  assert(updatesReverted == false);

  // check addedProperties
  if (!addedProperties.empty()) {
    return true;
  }

  // check addedSubGraphs
  if (!addedSubGraphs.empty()) {
    return true;
  }

  // check graphAddedEdges
  if (!graphAddedEdges.empty()) {
    return true;
  }

  // check graphAddedNodes
  for (const auto &[g, nodes] : graphAddedNodes) {
    if (!nodes.empty()) {
      return true;
    }
  }

  // check deletedSubGraphs
  if (!deletedSubGraphs.empty()) {
    return true;
  }

  // check graphDeletedNodes
  if (!graphDeletedNodes.empty()) {
    return true;
  }

  // check revertedEdges
  if (!revertedEdges.empty()) {
    return true;
  }

  // check oldEdgesEnds
  if (!oldEdgesEnds.empty()) {
    return true;
  }

  // check oldIncidences
  if (!oldIncidences.empty()) {
    return true;
  }

  // check graphDeletedEdges
  if (!graphDeletedEdges.empty()) {
    return true;
  }

  // check deletedProperties
  if (!deletedProperties.empty()) {
    return true;
  }

  // check renamedProperties
  if (!renamedProperties.empty()) {
    return true;
  }

  // check oldNodeDefaultValues
  if (!oldNodeDefaultValues.empty()) {
    return true;
  }

  // check oldEdgeDefaultValues
  if (!oldEdgeDefaultValues.empty()) {
    return true;
  }

  // check oldValues
  if (!oldValues.empty()) {
    return true;
  }

  // check oldAttributeValues
  if (!oldAttributeValues.empty()) {
    return true;
  }

  return false;
}

bool GraphUpdatesRecorder::dontObserveProperty(PropertyInterface *prop) {
  if (!restartAllowed) {
    // check if nothing is yet recorded for prop
    if ((!oldNodeDefaultValues.contains(prop)) && (!oldEdgeDefaultValues.contains(prop)) &&
        (!oldValues.contains(prop)) && (!updatedPropsAddedNodes.contains(prop)) &&
        (!updatedPropsAddedEdges.contains(prop))) {
      // prop is no longer observed
      prop->removeListener(this);
      // may be a newly added property
      Graph *g = prop->getGraph();

      if (const auto it = addedProperties.find(g);
          it != addedProperties.end() && (it->second.find(prop) != it->second.end())) {
        // the property is no longer recorded
        it->second.erase(prop);
      }

      return true;
    }
  }

  return false;
}

bool GraphUpdatesRecorder::isAddedOrDeletedProperty(Graph *g, PropertyInterface *prop) {

  if (const auto it = addedProperties.find(g);
      it != addedProperties.end() && (it->second.find(prop) != it->second.end())) {
    return true;
  }

  if (const auto it = deletedProperties.find(g);
      it != deletedProperties.end() && (it->second.find(prop) != it->second.end())) {
    return true;
  }

  return false;
}

void GraphUpdatesRecorder::addNode(Graph *g, node n) {

  if (const auto itgn = graphAddedNodes.find(g); itgn == graphAddedNodes.end()) {
    graphAddedNodes[g] = {n};
  } else {
    itgn->second.insert(n);
  }

  if (g->getRoot() == g) {
    addedNodes.insert(n);
  }

  // we need to backup properties values of the newly added node
  // in order to restore them when reading the node through the tlp::Graph::unpop() method
  // as the default properties values might change
  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    beforeSetNodeValue(prop, n);
  }
}

void GraphUpdatesRecorder::addEdge(Graph *g, edge e) {

  if (const auto itge = graphAddedEdges.find(g); itge == graphAddedEdges.end()) {
    graphAddedEdges[g] = {e};
  } else {
    itge->second.insert(e);
  }

  if (g == g->getRoot()) {
    const auto &[src, tgt] = g->ends(e);
    addedEdgesEnds[e] = {src, tgt};
    // record source and target incidences
    recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), src, e);
    recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), tgt, e);
  }

  // we need to backup properties values of the newly added edge
  // in order to restore them when reading the node through the tlp::Graph::unpop() method
  // as the default properties values can change
  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    beforeSetEdgeValue(prop, e);
  }
}

void GraphUpdatesRecorder::addEdges(Graph *g, uint nbAdded) {
  if (!graphAddedEdges.contains(g)) {
    graphAddedEdges[g] = {};
  }

  unordered_set<edge> &ge = graphAddedEdges[g];
  auto gEdges = g->edges();

  for (uint i = gEdges.size() - nbAdded; i < gEdges.size(); ++i) {
    edge e = gEdges[i];
    ge.insert(e);

    if (g == g->getRoot()) {
      const auto &[src, tgt] = g->ends(e);
      addedEdgesEnds[e] = {src, tgt};
      // record source and target old incidences
      recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), src, gEdges, nbAdded);
      recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), tgt, gEdges, nbAdded);
    }

    // we need to backup properties values of the newly added edge
    // in order to restore them when reading the node through the tlp::Graph::unpop() method
    // as the default properties values can change
    for (PropertyInterface *prop : g->getLocalObjectProperties()) {
      beforeSetEdgeValue(prop, e);
    }
  }
}

void GraphUpdatesRecorder::delNode(Graph *g, node n) {

  if (const auto itgn = graphAddedNodes.find(g); itgn != graphAddedNodes.end()) {

    if (const auto itn = itgn->second.find(n); itn != itgn->second.end()) {
      // remove n from graph's recorded nodes if it is a newly added node
      itgn->second.erase(itn);
      // but don't remove it from addedNodes
      // to ensure further erasal from property will not
      // record a value as if it was a preexisting node
      return;
    }
  }

  // insert n into graphDeletedNodes
  if (const auto itgn = graphDeletedNodes.find(g); itgn == graphDeletedNodes.end()) {
    graphDeletedNodes[g] = {n};
  } else {
    itgn->second.insert(n);
  }

  // get the set of added properties if any

  const set<PropertyInterface *> *newProps = nullptr;
  if (const auto itp = addedProperties.find(g); itp != addedProperties.end()) {
    newProps = &(itp->second);
  }

  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    // nothing to record for newly added properties
    if (newProps && (newProps->contains(prop))) {
      continue;
    }

    beforeSetNodeValue(prop, n);
  }

  if (g == g->getSuperGraph()) {
    recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), n);
  }
}

void GraphUpdatesRecorder::delEdge(Graph *g, edge e) {

  if (const auto itge = graphAddedEdges.find(g); itge != graphAddedEdges.end()) {

    if (const auto ite = itge->second.find(e); ite != itge->second.end()) {
      // remove e as it is a newly added edge
      itge->second.erase(ite);
      // do not remove from addedEdgesEnds
      // to ensure further erasal from property will not
      // record a value as if it was a preexisting edge
      /* if (graphs.empty())
         addedEdges.erase(it); */
      // remove from revertedEdges if needed

      if (const auto itR = revertedEdges.find(e); itR != revertedEdges.end()) {
        revertedEdges.erase(itR);
      }

      // remove edge from nodes newIncidences if needed
      if (const auto itEnds = addedEdgesEnds.find(e); itEnds != addedEdgesEnds.end()) {
        const auto &[src, tgt] = itEnds->second;
        removeFromIncidence(newIncidences, e, src);
        removeFromIncidence(newIncidences, e, tgt);
      }

      return;
    }
  }

  // insert e into graph's deleted edges
  if (const auto itge = graphDeletedEdges.find(g); itge == graphDeletedEdges.end()) {
    graphDeletedEdges[g] = {e};
  } else {
    itge->second.insert(e);
  }

  const auto &[src, tgt] = g->ends(e);
  if (!deletedEdgesEnds.contains(e)) {
    if (g == g->getRoot()) {
      // remove from revertedEdges if needed
      if (const auto it = revertedEdges.find(e); it != revertedEdges.end()) {
        revertedEdges.erase(it);
        deletedEdgesEnds[e] = {tgt, src};
      } else {

        if (const auto ite = oldEdgesEnds.find(e); ite == oldEdgesEnds.end()) {
          deletedEdgesEnds[e] = {src, tgt};
        } else {
          const auto &[src2, tgt2] = ite->second;
          deletedEdgesEnds[e] = {src2, tgt2};
          // remove from oldEdgesEnds
          oldEdgesEnds.erase(ite);
          // remove from newEdgesEnds
          newEdgesEnds.erase(e);
        }
      }
    } else {
      deletedEdgesEnds[e] = {src, tgt};
    }
  }

  // get the set of added properties if any
  const set<PropertyInterface *> *newProps = nullptr;
  if (const auto itp = addedProperties.find(g); itp != addedProperties.end()) {
    newProps = &(itp->second);
  }

  // loop on properties to save the edge's associated values
  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    // nothing to record for newly added properties
    if (newProps && (newProps->contains(prop))) {
      continue;
    }

    beforeSetEdgeValue(prop, e);
  }

  if (g == g->getRoot()) {
    // record source and target old incidences
    recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), src);
    recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), tgt);
  }
}

void GraphUpdatesRecorder::reverseEdge(Graph *g, edge e) {
  if (g == g->getSuperGraph()) {

    // if it is a newly added edge revert its source and target
    if (const auto itEnds = addedEdgesEnds.find(e); itEnds != addedEdgesEnds.end()) {
      auto &[src, tgt] = itEnds->second;
      std::swap(src, tgt);
      return;
    }

    if (const auto itne = newEdgesEnds.find(e); itne != newEdgesEnds.end()) {
      // revert ends of itne
      auto &[src, tgt] = itne->second;
      std::swap(src, tgt);
    } else { // update reverted edges

      if (const auto it = revertedEdges.find(e); it != revertedEdges.end()) {
        revertedEdges.erase(it);
      } else {
        revertedEdges.insert(e);
        // record source and target old incidences
        const auto &[src, tgt] = g->ends(e);
        recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), src);
        recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), tgt);
      }
    }
  }
}

void GraphUpdatesRecorder::beforeSetEnds(Graph *g, edge e) {
  if ((g == g->getRoot()) && (!oldEdgesEnds.contains(e)) && (!addedEdgesEnds.contains(e))) {
    auto [src, tgt] = g->ends(e);

    // if it is a reverted edge
    // remove it from the set
    if (const auto it = revertedEdges.find(e); it != revertedEdges.end()) {
      revertedEdges.erase(it);
      // revert ends of it
      std::swap(src, tgt);
    } else {
      // record source & target old incidences
      recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), src);
      recordIncidence(oldIncidences, static_cast<GraphImpl *>(g), tgt);
    }

    // add e old ends in oldEdgesEnds
    oldEdgesEnds[e] = {src, tgt};
  }
}

void GraphUpdatesRecorder::afterSetEnds(Graph *g, edge e) {
  if (g == g->getSuperGraph()) {
    auto ends = g->ends(e);

    // if it is a newly added edge update its source and target
    if (const auto itEnds = addedEdgesEnds.find(e); itEnds != addedEdgesEnds.end()) {
      itEnds->second = ends;
      return;
    }

    // update new ends in newEdgesEnds
    newEdgesEnds[e] = ends;
  }
}

void GraphUpdatesRecorder::addSubGraph(Graph *g, Graph *sg) {
  // last added subgraph will be deleted first during undo/redo
  addedSubGraphs.push_front({g, sg});

  // sg may already have nodes and edges
  // cf addCloneSubGraph
  if (sg->numberOfNodes()) {

    for (auto n : sg->nodes()) {
      addNode(sg, n);
    }

    for (auto e : sg->edges()) {
      addEdge(sg, e);
    }
  }

  sg->addListener(this);
}

void GraphUpdatesRecorder::delSubGraph(Graph *g, Graph *sg) {

  pair p = {g, sg};

  // remove sg from addedSubGraphs if it is a newly added subgraph
  if (const auto it = std::find(addedSubGraphs.begin(), addedSubGraphs.end(), p);
      it != addedSubGraphs.end()) {

    addedSubGraphs.erase(it);

    // remove any update data concerning the removed subgraph
    // as it will be deleted
    removeGraphData(sg);

    // but set its subgraphs as added in its supergraph
    for (Graph *ssg : sg->subGraphs()) {
      addSubGraph(g, ssg);
    }

    return;
  }

  // last deleted subgraph will be the last one created during undo/redo
  deletedSubGraphs.push_back(p);

  // sg is no longer observed
  sg->removeListener(this);

  // but it must not be really deleted
  g->setSubGraphToKeep(sg);
}

void GraphUpdatesRecorder::removeGraphData(Graph *g) {
  for (Graph *sg : g->subGraphs()) {
    if (const auto it = std::find(addedSubGraphs.begin(), addedSubGraphs.end(), pair(g, sg));
        it != addedSubGraphs.end()) {
      addedSubGraphs.erase(it);
    }
  }
  graphAddedNodes.erase(g);
  graphDeletedNodes.erase(g);
  graphAddedEdges.erase(g);
  graphDeletedEdges.erase(g);
  addedProperties.erase(g);
  deletedProperties.erase(g);
  oldAttributeValues.erase(g);
  newAttributeValues.erase(g);
}

void GraphUpdatesRecorder::addLocalProperty(Graph *g, const string &name) {
  PropertyInterface *prop = g->getProperty(name);

  if (const auto it = addedProperties.find(g); it == addedProperties.end()) {
    addedProperties[g] = {prop};
  } else {
    it->second.insert(prop);
  }
}

void GraphUpdatesRecorder::delLocalProperty(Graph *g, const string &name) {
  PropertyInterface *prop = g->getProperty(name);

  // remove p from addedProperties if it is a newly added one
  if (const auto it = addedProperties.find(g);
      it != addedProperties.end() && (it->second.find(prop) != it->second.end())) {
    // the property is no longer recorded
    it->second.erase(prop);
    // remove from renamed properties
    // if needed

    if (const auto itr = renamedProperties.find(prop); itr != renamedProperties.end()) {
      renamedProperties.erase(itr);
    }

    updatedPropsAddedNodes.erase(prop);
    updatedPropsAddedEdges.erase(prop);

    return;
  }

  // insert p into deletedProperties
  if (const auto it = deletedProperties.find(g); it == deletedProperties.end()) {
    deletedProperties[g] = {prop};
  } else {
    it->second.insert(prop);
  }

  // the property is no longer observed
  prop->removeListener(this);
}

void GraphUpdatesRecorder::propertyRenamed(PropertyInterface *prop) {

  // remove p from addedProperties if it is a newly added one
  if (const auto it = addedProperties.find(prop->getGraph());
      it != addedProperties.end() && (it->second.find(prop) != it->second.end())) {
    return;
  } else {
    if (!renamedProperties.contains(prop)) {
      renamedProperties[prop] = prop->getName();
    }
  }
}

void GraphUpdatesRecorder::beforeSetNodeValue(PropertyInterface *p, node n) {
  // don't record the old value if the default one has been changed
  if (oldNodeDefaultValues.contains(p)) {
    return;
  }

  // don't record old values for newly added nodes
  if (addedNodes.contains(n)) {
    if (!restartAllowed) {
      return;
    } else {
      if (p->getGraph()->isElement(n)) {
        updatedPropsAddedNodes[p].insert(n);
      } else {
        // n has been deleted in the whole graph hierarchy, so we don't
        // need to backup its property value in the next push as the node
        // does not belong to a graph anymore
        updatedPropsAddedNodes[p].erase(n);
      }
    }
  } else {
    if (const auto it = oldValues.find(p); it == oldValues.end()) {
      PropertyInterface *pv = p->clonePrototype(p->getGraph(), "");
      auto *rn = new MutableContainer<bool>();

      pv->copy(n, n, p);
      rn->set(n, true);
      oldValues[p] = RecordedValues(pv, rn);
    } else {
      // check for a previously recorded old value
      if (it->second.recordedNodes) {
        if (it->second.recordedNodes->get(n)) {
          return;
        }
      } else {
        it->second.recordedNodes = new MutableContainer<bool>();
      }

      it->second.values->copy(n, n, p);
      it->second.recordedNodes->set(n, true);
    }
  }
}

void GraphUpdatesRecorder::beforeSetAllNodeValue(PropertyInterface *p) {
  if (!oldNodeDefaultValues.contains(p)) {
    // first save the already existing value for all non default valuated nodes
    for (auto n : p->getNonDefaultValuatedNodes()) {
      beforeSetNodeValue(p, n);
    }
    // then record the old default value
    // because beforeSetNodeValue does nothing if it has already been changed
    oldNodeDefaultValues[p] = p->getNodeDefaultDataMemValue();
  }
}

void GraphUpdatesRecorder::beforeSetEdgeValue(PropertyInterface *p, edge e) {
  // don't record the old value if the default one has been changed
  if (oldEdgeDefaultValues.contains(p)) {
    return;
  }

  // don't record old value for newly added edge
  if (addedEdgesEnds.contains(e)) {
    if (!restartAllowed) {
      return;
    }

    if (p->getGraph()->isElement(e)) {
      updatedPropsAddedEdges[p].insert(e);
    } else {
      // e has been deleted in the whole graph hierarchy, so we don't
      // need to backup its property value in the next push as the edge
      // does not belong to a graph anymore
      updatedPropsAddedEdges[p].erase(e);
    }
  } else {
    if (const auto it = oldValues.find(p); it == oldValues.end()) {
      PropertyInterface *pv = p->clonePrototype(p->getGraph(), "");
      auto *re = new MutableContainer<bool>();

      pv->copy(e, e, p);
      re->set(e, true);
      oldValues[p] = RecordedValues(pv, nullptr, re);
    } else {
      // check for a previously recorded old value
      if (it->second.recordedEdges) {
        if (it->second.recordedEdges->get(e)) {
          return;
        }
      } else {
        it->second.recordedEdges = new MutableContainer<bool>();
      }

      it->second.values->copy(e, e, p);
      it->second.recordedEdges->set(e, true);
    }
  }
}

void GraphUpdatesRecorder::beforeSetAllEdgeValue(PropertyInterface *p) {
  if (!oldEdgeDefaultValues.contains(p)) {
    // first save the already existing value for all non default valuated edges
    for (auto e : p->getNonDefaultValuatedEdges()) {
      beforeSetEdgeValue(p, e);
    }
    // then record the old default value
    // because beforeSetEdgeValue does nothing if it has already been changed
    oldEdgeDefaultValues[p] = p->getEdgeDefaultDataMemValue();
  }
}

void GraphUpdatesRecorder::beforeSetAttribute(Graph *g, const std::string &name) {
  if (const auto it = oldAttributeValues.find(g);
      it != oldAttributeValues.end() && it->second.exists(name)) {
    return;
  }

  // save the previously existing value
  DataType *valType = g->getAttributes().getData(name);
  oldAttributeValues[g].setData(name, valType);
  delete valType;
}

void GraphUpdatesRecorder::removeAttribute(Graph *g, const std::string &name) {
  beforeSetAttribute(g, name);
}
