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

#ifndef TALIPOT_GRAPH_STORAGE_H
#define TALIPOT_GRAPH_STORAGE_H

#include <cstring>
#include <cassert>
#include <vector>

#include <talipot/Node.h>
#include <talipot/Edge.h>
#include <talipot/IdManager.h>

namespace tlp {

class Graph;

//===========================================
/**
 * @class GraphStorageIdsMemento
 * @brief simple class to save the state of the ids managed by the
 * GraphStorage class
 */
struct GraphStorageIdsMemento {
  IdContainer<node> nodeIds;
  IdContainer<edge> edgeIds;
};
//===========================================
/**
 * @class GraphStorage
 * @brief That class provide a simple implementation
 * for the storage of graph elts (nodes edges)
 */
class GraphStorage {
public:
  //=======================================================
  void clear();
  //=======================================================
  /**
   * @brief Return true if n belongs to the graph
   */
  bool isElement(const node n) const {
    return nodeIds.isElement(n);
  }
  //=======================================================
  /**
   * @brief Return the number of nodes in the graph
   */
  uint numberOfNodes() const {
    return nodeIds.size();
  }
  //=======================================================
  /**
   * @brief Return true if e belongs to the graph
   */
  bool isElement(const edge e) const {
    return edgeIds.isElement(e);
  }
  //=======================================================
  /**
   * @brief Return the number of edges in the graph
   */
  uint numberOfEdges() const {
    return edgeIds.size();
  }
  //=======================================================
  /**
   * @brief Enables to reserve memory for nbNodes
   * Reserving memory before to addNode enable to reduce the number of vector resizing and then
   * to speed up construction of graphs.
   */
  void reserveNodes(const size_t nb);
  //=======================================================
  /**
   * @brief Enables to reserve memory for nbEdges
   * Reserving memory before to addEdge enable to reduce the number of vector resizing and then
   * to speed up construction of graphs.
   */
  void reserveEdges(const size_t nb);
  //=======================================================
  /**
   * @brief return the incident edges of a given node
   */
  const std::vector<edge> &incidence(const node n) const {
    assert(isElement(n));
    return nodeData[n.id].edges;
  }
  //=======================================================
  /**
   * @brief Return the first node of graph
   */
  node getOneNode() const {
    return numberOfNodes() ? nodeIds[0] : node();
  }
  //=======================================================
  /**
   * @brief Return a Talipot iterator on nodes of the graph
   * @warning: The returned iterator should be deleted by the caller to prevent memory leaks
   * @complexity: o(1)
   */
  Iterator<node> *getNodes() const {
    return nodeIds.getElts();
  }
  //=======================================================
  /**
   * @brief Return the current state of the ids management
   * must be deleted by the caller
   * this can be used for push/pop
   */
  const GraphStorageIdsMemento *getIdsMemento() const;
  //=======================================================
  /**
   * @brief  restore a state of the ids management
   */
  void restoreIdsMemento(const GraphStorageIdsMemento *);

  //=======================================================
  /**
   * @brief Return if edges exist between two nodes
   * @param src The source of the hypothetical edges.
   * @param tgt The target of the hypothetical edges.
   * @param directed When set to false edges from target to source are also considered
   * @param edges The vector of edges to fill up with the edges found
   * @param the subgraph owning the edges
   * @param onlyFirst If true only the first edge found will be returned
   * @return true if an edge has been bound
   */
  std::vector<edge> getEdges(const node src, const node tgt, bool directed,
                             const Graph *sg = nullptr) const;

  //=======================================================
  /**
   * @brief Return the degree of a node
   */
  uint deg(const node n) const {
    assert(isElement(n));
    return nodeData[n.id].edges.size();
  }
  //=======================================================
  /**
   * @brief Return the out degree of a node
   */
  uint outdeg(const node n) const {
    assert(isElement(n));
    return nodeData[n.id].outDegree;
  }
  //=======================================================
  /**
   * @brief Return the in degree of a node
   */
  uint indeg(const node n) const {
    assert(isElement(n));
    const NodeData &ctnr = nodeData[n.id];
    return ctnr.edges.size() - ctnr.outDegree;
  }
  //=======================================================
  /**
   * @brief Return the edges of the graph
   */
  const std::vector<edge> &edges() const {
    return edgeIds;
  }
  //=======================================================
  /**
   * @brief Return the position of an edge in the edges of the graph
   */
  uint edgePos(const edge e) const {
    return edgeIds.getPos(e);
  }
  //=======================================================
  /**
   * @brief Return the nodes of the graph
   */
  const std::vector<node> &nodes() const {
    return nodeIds;
  }
  //=======================================================
  /**
   * @brief Return the position of a node in the nodes of the graph
   */
  uint nodePos(const node n) const {
    return nodeIds.getPos(n);
  }
  //=======================================================
  /**
   * @brief Return the extremities of an edge (src, target)
   */
  const std::pair<node, node> &ends(const edge e) const {
    assert(isElement(e));
    return edgeEnds[e.id];
  }
  //=======================================================
  /**
   * @brief return the first extremity (considered as source if the graph is directed) of an edge
   */
  node source(const edge e) const {
    assert(isElement(e));
    return edgeEnds[e.id].first;
  }
  //=======================================================
  /**
   * @brief return the second extremity (considered as target if the graph is directed) of an edge
   */
  node target(const edge e) const {
    assert(isElement(e));
    return edgeEnds[e.id].second;
  }
  //=======================================================
  /**
   * @brief return the opposite node of n through edge e
   */
  node opposite(const edge e, const node n) const {
    assert(isElement(e));
    const auto &[src, tgt] = edgeEnds[e.id];
    assert((src == n) || (tgt == n));
    return (src == n) ? tgt : src;
  }

  //=======================================================
  /**
   * @brief Reconnect an edge to new ends
   */
  void setEnds(const edge e, const node newSrc, const node newTgt);
  //=======================================================
  /**
   * @brief change the source of an edge
   * \see setEnds
   */
  void setSource(const edge e, const node n) {
    setEnds(e, n, node());
  }
  //=======================================================
  /**
   * @brief change the target of an edge
   * \see setEnds
   */
  void setTarget(const edge e, const node n) {
    setEnds(e, node(), n);
  }
  //=======================================================
  /**
   * @brief Reverse an edge e, source become target and target become source
   */
  void reverse(const edge e);
  //=======================================================
  /**
   * \brief Set the ordering of edges around n according to their order in v.
   */
  void setEdgeOrder(const node n, const std::vector<edge> &v);
  //=======================================================
  /**
   * \brief Swap to edges in the incidence list of a node
   */
  void swapEdgeOrder(const node n, const edge e1, const edge e2);
  //=======================================================
  /**
   * @brief Add the given node in the structure and return it
   * @warning: That operation modify the array of nodes
   * and thus devalidate all iterators on it.
   * @complexity: o(1)
   */
  void restoreNode(const node n);
  //=======================================================
  /**
   * @brief Add a new node in the structure and return it
   * @warning: That operation modify the array of nodes
   * and thus devalidate all iterators on it.
   * @complexity: o(1)
   */
  node addNode();
  //=======================================================
  /**
   * @brief Add nb new nodes in the structure and return them
   * in a vector
   * @warning: That operation modifies the array of nodes
   * and thus devalidate all iterators on it.
   * @complexity: o(1)
   */
  std::vector<node> addNodes(uint nb);
  //=======================================================
  /**
   * @brief remove a node from the nodes structure only
   * @warning That operation modify the array of nodes
   * and thus devalidate all iterators on it.
   * @complexity: o(1)
   */
  void removeFromNodes(const node n);
  //=======================================================
  /**
   * @brief Delete a node and all incident edges in the graph
   */
  void delNode(const node n);
  //=======================================================
  /**
   * @brief restore the given edge between src and tgt and return it
   */
  void restoreEdge(const node src, const node tgt, const edge e);
  //=======================================================
  /**
   * @brief Add a new edge between src and tgt and return it
   */
  edge addEdge(const node src, const node tgt);
  //=======================================================
  /**
   * @brief Add edges in the graph structure and returns them
   * in a vector
   */
  std::vector<edge> addEdges(const std::vector<std::pair<node, node>> &edges);
  //=======================================================
  /**
   * @brief Delete an edge in the graph
   */
  void delEdge(const edge e);
  //=======================================================
  /**
   * @brief Delete all edges in the graph
   * @warning: That operation modify the array of edges and all arrays of nodes
   * and thus devalidate all iterators, only graph nodes iterators are not affected.
   */
  void delAllEdges();
  //=======================================================
  /**
   * @brief Delete all nodes in the graph
   * @warning: That operation modify the array of edges and all arrays of nodes
   * and thus devalidate all iterators.
   */
  void delAllNodes();
  //=======================================================
  /**
   * @brief sort the graph elements in ascending order
   * @warning: That operation modify the vector of nodes and the vector of edges
   * and thus devalidate all iterators.
   */
  void sortElts() {
    nodeIds.sort();
    edgeIds.sort();
  }
  //=======================================================
private:
  // specific types
  struct NodeData {
    std::vector<edge> edges;
    uint outDegree;

    NodeData() : outDegree(0) {}
  };

  // data members
  mutable std::vector<std::pair<node, node>> edgeEnds;
  mutable std::vector<NodeData> nodeData;
  IdContainer<node> nodeIds;
  IdContainer<edge> edgeIds;

  // member functions below do not belong to the public API
  // they are just needed by the current implementation
  //=======================================================
  /**
   * @brief remove an edge from an NodeData
   * @warning That operation modify the NodeData
   * and thus devalidate all iterators on it.
   */
  static void removeFromNodeData(NodeData &c, const edge e);
  //=======================================================
  /**
   * @brief remove an edge from the edges structure
   * and from the NodeData of its ends
   * except for the end node in argument if it is valid
   * @warning That operation modify the array of edges
   * and thus devalidate all iterators on it.
   */
  void removeFromEdges(const edge e, node end = node());
};
}
#endif // TALIPOT_GRAPH_STORAGE_H
