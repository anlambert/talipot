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

#include <talipot/GraphStorage.h>
#include <talipot/Graph.h>

using namespace tlp;

//=======================================================
void GraphStorage::clear() {
  nodeData.clear();
  nodeIds.clear();
  edgeIds.clear();
  edgeEnds.clear();
}
//=======================================================
/**
 * @brief reserve memory for nb nodes
 */
void GraphStorage::reserveNodes(const size_t nb) {
  if (nb > nodeData.capacity()) {
    nodeData.reserve(nb);
    nodeIds.reserve(nb);
  }
}
//=======================================================
/**
 * @brief reserve memory for nb edges
 */
void GraphStorage::reserveEdges(const size_t nb) {
  if (nb > edgeEnds.capacity()) {
    edgeEnds.reserve(nb);
    edgeIds.reserve(nb);
  }
}
//=======================================================
/**
 * @brief Return the current state of the ids management
 * must be deleted by the caller
 */
const GraphStorageIdsMemento *GraphStorage::getIdsMemento() const {
  auto *memento = new GraphStorageIdsMemento();
  memento->nodeIds = nodeIds;
  memento->edgeIds = edgeIds;
  return memento;
}
//=======================================================
/**
 * @brief restore a state of the ids management
 */
void GraphStorage::restoreIdsMemento(const GraphStorageIdsMemento *memento) {
  nodeIds = memento->nodeIds;
  edgeIds = memento->edgeIds;
}
//=======================================================
// specific iterator classes used to implement
// the get*Nodes & get*Edges methods

// define some values for further template specializations
// IO_IN => in nodes/edges
// IO_OUT => out nodes/edges
// IO_INOUT => inout nodes/edges
// IO_ prefix is needed on windows platform
// to avoid compilation failure
enum IO_TYPE { IO_IN = 0, IO_OUT = 1, IO_INOUT = 2 };

// define a template class to iterate on in or out edges of a given node
template <IO_TYPE io_type>
struct IOEdgeContainerIterator : public Iterator<edge>,
                                 public MemoryPool<IOEdgeContainerIterator<io_type>> {
  node n;
  edge curEdge;
  std::set<edge> loops;
  const std::vector<std::pair<node, node>> &edges;
  std::vector<edge>::iterator it, itEnd;

  void prepareNext() {
    for (; it != itEnd; ++it) {
      curEdge = *it;
      const auto &[curEdgeSrc, curEdgeTgt] = edges[curEdge.id];
      // note that io_type value may only be IO_IN which is null
      // or IO_OUT which is define to 1
      node curNode;

      if constexpr (io_type != IO_IN) {
        curNode = curEdgeSrc;
      } else {
        curNode = curEdgeTgt;
      }

      if (curNode != n) {
        continue;
      }

      if constexpr (io_type != IO_IN) {
        curNode = curEdgeTgt;
      } else {
        curNode = curEdgeSrc;
      }

      if (curNode == n) {
        if (!loops.contains(curEdge)) {
          loops.insert(curEdge);
          ++it;
          return;
        }
      } else {
        ++it;
        return;
      }
    }

    // set curEdge as invalid
    curEdge = edge();
  }

  IOEdgeContainerIterator(node n, std::vector<edge> &v,
                          const std::vector<std::pair<node, node>> &edges)
      : n(n), edges(edges), it(v.begin()), itEnd(v.end()) {
    prepareNext();
  }

  ~IOEdgeContainerIterator() override = default;

  edge next() override {
    // check hasNext()
    assert(curEdge.isValid());
    // we are already pointing to the next
    edge tmp = curEdge;
    // anticipating the next iteration
    prepareNext();
    return tmp;
  }

  bool hasNext() override {
    return (curEdge.isValid());
  }
};

// define a class to iterate on in or out nodes of a given node
template <IO_TYPE io_type>
struct IONodesIterator : public Iterator<node>, public MemoryPool<IONodesIterator<io_type>> {
  node n;
  const std::vector<std::pair<node, node>> &edges;
  Iterator<edge> *it;

  IONodesIterator(node n, std::vector<edge> &nEdges,
                  const std::vector<std::pair<node, node>> &edges)
      : n(n), edges(edges) {
    if constexpr (io_type == IO_INOUT) {
      it = stlIterator(nEdges);
    } else {
      it = new IOEdgeContainerIterator<io_type>(n, nEdges, edges);
    }
  }

  ~IONodesIterator() override {
    delete it;
  }

  bool hasNext() override {
    return (it->hasNext());
  }

  node next() override {
    // check hasNext()
    assert(it->hasNext());
    const auto &[src, tgt] = edges[it->next()];

    // out nodes
    if constexpr (io_type == IO_OUT) {
      return tgt;
    }
    // in nodes
    else if constexpr (io_type == IO_IN) {
      return src;
    }
    // inout nodes
    else {
      return (src == n) ? tgt : src;
    }
  }
};
//=======================================================
Iterator<edge> *GraphStorage::getInOutEdges(const node n) const {
  return stlIterator(nodeData[n.id].edges);
}
//=======================================================
std::vector<edge> GraphStorage::getEdges(const node src, const node tgt, bool directed,
                                         const Graph *sg) const {

  std::vector<edge> edges;

  for (auto e : nodeData[src.id].edges) {
    const auto &[eSrc, eTgt] = edgeEnds[e.id];

    if (((eTgt == tgt && eSrc == src) || (!directed && eSrc == tgt && eTgt == src)) &&
        (!sg || sg->isElement(e))) {
      edges.push_back(e);
    }
  }

  // remove possible duplicates due to self loops appearing twice
  std::sort(edges.begin(), edges.end());
  edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

  return edges;
}
//=======================================================
Iterator<edge> *GraphStorage::getOutEdges(const node n) const {
  return new IOEdgeContainerIterator<IO_OUT>(n, nodeData[n.id].edges, edgeEnds);
}
//=======================================================
Iterator<edge> *GraphStorage::getInEdges(const node n) const {
  return new IOEdgeContainerIterator<IO_IN>(n, nodeData[n.id].edges, edgeEnds);
}
//=======================================================
Iterator<node> *GraphStorage::getInOutNodes(const node n) const {
  return new IONodesIterator<IO_INOUT>(n, nodeData[n.id].edges, edgeEnds);
}
//=======================================================
Iterator<node> *GraphStorage::getInNodes(const node n) const {
  return new IONodesIterator<IO_IN>(n, nodeData[n.id].edges, edgeEnds);
}
//=======================================================
Iterator<node> *GraphStorage::getOutNodes(const node n) const {
  return new IONodesIterator<IO_OUT>(n, nodeData[n.id].edges, edgeEnds);
}
//=======================================================
/**
 * @brief Reconnect the edge e to have the new given ends
 */
void GraphStorage::setEnds(const edge e, const node newSrc, const node newTgt) {

  assert(isElement(e));
  auto [src, tgt] = edgeEnds[e.id];

  // nothing to do if same ends
  if (src == newSrc && tgt == newTgt) {
    return;
  }

  node nSrc = newSrc;

  if (newSrc.isValid() && src != newSrc) {
    assert(isElement(newSrc));
    edgeEnds[e.id].first = newSrc;
    NodeData &sCtnr = nodeData[src.id];
    NodeData &nCtnr = nodeData[newSrc.id];
    sCtnr.outDegree -= 1;
    nCtnr.outDegree += 1;
    nCtnr.edges.push_back(e);
    removeFromNodeData(sCtnr, e);
  } else {
    nSrc = src;
  }

  if (newTgt.isValid() && tgt != newTgt) {
    assert(isElement(newTgt));
    edgeEnds[e.id].second = newTgt;
    nodeData[newTgt.id].edges.push_back(e);
    if (tgt != nSrc) {
      // remove edge from node data only if previous target
      // does not become the new source
      removeFromNodeData(nodeData[tgt.id], e);
    }
  }
}
//=======================================================
/**
 * @brief Reverse an edge e, source become target and target become source
 */
void GraphStorage::reverse(const edge e) {
  assert(isElement(e));
  auto &[src, tgt] = edgeEnds[e.id];
  nodeData[src.id].outDegree -= 1;
  nodeData[tgt.id].outDegree += 1;
  std::swap(src, tgt);
}
//=======================================================
/**
 *  \brief Set the ordering of edges around n according to their order in v.
 */
void GraphStorage::setEdgeOrder(const node n, const std::vector<edge> &edges) {
  nodeData[n.id].edges = edges;
}
//=======================================================
/**
 * \brief swap to edge in the ordered adjacency vector
 * \warning the two edges must be element of star(v)
 * @complexity o(1)
 */
void GraphStorage::swapEdgeOrder(const node n, const edge e1, const edge e2) {
  if (e1 == e2) {
    return;
  }

  std::vector<edge> &adjacency = nodeData[n.id].edges;
  uint e1Pos = UINT_MAX, e2Pos = UINT_MAX;

  for (uint i = 0; i < deg(n); ++i) {
    if (adjacency[i] == e1) {
      e1Pos = i;
    }

    if (adjacency[i] == e2) {
      e2Pos = i;
    }

    if (e1Pos != UINT_MAX && e2Pos != UINT_MAX) {
      break;
    }
  }

  assert(e1Pos != UINT_MAX && e2Pos != UINT_MAX);
  adjacency[e1Pos] = e2;
  adjacency[e2Pos] = e1;
}
//=======================================================
/**
 * @brief restore the given node in the structure and return it
 */
void GraphStorage::restoreNode(const node n) {
  if (n.id >= nodeData.size()) {
    nodeData.resize(n.id + 1);
  } else {
    NodeData &nData = nodeData[n.id];
    // clear edge infos
    nData.edges.clear();
    nData.outDegree = 0;
  }
}
//=======================================================
/**
 * @brief Add a new node in the structure and return it
 * @warning: That operation modify the array of nodes
 * and thus devalidate all iterators on it.
 * @complexity: o(1)
 */
node GraphStorage::addNode() {
  node n(nodeIds.add());
  restoreNode(n);
  return n;
}
//=======================================================
/**
 * @brief Add nb new nodes in the structure
 * and return them in addedNodes
 */
std::vector<node> GraphStorage::addNodes(uint nb) {
  std::vector<node> addedNodes = nodeIds.addNb(nb);
  uint sz = nodeData.size();

  if (sz < nodeIds.size()) {
    nodeData.resize(nodeIds.size());
    // get the number of recycled nodes
    // that need to be restored
    nb -= nodeIds.size() - sz;
  }

  for (auto n : addedNodes) {
    restoreNode(n);
  }
  return addedNodes;
}
//=======================================================
/**
 * @brief remove a node from the nodes structure
 */
void GraphStorage::removeFromNodes(const node n) {
  NodeData &nData = nodeData[n.id];
  // clear edge infos
  nData.edges.clear();
  nData.outDegree = 0;
  // push in free pool
  nodeIds.free(n);

  if (nodeIds.empty()) {
    nodeData.resize(0);
  }
}
//=======================================================
/**
 * @brief Delete a node and all its adjacent edges in the graph
 */
void GraphStorage::delNode(const node n) {
  assert(isElement(n));
  std::vector<edge> loops;

  const std::vector<edge> &edges = nodeData[n.id].edges;

  for (auto e : edges) {
    const auto &[src, tgt] = ends(e);

    if (src != tgt) {
      if (src != n) {
        nodeData[src.id].outDegree -= 1;
      }

      removeFromEdges(e, n);
    } else {
      loops.push_back(e);
    }
  }

  for (auto e : loops) {
    removeFromEdges(e, n);
  }

  removeFromNodes(n);
}
//=======================================================
/**
 * @brief Add a new edge between src and tgt and return it
 * @warning That operation modify the array of edges and
 * the adjacency edges of its ends thus any iterators existing for
 * these structures will be devalidated.
 */
void GraphStorage::restoreEdge(const node src, const node tgt, const edge e) {
  edgeEnds[e.id] = {src, tgt};
  nodeData[src.id].outDegree += 1;
}
//=======================================================
/**
 * @brief Add a new edge between src and tgt and return it
 * @warning That operation modify the array of edges and
 * the adjacency edges of its ends thus any iterators existing for
 * these structures will be devalidated.
 */
edge GraphStorage::addEdge(const node src, const node tgt) {
  edge e(edgeIds.add());

  if (e.id == edgeEnds.size()) {
    edgeEnds.resize(e.id + 1);
  }

  edgeEnds[e.id] = {src, tgt};
  NodeData &srcData = nodeData[src.id];
  srcData.outDegree += 1;
  srcData.edges.push_back(e);
  nodeData[tgt.id].edges.push_back(e);

  return e;
}
//=======================================================
/**
 * @brief Add edges in the structure and returns them
 * in the addedEdges vector
 */
std::vector<edge> GraphStorage::addEdges(const std::vector<std::pair<node, node>> &ends) {
  uint nb = ends.size();
  std::vector<edge> addedEdges = edgeIds.addNb(nb);
  uint sz = edgeEnds.size();

  if (sz < edgeIds.size()) {
    edgeEnds.resize(edgeIds.size());
  }

  for (uint i = 0; i < nb; ++i) {
    const auto &[src, tgt] = ends[i];
    edge e = addedEdges[i];
    edgeEnds[e.id] = {src, tgt};
    NodeData &srcData = nodeData[src.id];
    srcData.outDegree += 1;
    srcData.edges.push_back(e);
    nodeData[tgt.id].edges.push_back(e);
  }

  return addedEdges;
}
//=======================================================
/**
 * @brief Delete an edge in the graph
 */
void GraphStorage::delEdge(const edge e) {
  unsigned srcId = source(e).id;
  nodeData[srcId].outDegree -= 1;
  removeFromEdges(e);
}
//=======================================================
/**
 * @brief Delete all edges in the graph
 */
void GraphStorage::delAllEdges() {
  edgeEnds.clear();
  edgeIds.clear();

  // loop on nodes to clear adjacency edges
  for (auto &nd : nodeData) {
    nd.edges.clear();
  }
}
//=======================================================
/**
 * @brief Delete all nodes in the graph
 */
void GraphStorage::delAllNodes() {
  clear();
}

// member functions below do not belong to the public API
// they are just needed by the current implementation
//=======================================================
/**
 * @brief remove an edge from a NodeData
 */
void GraphStorage::removeFromNodeData(NodeData &c, const edge e) {
  c.edges.erase(std::remove(c.edges.begin(), c.edges.end(), e), c.edges.end());
}
//=======================================================
/**
 * @brief remove an edge from the edges structure
 * and from the NodeData of its ends
 * except for the end node in argument if it is valid
 */
void GraphStorage::removeFromEdges(const edge e, node end) {
  edgeIds.free(e);
  const auto &[src, tgt] = edgeEnds[e.id];
  // remove from source's edges
  if (src != end) {
    removeFromNodeData(nodeData[src.id], e);
  }

  // remove from target's edges
  if (tgt != end) {
    removeFromNodeData(nodeData[tgt.id], e);
  }
}
