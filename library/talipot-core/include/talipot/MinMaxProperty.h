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

#ifndef TALIPOT_MIN_MAX_PROPERTY_H
#define TALIPOT_MIN_MAX_PROPERTY_H

#include <talipot/hash.h>

#include <talipot/Observable.h>
#include <talipot/AbstractProperty.h>

#define MINMAX_PAIR(TYPE) std::pair<REAL_TYPE(TYPE), REAL_TYPE(TYPE)>
#define MINMAX_MAP(TYPE) typename flat_hash_map<uint, MINMAX_PAIR(TYPE)>

namespace tlp {

/**
 * @brief Abstracts the computation of minimal and maximal values on node and edge values of
 *properties.
 *
 * The value is lazily computed on first request.
 * The value is cached, and the cache is invalidated whenever it cannot be simply updated.
 **/
template <typename NodeType, typename EdgeType, typename PropType = PropertyInterface>
class MinMaxProperty : public tlp::AbstractProperty<NodeType, EdgeType, PropType> {
public:
  /**
   * @brief Constructs a MinMaxProperty.
   *
   * @param graph The graph to attach the property to.
   * @param name The name of the property.
   * @param nodeMin The minimum value the property can take for nodes (e.g. INT_MIN)
   * @param nodeMax The maximum value the property can take for nodes (e.g. INT_MAX)
   * @param edgeMin The minimum value the property can take for edges (e.g. INT_MIN)
   * @param edgeMax The maximum value the property can take for edges (e.g. INT_MAX)
   **/
  MinMaxProperty(tlp::Graph *graph, const std::string &name, REAL_TYPE(NodeType) nodeMin,
                 REAL_TYPE(NodeType) nodeMax, REAL_TYPE(EdgeType) edgeMin,
                 REAL_TYPE(EdgeType) edgeMax);

  void treatEvent(const tlp::Event &ev) override;

  /**
   * @brief Gets the minimum value on the nodes.
   * It is only computed if it has never been retrieved before, or if the cached value could not be
   *updated.
   *
   * @param graph The graph on which to compute.
   * @return The minimal value on this graph for this property.
   **/
  TYPE_CONST_REFERENCE(NodeType)
  getNodeMin(const Graph *graph = nullptr);

  /**
   * @brief Computes the maximum value on the nodes.
   * It is only computed if it has never been retrieved before, or if the cached value could not be
   *updated.
   *
   * @param graph The graph on which to compute.
   * @return The maximal value on this graph for this property.
   **/
  TYPE_CONST_REFERENCE(NodeType)
  getNodeMax(const Graph *graph = nullptr);

  /**
   * @brief Computes the minimum value on the edges.
   * It is only computed if it has never been retrieved before, or if the cached value could not be
   *updated.
   *
   * @param graph The graph on which to compute.
   * @return The minimal value on this graph for this property.
   **/
  TYPE_CONST_REFERENCE(EdgeType)
  getEdgeMin(const Graph *graph = nullptr);

  /**
   * @brief Computes the maximum value on the edges.
   * It is only computed if it has never been retrieved before, or if the cached value could not be
   *updated.
   *
   * @param graph The graph on which to compute.
   * @return The maximal value on this graph for this property.
   **/
  TYPE_CONST_REFERENCE(EdgeType)
  getEdgeMax(const Graph *graph = nullptr);

  /**
   * @brief Updates the value on a node, and updates the minimal/maximal cached values if necessary.
   * Should be called by subclasses in order to keep the cache up to date.
   *
   * @param n The node for which the value is updated.
   * @param newValue The new value on this node.
   **/
  void updateNodeValue(tlp::node n, TYPE_CONST_REFERENCE(NodeType) newValue);

  /**
   * @brief Updates the value on an edge, and updates the minimal/maximal cached values if
   *necessary.
   * Should be called by subclasses in order to keep the cache up to date.
   *
   * @param e The edge for which the value is updated.
   * @param newValue The new value on this edge.
   **/
  virtual void updateEdgeValue(tlp::edge e, TYPE_CONST_REFERENCE(EdgeType) newValue);

  /**
   * @brief Updates the value of all nodes, setting the maximum and minimum values to this.
   * Should be called by subclasses in order to keep the cache up to date.
   *
   * @param newValue The new maximal and minimal value.
   **/
  void updateAllNodesValues(TYPE_CONST_REFERENCE(NodeType) newValue);

  /**
   * @brief Updates the value of all edges, setting the maximum and minimum values to this.
   * Should be called by subclasses in order to keep the cache up to date.
   *
   * @param newValue The new maximal and minimal value.
   **/
  void updateAllEdgesValues(TYPE_CONST_REFERENCE(EdgeType) newValue);

protected:
  MINMAX_MAP(NodeType) _minMaxNode;
  MINMAX_MAP(EdgeType) _minMaxEdge;

  REAL_TYPE(NodeType) _nodeMin;
  REAL_TYPE(NodeType) _nodeMax;
  REAL_TYPE(EdgeType) _edgeMin;
  REAL_TYPE(EdgeType) _edgeMax;

  // this will indicate if we can stop PropType::graph observation
  bool _needGraphListener; // default is false

  virtual MINMAX_PAIR(NodeType) computeMinMaxNode(const Graph *graph);
  MINMAX_PAIR(EdgeType) computeMinMaxEdge(const Graph *graph);
  void removeListenersAndClearNodeMap();
  void removeListenersAndClearEdgeMap();
};
}

#ifdef DLL_TALIPOT
#include "cxx/MinMaxProperty.cxx"
#endif

#endif // TALIPOT_MIN_MAX_PROPERTY_H
