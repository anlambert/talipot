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

#include <stack>

#include <talipot/BooleanProperty.h>
#include <talipot/FilterIterator.h>
#include <talipot/GraphView.h>
#include <talipot/PropertyManager.h>
#include <talipot/UniqueIterator.h>

using namespace std;
namespace tlp {
//----------------------------------------------------------------
GraphView::GraphView(Graph *superGraph, BooleanProperty *filter, uint sgId)
    : GraphAbstract(superGraph, sgId) {

  _nodeData.alloc(this);

  if (filter == nullptr) {
    return;
  }

  for (auto n :
       filterIterator(superGraph->nodes(), [filter](node n) { return filter->getNodeValue(n); })) {
    addNode(n);
  }

  for (auto e :
       filterIterator(superGraph->edges(), [filter](edge e) { return filter->getEdgeValue(e); })) {
    addEdge(e);
  }
}
//----------------------------------------------------------------
GraphView::~GraphView() {
  // notify destruction
  observableDeleted();
}
//----------------------------------------------------------------
edge GraphView::existEdge(const node src, const node tgt, bool directed) const {
  if (!isElement(src) || !isElement(tgt)) {
    return edge();
  }

  std::vector<edge> edges = getRootImpl()->getEdges(src, tgt, directed, this);
  return !edges.empty() ? edges[0] : edge();
}
//----------------------------------------------------------------
void GraphView::reverseInternal(const edge e, const node src, const node tgt) {
  if (isElement(e)) {
    SGraphNodeData &srcData = _nodeData[src];
    SGraphNodeData &tgtData = _nodeData[tgt];
    srcData.outEdgeRemove(e);
    srcData.inEdgeAdd(e);
    tgtData.inEdgeRemove(e);
    tgtData.outEdgeAdd(e);

    notifyReverseEdge(e);

    // propagate edge reversal on subgraphs
    for (Graph *sg : subGraphs()) {
      static_cast<GraphView *>(sg)->reverseInternal(e, src, tgt);
    }
  }
}
//----------------------------------------------------------------
void GraphView::setEndsInternal(const edge e, node src, node tgt, const node newSrc,
                                const node newTgt) {
  if (isElement(e)) {
    if (isElement(newSrc) && isElement(newTgt)) {
      notifyBeforeSetEnds(e);

      if (src != newSrc) {
        _nodeData[newSrc].outEdgeAdd(e);

        if (src.isValid() && isElement(src)) {
          _nodeData[src].outEdgeRemove(e);
        } else {
          // as src may no longer exist (pop case)
          // set src as invalid for subgraphs loop
          src = node();
        }
      }

      if (tgt != newTgt) {
        _nodeData[newTgt].inEdgeAdd(e);

        if (tgt.isValid() && isElement(tgt)) {
          _nodeData[tgt].inEdgeRemove(e);
        } else {
          // as tgt may no longer exist (pop case)
          // set tgt as invalid for subgraphs loop
          tgt = node();
        }
      }

      // notification
      notifyAfterSetEnds(e);

      // propagate edge ends update on subgraphs
      for (Graph *sg : subGraphs()) {
        static_cast<GraphView *>(sg)->setEndsInternal(e, src, tgt, newSrc, newTgt);
      }
    } else {
      // delete e if its new ends do no belong to the graph
      // propagate edge ends update on subgraphs
      for (Graph *sg : subGraphs()) {
        static_cast<GraphView *>(sg)->setEndsInternal(e, src, tgt, newSrc, newTgt);
      }
      notifyDelEdge(e);

      _edges.remove(e);
      propertyContainer->erase(e);
      _nodeData[src].outEdgeRemove(e);
      _nodeData[tgt].inEdgeRemove(e);
    }
  }
}
//----------------------------------------------------------------
node GraphView::addNode() {
  node tmp = getSuperGraph()->addNode();
  restoreNode(tmp);
  return tmp;
}
//----------------------------------------------------------------
std::vector<node> GraphView::addNodes(uint nb) {
  std::vector<node> addedNodes = getSuperGraph()->addNodes(nb);
  if (!addedNodes.empty()) {
    addNodesInternal(addedNodes);
  }
  return addedNodes;
}
//----------------------------------------------------------------
void GraphView::restoreNode(node n) {
  _nodes.add(n);
  _nodeData[n] = SGraphNodeData();
  notifyAddNode(n);
}
//----------------------------------------------------------------
void GraphView::addNodesInternal(const std::vector<node> &nodes) {
  reserveNodes(_nodes.size() + nodes.size());

  for (auto n : nodes) {
    assert(getRootImpl()->isElement(n));
    _nodes.add(n);
    _nodeData[n] = SGraphNodeData();
  }

  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEventType::TLP_ADD_NODES, nodes.size()));
  }
}
//----------------------------------------------------------------
void GraphView::addNode(const node n) {
  assert(getRoot()->isElement(n));

  if (!isElement(n)) {
    if (!getSuperGraph()->isElement(n)) {
      getSuperGraph()->addNode(n);
    }

    restoreNode(n);
  }
}
//----------------------------------------------------------------
void GraphView::addNodes(const vector<node> &addedNodes) {
  std::vector<node> nodes;
  std::vector<node> superNodes;
  Graph *super = getSuperGraph();
  bool superIsRoot = (super == getRoot());

  nodes.reserve(addedNodes.size());
  superNodes.reserve(addedNodes.size());
  for (auto n : uniqueIterator(addedNodes)) {
    if (!isElement(n)) {
      nodes.push_back(n);

      if (!superIsRoot && !super->isElement(n)) {
        superNodes.push_back(n);
      }
    }
  }

  if (!superNodes.empty()) {
    super->addNodes(superNodes);
  }

  if (!nodes.empty()) {
    addNodesInternal(nodes);
  }
}
//----------------------------------------------------------------
edge GraphView::addEdgeInternal(edge e) {
  _edges.add(e);
  const auto &[src, tgt] = ends(e);
  _nodeData[src].outEdgeAdd(e);
  _nodeData[tgt].inEdgeAdd(e);
  notifyAddEdge(e);
  return e;
}
//----------------------------------------------------------------
void GraphView::restoreEdge(edge e, const node, const node) {
  addEdgeInternal(e);
}
//----------------------------------------------------------------
void GraphView::addEdgesInternal(const std::vector<edge> &edges) {
  reserveEdges(_edges.size() + edges.size());

  for (auto e : edges) {
    assert(getRootImpl()->isElement(e));
    _edges.add(e);
    const auto &[src, tgt] = ends(e);
    _nodeData[src].outEdgeAdd(e);
    _nodeData[tgt].inEdgeAdd(e);
  }

  if (hasOnlookers()) {
    sendEvent(GraphEvent(*this, GraphEventType::TLP_ADD_EDGES, edges.size()));
  }
}
//----------------------------------------------------------------
edge GraphView::addEdge(const node n1, const node n2) {
  assert(isElement(n1));
  assert(isElement(n2));
  return addEdgeInternal(getSuperGraph()->addEdge(n1, n2));
}
//----------------------------------------------------------------
void GraphView::addEdge(const edge e) {
  assert(getRootImpl()->isElement(e));
  assert(isElement(source(e)));
  assert(isElement(target(e)));

  if (!isElement(e)) {
    if (!getSuperGraph()->isElement(e)) {
      getSuperGraph()->addEdge(e);
    }

    addEdgeInternal(e);
  }
}
//----------------------------------------------------------------
std::vector<edge> GraphView::addEdges(const std::vector<std::pair<node, node>> &ends) {
  std::vector<edge> addedEdges = getSuperGraph()->addEdges(ends);
  if (!addedEdges.empty()) {
    addEdgesInternal(addedEdges);
  }
  return addedEdges;
}
//----------------------------------------------------------------
void GraphView::addEdges(const vector<edge> &addedEdges) {
  std::vector<edge> edges;
  std::vector<edge> superEdges;
  Graph *super = getSuperGraph();
  bool superIsRoot = (super == getRoot());

  edges.reserve(addedEdges.size());
  superEdges.reserve(addedEdges.size());
  for (auto e : uniqueIterator(addedEdges)) {
    assert(getRootImpl()->isElement(e));
    assert(isElement(source(e)));
    assert(isElement(target(e)));

    if (!isElement(e)) {
      edges.push_back(e);

      if (!superIsRoot && !super->isElement(e)) {
        superEdges.push_back(e);
      }
    }
  }

  if (!superEdges.empty()) {
    super->addEdges(superEdges);
  }

  if (!edges.empty()) {
    addEdgesInternal(edges);
  }
}
//----------------------------------------------------------------
void GraphView::removeNode(const node n) {
  assert(isElement(n));
  notifyDelNode(n);
  _nodeData.remove(n);
  _nodes.remove(n);
  propertyContainer->erase(n);
}
//----------------------------------------------------------------
void GraphView::removeNode(const node n, const std::vector<edge> &edges) {
  removeEdges(edges);
  removeNode(n);
}
//----------------------------------------------------------------
void GraphView::delNode(const node n, bool deleteInAllGraphs) {
  if (deleteInAllGraphs) {
    getRootImpl()->delNode(n, true);
  } else {
    assert(isElement(n));

    // get edges vector
    std::vector<edge> edges = this->incidence(n);

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

    removeNode(n, edges);
  }
}
//----------------------------------------------------------------
void GraphView::removeEdge(const edge e) {
  assert(isElement(e));
  notifyDelEdge(e);

  _edges.remove(e);
  propertyContainer->erase(e);
  const auto &[src, tgt] = ends(e);
  _nodeData[src].outEdgeRemove(e);
  _nodeData[tgt].inEdgeRemove(e);
}
//----------------------------------------------------------------
void GraphView::removeEdges(const std::vector<edge> &edges) {
  for (auto e : edges) {
    if (isElement(e)) {
      removeEdge(e);
    }
  }
}
//----------------------------------------------------------------
void GraphView::delEdge(const edge e, bool deleteInAllGraphs) {
  if (deleteInAllGraphs) {
    getRootImpl()->delEdge(e, true);
  } else {
    assert(isElement(e));
    // propagate to subgraphs
    for (Graph *subGraph : subGraphs()) {
      if (subGraph->isElement(e)) {
        subGraph->delEdge(e);
      }
    }

    removeEdge(e);
  }
}
//----------------------------------------------------------------
Iterator<node> *GraphView::getNodes() const {
  return stlIterator(_nodes);
}
//----------------------------------------------------------------
Iterator<node> *GraphView::getInNodes(const node n) const {
  return conversionIterator<node>(getInEdges(n), [this](edge e) { return source(e); });
}
//----------------------------------------------------------------
Iterator<node> *GraphView::getOutNodes(const node n) const {
  return conversionIterator<node>(getOutEdges(n), [this](edge e) { return target(e); });
}
//----------------------------------------------------------------
Iterator<node> *GraphView::getInOutNodes(const node n) const {
  return conversionIterator<node>(getInOutEdges(n), [n, this](edge e) { return opposite(e, n); });
}
//----------------------------------------------------------------
Iterator<edge> *GraphView::getEdges() const {
  return stlIterator(_edges);
}
//----------------------------------------------------------------
enum EdgeDirection { OUT = 0, IN = 1 };
template <EdgeDirection edgeDirection>
struct EdgeDirectionFilter {
  set<edge> loops;
  const GraphView *graph;
  node n;

  EdgeDirectionFilter(const GraphView *graph, node n) : graph(graph), n(n) {}

  bool operator()(edge e) {
    bool seenLoop = loops.contains(e);
    if (graph->source(e) == graph->target(e)) {
      loops.insert(e);
    }
    if constexpr (edgeDirection == OUT) {
      return !seenLoop && graph->source(e) == n;
    } else {
      return !seenLoop && graph->target(e) == n;
    }
  }
};
//----------------------------------------------------------------
Iterator<edge> *GraphView::getInEdges(const node n) const {
  assert(isElement(n));
  return filterIterator(getInOutEdges(n), EdgeDirectionFilter<IN>(this, n));
}
//----------------------------------------------------------------
Iterator<edge> *GraphView::getOutEdges(const node n) const {
  assert(isElement(n));
  return filterIterator(getInOutEdges(n), EdgeDirectionFilter<OUT>(this, n));
}
//----------------------------------------------------------------
Iterator<edge> *GraphView::getInOutEdges(const node n) const {
  assert(isElement(n));
  return stlIterator(_nodeData[n].incidence);
}
//----------------------------------------------------------------
std::vector<edge> GraphView::getEdges(const node src, const node tgt, bool directed) const {
  std::vector<edge> edges;

  if (isElement(src) && isElement(tgt)) {
    edges = getRootImpl()->getEdges(src, tgt, directed, this);
  }

  return edges;
}
//----------------------------------------------------------------
void GraphView::reserveNodes(uint nb) {
  _nodeData.alloc(this, nb);
  _nodes.reserve(nb);
}
//----------------------------------------------------------------
void GraphView::reserveEdges(uint nb) {
  _edges.reserve(nb);
}
//----------------------------------------------------------------
bool GraphView::canPop() {
  return getRootImpl()->canPop();
}
//----------------------------------------------------------------
bool GraphView::canUnpop() {
  return getRootImpl()->canUnpop();
}
//----------------------------------------------------------------
bool GraphView::canPopThenUnpop() {
  return getRootImpl()->canPopThenUnpop();
}
//----------------------------------------------------------------
void GraphView::push(bool unpopAllowed,
                     std::vector<PropertyInterface *> *propertiesToPreserveOnPop) {
  getRootImpl()->push(unpopAllowed, propertiesToPreserveOnPop);
}
//----------------------------------------------------------------
void GraphView::pop(bool unpopAllowed) {
  getRootImpl()->pop(unpopAllowed);
}
//----------------------------------------------------------------
void GraphView::popIfNoUpdates() {
  getRootImpl()->popIfNoUpdates();
}
//----------------------------------------------------------------
void GraphView::unpop() {
  getRootImpl()->unpop();
}
}
