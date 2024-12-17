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

#include <stack>

#include <talipot/Graph.h>
#include <talipot/PropertyManager.h>
#include <talipot/GraphView.h>
#include <talipot/GraphUpdatesRecorder.h>
#include <talipot/UniqueIterator.h>

using namespace std;
using namespace tlp;

#ifndef NDEBUG
/*
 * function to test if an edge e exist in the adjacency of a node
 */
static bool existEdgeE(Graph *g, const node n1, const node, edge e) {
  for (auto e1 : g->getOutEdges(n1)) {
    if (e == e1) {
      return true;
    }
  }
  return false;
}
#endif
//----------------------------------------------------------------
GraphImpl::GraphImpl() : GraphAbstract(this) {
  // id 0 is for the root
  graphIds.get();
}
//----------------------------------------------------------------
GraphImpl::~GraphImpl() {
  unobserveUpdates();

  // delete recorders
  if (!recorders.empty()) {
    recorders.front()->stopRecording(this);

    for (auto *r : recorders) {
      delete r;
    }

    recorders.clear();
  }

  delPreviousRecorders();

  // notify destruction
  observableDeleted();
}
//----------------------------------------------------------------
void GraphImpl::clear() {
  GraphAbstract::clear();
  storage.clear();
}
//----------------------------------------------------------------
edge GraphImpl::existEdge(const node src, const node tgt, bool directed) const {
  std::vector<edge> edges = storage.getEdges(src, tgt, directed, nullptr);
  return !edges.empty() ? edges[0] : edge();
}
//----------------------------------------------------------------
uint GraphImpl::getSubGraphId(uint id) {
  if (id == 0) {
    return graphIds.get();
  }

  graphIds.updateFreeIds(id);
  return id;
}
//----------------------------------------------------------------
void GraphImpl::freeSubGraphId(uint id) {
  graphIds.free(id);
}
//----------------------------------------------------------------
void GraphImpl::restoreNode(node newNode) {
  storage.restoreNode(newNode);
  notifyAddNode(newNode);
}
//----------------------------------------------------------------
node GraphImpl::addNode() {
  node newNode = storage.addNode();
  notifyAddNode(newNode);
  return newNode;
}
//----------------------------------------------------------------
std::vector<node> GraphImpl::addNodes(uint nb) {
  std::vector<node> addedNodes;
  if (nb) {
    addedNodes = storage.addNodes(nb);
    if (hasOnlookers()) {
      sendEvent(GraphEvent(*this, GraphEventType::TLP_ADD_NODES, nb));
    }
  }
  return addedNodes;
}
//----------------------------------------------------------------
void GraphImpl::addNode(const node) {
  tlp::warning() << "Warning : " << __PRETTY_FUNCTION__ << " ... Impossible operation on Root Graph"
                 << std::endl;
}
//----------------------------------------------------------------
void GraphImpl::addNodes(const vector<node> &) {
  tlp::warning() << "Warning : " << __PRETTY_FUNCTION__ << " ... Impossible operation on Root Graph"
                 << std::endl;
}
//----------------------------------------------------------------
void GraphImpl::reserveNodes(uint nb) {
  storage.reserveNodes(nb);
}
//----------------------------------------------------------------
void GraphImpl::restoreEdge(edge newEdge, const node src, const node tgt) {
  storage.restoreEdge(src, tgt, newEdge);
  notifyAddEdge(newEdge);
}
//----------------------------------------------------------------
edge GraphImpl::addEdge(const node src, const node tgt) {
  assert(src.isValid() && tgt.isValid());
  edge newEdge = storage.addEdge(src, tgt);
  notifyAddEdge(newEdge);
  return newEdge;
}
//----------------------------------------------------------------
std::vector<edge> GraphImpl::addEdges(const std::vector<std::pair<node, node>> &edges) {
  std::vector<edge> addedEdges;
  if (!edges.empty()) {
    addedEdges = storage.addEdges(edges);
    if (hasOnlookers()) {
      sendEvent(GraphEvent(*this, GraphEventType::TLP_ADD_EDGES, edges.size()));
    }
  }
  return addedEdges;
}
//----------------------------------------------------------------
void GraphImpl::addEdge(const edge e) {
  tlp::warning() << "Warning: " << __PRETTY_FUNCTION__ << " ... Impossible operation on Root Graph"
                 << std::endl;
  tlp::warning() << "\t Trying to add edge " << e.id << " (" << source(e).id << "," << target(e).id
                 << ")";
}
//----------------------------------------------------------------
void GraphImpl::addEdges(const vector<edge> &) {
  tlp::warning() << "Warning: " << __PRETTY_FUNCTION__ << " ... Impossible operation on Root Graph"
                 << std::endl;
}
//----------------------------------------------------------------
void GraphImpl::reserveEdges(uint nb) {
  storage.reserveEdges(nb);
}
//----------------------------------------------------------------
void GraphImpl::removeNode(const node n) {
  assert(isElement(n));
  notifyDelNode(n);
  // remove from storage and propertyContainer
  storage.removeFromNodes(n);
  propertyContainer->erase(n);
}
//----------------------------------------------------------------
void GraphImpl::delNode(const node n, bool) {
  assert(isElement(n));
  std::vector<edge> edges = storage.incidence(n);

  // use a stack for a dfs subgraphs propagation
  std::stack<Graph *> sgq;

  for (Graph *sg : subGraphs()) {
    if (sg->isElement(n)) {
      sgq.push(sg);
    }
  }

  // subgraphs loop
  while (!sgq.empty()) {
    Graph *sg = sgq.top();

    for (Graph *ssg : sg->subGraphs()) {
      if (ssg->isElement(n)) {
        sgq.push(ssg);
      }
    }

    if (sg == sgq.top()) {
      static_cast<GraphView *>(sg)->removeNode(n, edges);
      sgq.pop();
    }
  }

  // loop on inout edges of n
  // for notification and removal from propertyContainer
  for (auto e : edges) {
    // if e is a loop it may have been previously deleted
    if (isElement(e)) {
      removeEdge(e);
    }
  }

  notifyDelNode(n);
  // delete n from storage
  storage.delNode(n);

  // remove from propertyContainer
  propertyContainer->erase(n);
}
//----------------------------------------------------------------
void GraphImpl::delEdge(const edge e, bool) {
  assert(existEdgeE(this, source(e), target(e), e));

  if (!isElement(e)) {
    return;
  }

  // propagate to subgraphs
  for (Graph *subgraph : subGraphs()) {
    assert(subgraph != this);

    if (subgraph->isElement(e)) {
      subgraph->delEdge(e);
    }
  }

  removeEdge(e);
}
//----------------------------------------------------------------
Iterator<node> *GraphImpl::getNodes() const {
  return stlIterator(storage.nodes());
}
//----------------------------------------------------------------
Iterator<node> *GraphImpl::getInNodes(const node n) const {
  return uniqueIterator(conversionIterator<node>(
      filterIterator(storage.incidence(n), [this, n](const edge e) { return target(e) == n; }),
      [this](const edge e) { return source(e); }));
}
//----------------------------------------------------------------
Iterator<node> *GraphImpl::getOutNodes(const node n) const {
  return uniqueIterator(conversionIterator<node>(
      filterIterator(storage.incidence(n), [this, n](const edge e) { return source(e) == n; }),
      [this](const edge e) { return target(e); }));
}
//----------------------------------------------------------------
Iterator<node> *GraphImpl::getInOutNodes(const node n) const {
  return conversionIterator<node>(storage.incidence(n), [this, n](const edge e) {
    return source(e) == n ? target(e) : source(e);
  });
}
//----------------------------------------------------------------
Iterator<edge> *GraphImpl::getEdges() const {
  return stlIterator(storage.edges());
}
//----------------------------------------------------------------
Iterator<edge> *GraphImpl::getInEdges(const node n) const {
  return uniqueIterator(
      filterIterator(storage.incidence(n), [this, n](const edge e) { return target(e) == n; }));
}
//----------------------------------------------------------------
Iterator<edge> *GraphImpl::getOutEdges(const node n) const {
  return uniqueIterator(
      filterIterator(storage.incidence(n), [this, n](const edge e) { return source(e) == n; }));
}
//----------------------------------------------------------------
Iterator<edge> *GraphImpl::getInOutEdges(const node n) const {
  return stlIterator(storage.incidence(n));
}
//----------------------------------------------------------------
std::vector<edge> GraphImpl::getEdges(const node src, const node tgt, bool directed) const {
  return storage.getEdges(src, tgt, directed);
}
//----------------------------------------------------------------
void GraphImpl::reverse(const edge e) {
  assert(isElement(e));
  auto [src, tgt] = storage.ends(e);

  // notification
  notifyReverseEdge(e);

  storage.reverse(e);

  // propagate edge reversal on subgraphs
  for (Graph *sg : subGraphs()) {
    static_cast<GraphView *>(sg)->reverseInternal(e, src, tgt);
  }
}
//----------------------------------------------------------------
void GraphImpl::setEnds(const edge e, const node newSrc, const node newTgt) {
  assert(isElement(e));

  // not allowed on meta edge
  if (isMetaEdge(e)) {
    tlp::warning() << "Warning: invoking Graph::setEnds on meta edge " << e.id << std::endl;
    return;
  }

  // be aware that newSrc or newTgt may not be valid
  // to indicate that only one of the ends has to be changed
  auto [src, tgt] = storage.ends(e);

  // nothing to do if same ends
  if (src == newSrc && tgt == newTgt) {
    return;
  }

  // notification
  notifyBeforeSetEnds(e);

  storage.setEnds(e, newSrc, newTgt);

  // notification
  notifyAfterSetEnds(e);

  // propagate edge reversal on subgraphs
  const auto &[nSrc, nTgt] = storage.ends(e);

  for (Graph *sg : subGraphs()) {
    static_cast<GraphView *>(sg)->setEndsInternal(e, src, tgt, nSrc, nTgt);
  }
}
//----------------------------------------------------------------
void GraphImpl::removeEdge(const edge e) {
  assert(isElement(e));
  notifyDelEdge(e);
  // remove from propertyContainer and storage
  storage.delEdge(e);
  propertyContainer->erase(e);
}
//----------------------------------------------------------------
bool GraphImpl::canPop() {
  return (!recorders.empty());
}
//----------------------------------------------------------------
bool GraphImpl::canPopThenUnpop() {
  return (!recorders.empty() && recorders.front()->restartAllowed);
}
//----------------------------------------------------------------
bool GraphImpl::canUnpop() {
  return (!previousRecorders.empty());
}
//----------------------------------------------------------------
void GraphImpl::delPreviousRecorders() {
  // we delete previous recorders in reverse order
  // because they are pushed in front of previousRecorders
  // when they are popped from recorders,
  // so the lasts created are back in previousRecorders
  for (auto *pr : reversed(previousRecorders)) {
    delete pr;
  }

  previousRecorders.clear();
}
//----------------------------------------------------------------
void GraphImpl::treatEvents(const std::vector<Event> &) {
  // an update occurs in the graph hierarchy
  // so delete the previous recorders
  delPreviousRecorders();
  unobserveUpdates();
}
//----------------------------------------------------------------
void GraphImpl::observeUpdates(Graph *g) {
  g->addObserver(this);
  observedGraphs.push_front(g);

  // loop on local properties
  for (PropertyInterface *prop : g->getLocalObjectProperties()) {
    prop->addObserver(this);
    observedProps.push_front(prop);
  }

  // loop on subgraphs
  for (Graph *sg : g->subGraphs()) {
    observeUpdates(sg);
  }
}
//----------------------------------------------------------------
void GraphImpl::unobserveUpdates() {
  // loop on observed graphs
  while (!observedGraphs.empty()) {
    observedGraphs.front()->removeObserver(this);
    observedGraphs.pop_front();
  }

  // loop on observed properties
  while (!observedProps.empty()) {
    observedProps.front()->removeObserver(this);
    observedProps.pop_front();
  }
}
//----------------------------------------------------------------
#define NB_MAX_RECORDERS 10
void GraphImpl::push(bool unpopAllowed, std::vector<PropertyInterface *> *propsToPreserve) {
  // from now if previous recorders exist
  // they cannot be unpop
  // so delete them
  delPreviousRecorders();

  bool hasRecorders = !recorders.empty();

  // if we have a current recorder with no updates
  // there is no need to push a new one, so go on with the same
  // (except if a temporary non redoable state is explicitly requested)
  if (unpopAllowed && hasRecorders && !recorders.front()->hasUpdates()) {
    return;
  }

  // end any previous updates observation
  unobserveUpdates();

  if (hasRecorders) {
    // stop recording for current recorder
    recorders.front()->stopRecording(this);
  }

  const GraphStorageIdsMemento *prevIdsMemento =
      hasRecorders ? recorders.front()->newIdsState : nullptr;

  auto *recorder = new GraphUpdatesRecorder(unpopAllowed, prevIdsMemento);
  recorder->startRecording(this);
  recorders.push_front(recorder);

  // if this is not a temporary state used for computation purpose
  // as in BubbleTree for example
  if (unpopAllowed) {
    // delete first pushed recorders (those at the end of the list) if needed
    uint nb = recorders.size();

    if (nb > NB_MAX_RECORDERS) {
      auto it = recorders.rbegin();

      while (nb > NB_MAX_RECORDERS) {
        delete (*it);
        --nb;
        ++it;
      }
      recorders.resize(nb);
    }
  }

  if (propsToPreserve) {
    // the properties to preserve do not have to be observed
    for (auto *prop : *propsToPreserve) {
      recorder->dontObserveProperty(prop);
    }
  }
}
//----------------------------------------------------------------
void GraphImpl::pop(bool unpopAllowed) {
  // save the front recorder
  // to allow unpop
  if (!recorders.empty()) {
    // if (!previousRecorders.empty())
    unobserveUpdates();
    GraphUpdatesRecorder *prevRecorder = recorders.front();

    if (unpopAllowed && prevRecorder->restartAllowed) {
      prevRecorder->recordNewValues(this);
    }

    prevRecorder->stopRecording(this);
    // undo all recorded updates
    prevRecorder->doUpdates(this, true);

    // push it
    if (unpopAllowed && prevRecorder->restartAllowed) {
      previousRecorders.push_front(prevRecorder);
      // observe any updates
      // in order to remove previous recorders if needed
      observeUpdates(this);
    } else {
      delete prevRecorder;
    }

    // must be done here (see canPop, canUnpop)
    recorders.pop_front();

    // restart the front recorder
    if (!recorders.empty()) {
      recorders.front()->restartRecording(this);
    }
  }
}
//----------------------------------------------------------------
void GraphImpl::popIfNoUpdates() {
  if (!recorders.empty() && !recorders.front()->hasUpdates()) {
    // no need of a "no updates" recorder
    this->pop(false);
  }
}
//----------------------------------------------------------------
void GraphImpl::unpop() {
  int nbPrev = previousRecorders.size();

  if (nbPrev != 0) {
    unobserveUpdates();

    if (!recorders.empty()) {
      recorders.front()->stopRecording(this);
    }

    GraphUpdatesRecorder *prevRecorder = previousRecorders.front();
    previousRecorders.pop_front();
    recorders.push_front(prevRecorder);
    // redo all recorded updates
    prevRecorder->doUpdates(this, false);
    prevRecorder->restartRecording(this);

    // if previous recorders can be unpop
    // ensure they will be removed
    // with the next update
    if (nbPrev > 1) {
      observeUpdates(this);
    }
  }
}
//----------------------------------------------------------------
bool GraphImpl::canDeleteProperty(Graph *g, PropertyInterface *prop) {
  return recorders.empty() || !recorders.front()->isAddedOrDeletedProperty(g, prop);
}
