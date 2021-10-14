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

#ifndef TALIPOT_VECTOR_PROPERTY_H
#define TALIPOT_VECTOR_PROPERTY_H

#include <talipot/Graph.h>
#include <talipot/GraphParallelTools.h>
#include <talipot/NumericProperty.h>

namespace tlp {
template <typename TYPE>
class NodeVectorProperty : public std::vector<TYPE> {
  const Graph *graph;

public:
  // constructor
  NodeVectorProperty(const Graph *g = nullptr) {
    alloc(g);
  }

  void alloc(const Graph *g) {
    graph = g;
    if (graph && graph->numberOfNodes()) {
      // set the vector size to the number of graph nodes
      this->resize(graph->numberOfNodes());
    }
  }

  void alloc(const Graph *g, size_t size) {
    graph = g;
    this->resize(size);
  }

  typename std::vector<TYPE>::const_reference operator[](uint i) const {
    return std::vector<TYPE>::operator[](i);
  }

  typename std::vector<TYPE>::reference operator[](uint i) {
    if (i + 1 > this->size()) {
      this->resize(i + 1);
    }
    return std::vector<TYPE>::operator[](i);
  }

  typename std::vector<TYPE>::const_reference operator[](node n) const {
    assert(graph);
    return (*this)[graph->nodePos(n)];
  }

  typename std::vector<TYPE>::reference operator[](node n) {
    assert(graph);
    return (*this)[nodePos(n)];
  }

  // get the stored value of a node
  typename std::vector<TYPE>::const_reference getNodeValue(node n) const {
    return (*this)[n];
  }

  // set the stored value of a node
  void setNodeValue(node n, TYPE val) {
    (*this)[n] = val;
  }

  // set all to same values
  void setAll(const TYPE &val) {
    assert(graph);
    TLP_PARALLEL_MAP_INDICES(graph->numberOfNodes(), [&](uint i) { (*this)[i] = val; });
  }

  uint nodePos(node n) {
    assert(graph);
    uint nPos = graph->nodePos(n);

    if (nPos + 1 > this->size()) {
      this->resize(nPos + 1);
    }
    return nPos;
  }

  void remove(node n) {
    // last node gets the removed node position
    (*this)[n] = this->back();
    this->resize(this->size() - 1);
  }

  // get values from a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyFromProperty(PROP_PTR prop) {
    assert(graph);
    this->resize(graph->numberOfNodes());
    TLP_PARALLEL_MAP_NODES(graph, [&](const node n) { (*this)[n] = (*prop)[n]; });
  }

  // get values from a NumericProperty
  void copyFromNumericProperty(const NumericProperty *prop) {
    assert(graph);
    this->resize(graph->numberOfNodes());
    TLP_PARALLEL_MAP_NODES(graph, [&](const node n) { (*this)[n] = prop->getNodeDoubleValue(n); });
  }

  // copy values into a typed typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyToProperty(PROP_PTR prop) {
    assert(graph);
    const std::vector<node> &nodes = graph->nodes();
    uint nbNodes = nodes.size();

    for (uint i = 0; i < nbNodes; ++i) {
      prop->setNodeValue(nodes[i], (*this)[i]);
    }
  }
};

template <>
class NodeVectorProperty<bool> : public std::vector<unsigned char> {
  const Graph *graph;

public:
  // constructor
  NodeVectorProperty(const Graph *g = nullptr) {
    alloc(g);
  }

  void alloc(const Graph *g) {
    graph = g;
    if (graph && graph->numberOfNodes()) {
      // set the vector size to the number of graph nodes
      this->resize(graph->numberOfNodes());
    }
  }

  void alloc(const Graph *g, size_t size) {
    graph = g;
    this->resize(size);
  }

  const Graph *getGraph() const {
    return graph;
  }

  bool operator[](uint i) const {
    return static_cast<bool>(std::vector<unsigned char>::operator[](i));
  }

  std::vector<unsigned char>::reference operator[](uint i) {
    if (i + 1 > this->size()) {
      this->resize(i + 1);
    }
    return std::vector<unsigned char>::operator[](i);
  }

  bool operator[](node n) const {
    assert(graph);
    return (*this)[graph->nodePos(n)];
  }

  std::vector<unsigned char>::reference operator[](node n) {
    assert(graph);
    return (*this)[nodePos(n)];
  }

  // get the stored value of a node
  bool getNodeValue(node n) const {
    assert(graph);
    return (*this)[n];
  }

  // set the stored value of a node
  void setNodeValue(node n, bool val) {
    assert(graph);
    (*this)[n] = val;
  }

  // set all to same values
  void setAll(const bool &val) {
    assert(graph);
    TLP_PARALLEL_MAP_INDICES(graph->numberOfNodes(), [&](uint i) { (*this)[i] = val; });
  }

  uint nodePos(node n) {
    assert(graph);
    uint nPos = graph->nodePos(n);

    if (nPos + 1 > this->size()) {
      this->resize(nPos + 1);
    }

    return nPos;
  }

  void remove(node n) {
    // last node gets the removed node position
    (*this)[n] = this->back();
    this->resize(this->size() - 1);
  }

  // get values from a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyFromProperty(PROP_PTR prop) {
    assert(graph);
    this->resize(graph->numberOfNodes());
    TLP_PARALLEL_MAP_NODES(graph, [&](const node n) { (*this)[n] = (*prop)[n]; });
  }

  // copy values into a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyToProperty(PROP_PTR prop) {
    assert(graph);
    const std::vector<node> &nodes = graph->nodes();
    uint nbNodes = nodes.size();

    for (uint i = 0; i < nbNodes; ++i) {
      prop->setNodeValue(nodes[i], (*this)[i]);
    }
  }
};

template <typename TYPE>
class EdgeVectorProperty : public std::vector<TYPE> {
  const Graph *graph;

public:
  // constructor
  EdgeVectorProperty(const Graph *g = nullptr) {
    alloc(g);
  }

  void alloc(const Graph *g) {
    graph = g;
    if (graph && graph->numberOfEdges()) {
      // set the vector size to the number of graph edges
      this->resize(graph->numberOfEdges());
    }
  }

  void alloc(const Graph *g, size_t size) {
    graph = g;
    this->resize(size);
  }

  const Graph *getGraph() const {
    return graph;
  }

  typename std::vector<TYPE>::const_reference operator[](uint i) const {
    return std::vector<TYPE>::operator[](i);
  }

  typename std::vector<TYPE>::reference operator[](uint i) {
    if (i + 1 > this->size()) {
      this->resize(i + 1);
    }
    return std::vector<TYPE>::operator[](i);
  }

  typename std::vector<TYPE>::const_reference operator[](edge e) const {
    assert(graph);
    return (*this)[graph->edgePos(e)];
  }

  typename std::vector<TYPE>::reference operator[](edge e) {
    assert(graph);
    return (*this)[edgePos(e)];
  }

  // get the stored value of a edge
  typename std::vector<TYPE>::const_reference getEdgeValue(edge e) const {
    return (*this)[e];
  }

  // set the stored value of a edge
  void setEdgeValue(edge e, TYPE val) {
    (*this)[e] = val;
  }

  void setAll(const TYPE &val) {
    assert(graph);
    TLP_PARALLEL_MAP_INDICES(graph->numberOfEdges(), [&](uint i) { (*this)[i] = val; });
  }

  // add a value for a newly created edge
  uint edgePos(edge e) {
    assert(graph);
    uint ePos = graph->edgePos(e);

    if (ePos + 1 > this->size()) {
      this->resize(ePos + 1);
    }

    return ePos;
  }

  void remove(edge e) {
    // last edge gets the removed edge position
    (*this)[e] = this->back();
    this->resize(this->size() - 1);
  }

  // get values from a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyFromProperty(PROP_PTR prop) {
    assert(graph);
    this->resize(graph->numberOfEdges());
    TLP_PARALLEL_MAP_EDGES(graph, [&](const edge e) { (*this)[e] = (*prop)[e]; });
  }

  // get values from a NumericProperty
  void copyFromNumericProperty(const NumericProperty *prop) {
    assert(graph);
    this->resize(graph->numberOfEdges());
    TLP_PARALLEL_MAP_EDGES(graph, [&](const edge e) { (*this)[e] = prop->getEdgeDoubleValue(e); });
  }

  // copy values into a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyToProperty(PROP_PTR prop) {
    assert(graph);
    const std::vector<edge> &edges = graph->edges();
    uint nbEdges = edges.size();

    for (uint i = 0; i < nbEdges; ++i) {
      prop->setEdgeValue(edges[i], (*this)[i]);
    }
  }
};

template <>
class EdgeVectorProperty<bool> : public std::vector<unsigned char> {
  const Graph *graph;

public:
  // constructor
  EdgeVectorProperty(const Graph *g = nullptr) {
    alloc(g);
  }

  void alloc(const Graph *g) {
    graph = g;
    if (graph && graph->numberOfEdges()) {
      // set the vector size to the number of graph edges
      this->resize(graph->numberOfEdges());
    }
  }

  void alloc(const Graph *g, size_t size) {
    graph = g;
    this->resize(size);
  }

  bool operator[](uint i) const {
    return static_cast<bool>(std::vector<unsigned char>::operator[](i));
  }

  std::vector<unsigned char>::reference operator[](uint i) {
    if (i + 1 > this->size()) {
      this->resize(i + 1);
    }
    return std::vector<unsigned char>::operator[](i);
  }

  bool operator[](edge e) const {
    assert(graph);
    return (*this)[graph->edgePos(e)];
  }

  std::vector<unsigned char>::reference operator[](edge e) {
    assert(graph);
    return (*this)[edgePos(e)];
  }

  // get the stored value of a edge
  bool getEdgeValue(edge e) const {
    assert(graph);
    return (*this)[e];
  }

  // set the stored value of a edge
  void setEdgeValue(edge e, bool val) {
    assert(graph);
    (*this)[edgePos(e)] = val;
  }

  // set all to same values
  void setAll(const bool &val) {
    assert(graph);
    TLP_PARALLEL_MAP_INDICES(graph->numberOfEdges(), [&](uint i) { (*this)[i] = val; });
  }

  uint edgePos(edge e) {
    assert(graph);
    uint ePos = graph->edgePos(e);

    if (ePos + 1 > this->size()) {
      this->resize(ePos + 1);
    }

    return ePos;
  }

  void remove(edge e) {
    // last edge gets the removed edge position
    (*this)[e] = this->back();
    this->resize(this->size() - 1);
  }

  // get values from a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyFromProperty(PROP_PTR prop) {
    assert(graph);
    this->resize(graph->numberOfEdges());
    TLP_PARALLEL_MAP_EDGES(graph, [&](const edge e) { (*this)[e] = (*prop)[e]; });
  }

  // copy values into a typed instance of PropertyInterface
  template <typename PROP_PTR>
  void copyToProperty(PROP_PTR prop) {
    assert(graph);
    const std::vector<edge> &edges = graph->edges();
    uint nbEdges = edges.size();

    for (uint i = 0; i < nbEdges; ++i) {
      prop->setEdgeValue(edges[i], (*this)[i]);
    }
  }
};
}

#endif // TALIPOT_VECTOR_PROPERTY_H
