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

#ifndef TALIPOT_ABSTRACT_PROPERTY_H
#define TALIPOT_ABSTRACT_PROPERTY_H

#include <typeinfo>
#include <string>
#include <cstdlib>
#include <talipot/config.h>
#include <talipot/StoredType.h>
#include <talipot/MutableContainer.h>
#include <talipot/PropertyInterface.h>
#include <talipot/Iterator.h>
#include <talipot/DataSet.h>
#include <talipot/Graph.h>
#include <talipot/TypeInterface.h>

namespace tlp {

class GraphView;

template <class NodeType, class EdgeType, class PropType>
class PropertyNodeValueWrapper;

template <class NodeType, class EdgeType, class PropType>
class PropertyEdgeValueWrapper;

//==============================================================

/**
 * @ingroup Graph
 * @brief This class extends upon PropertyInterface, and adds type-safe methods to
 * get and set the node and edge values, through the magic of template programming.
 *
 * Nodes and Edges can have different types (e.g. tlp::LayoutProperty has tlp::PointType as node
 * type and tlp::LineType as edge type),
 * but most of the time they have the same type (e.g. tlp::DoubleProperty, tlp::IntegerProperty).
 *
 * Some of the pure virtual functions of PropertyInterface are implemented in this class (e.g.
 * erase()).
 *
 * The actual data is stored in this class, and it manages the default values.
 */
template <class NodeType, class EdgeType, class PropType = PropertyInterface>
class AbstractProperty : public PropType {
  friend class Graph;
  friend class GraphView;

public:
  AbstractProperty(Graph *, const std::string &n = "");

  AbstractProperty(const AbstractProperty<NodeType, EdgeType, PropType> &);

  /**
   * @brief Gets the default node value of the property.
   * @return The default value of nodes.
   */
  REAL_TYPE(NodeType) getNodeDefaultValue() const;

  /**
   * @brief Gets the default edge value of the property.
   * @return The default value of edges.
   **/
  REAL_TYPE(EdgeType) getEdgeDefaultValue() const;

  /**
   * @brief Returns the value associated with the node n in this property.
   * If there is no value, it returns the default node value.
   *
   * @param n The node for which we want to get the value of the property.
   * @return :StoredType< NodeType::RealType >::ConstReference The value of the property for
   *this node.
   **/
  TYPE_CONST_REFERENCE(NodeType)
  getNodeValue(const node n) const;

  /**
   * @brief Returns the value associated to the edge e in this property.
   * If there is no value, it returns the default edge value.
   *
   * @param e The edge for which we want to get the value of the property.
   * @return :StoredType< EdgeType::RealType >::ConstReference The value of the property for
   *this edge.
   **/
  TYPE_CONST_REFERENCE(EdgeType)
  getEdgeValue(const edge e) const;

  /**
   * Returns an iterator through all nodes belonging to g
   * whose associated value is equal to val.
   * If g is nullptr, the graph given when creating the property is considered.
   */
  virtual tlp::Iterator<node> *getNodesEqualTo(TYPE_CONST_REFERENCE(NodeType) v,
                                               const Graph *g = nullptr) const;

  /**
   * Returns an iterator through all edges belonging to g
   * whose associated value is equal to val.
   * If g is nullptr, the graph given when creating the property is considered.
   */
  virtual tlp::Iterator<edge> *getEdgesEqualTo(TYPE_CONST_REFERENCE(EdgeType) v,
                                               const Graph *g = nullptr) const;

  /**
   * @brief Sets the value of a node and notify the observers of a modification.
   *
   * @param n The node to set the value of.
   * @param v The value to affect for this node.
   **/
  virtual void setNodeValue(const node n, TYPE_CONST_REFERENCE(NodeType) v);

  /**
   * @brief Set the value of an edge and notify the observers of a modification.
   *
   * @param e The edge to set the value of.
   * @param v The value to affect for this edge.
   **/
  virtual void setEdgeValue(const edge e, TYPE_CONST_REFERENCE(EdgeType) v);

  /**
   * @brief Sets the value of all nodes and notify the observers.
   *
   * All previous values are lost and the given value is assigned
   * as the default one to the future added nodes.
   *
   * @param v The value to set to all nodes.
   * @param graph An optional descendant graph from the one associated
   * to that property. If provided, only the nodes from
   * that graph will have their value modified and the default node value
   * will not be modified.
   *
   * @warning If the provided graph is not a descendant of the one associated
   * to that property, no node value will be modified in it.
   *
   */
  virtual void setAllNodeValue(TYPE_CONST_REFERENCE(NodeType) v, const Graph *graph = nullptr);

  /**
   * @brief Sets the value assigned as the default one to the future added nodes.
   *
   * @param v the new value to set on future added nodes.
   *
   * @return Whether the given string was a correct representation for this property's type. If not,
   * the default value is not set.
   */
  virtual void setNodeDefaultValue(TYPE_CONST_REFERENCE(NodeType) v);

  /**
   * @brief Sets the value assigned as the default one to the future added edges.
   *
   * @param value the new value to set on future added edges.
   *
   * @return Whether the given string was a correct representation for this property's type. If not,
   * the default value is not set.
   */
  virtual void setEdgeDefaultValue(TYPE_CONST_REFERENCE(EdgeType) v);

  /**
   * @brief Sets the value of all edges and notify the observers.
   *
   * All previous values are lost and the given value is assigned
   * as the default one to the future added edges.
   *
   * @param v The value to set to all edges.
   * @param graph An optional descendant graph from the one associated
   * to that property. If provided, only the edges
   * from that graph will have their value modified and the default edge
   * value will not be modified.
   *
   * @warning If the provided graph is not a descendant of the one associated
   * to that property, no edge value will be modified in it.
   *
   */
  virtual void setAllEdgeValue(TYPE_CONST_REFERENCE(EdgeType) v, const Graph *graph = nullptr);

  //=================================================================================

  /**
   * @brief Resets the value of a node to the default value.
   *
   * @param n The node to reset the value of.
   *
   **/
  void erase(const node n) override;
  //=================================================================================

  /**
   * @brief Resets the value of an edge to the default value.
   *
   * @param e The edge to reset the value of.
   *
   **/
  void erase(const edge e) override;
  //=================================================================================
  /**
   * @brief This operator overload allows to copy a property using the following syntax :
   *
   * @code
   * IntegerProperty* shape = graph->getIntegerProperty("viewShape");
   * IntegerProperty* backup = graph->getIntegerProperty("shapeBackup");
   * *backup = *shape; // all the values from 'shape' will be copied into 'backup'.
   * @endcode
   * @param prop The property to copy the values from.
   * @return This property with the values copied.
   */
  AbstractProperty<NodeType, EdgeType, PropType> &
  operator=(const AbstractProperty<NodeType, EdgeType, PropType> &prop);
  //=================================================================================
  // Untyped accessors inherited from PropertyInterface, documentation is inherited
  std::string getNodeDefaultStringValue() const override;
  std::string getEdgeDefaultStringValue() const override;
  std::string getNodeStringValue(const node n) const override;
  std::string getEdgeStringValue(const edge e) const override;
  bool setNodeStringValue(const node inN, const std::string &inV) override;
  bool setEdgeStringValue(const edge inE, const std::string &inV) override;
  bool setNodeDefaultStringValue(const std::string &inV) override;
  bool setAllNodeStringValue(const std::string &inV, const Graph *graph = nullptr) override;
  bool setEdgeDefaultStringValue(const std::string &inV) override;
  bool setAllEdgeStringValue(const std::string &inV, const Graph *graph = nullptr) override;

  tlp::Iterator<node> *getNonDefaultValuatedNodes(const Graph *g = nullptr) const override;
  bool hasNonDefaultValuatedNodes(const Graph *g = nullptr) const override;
  uint numberOfNonDefaultValuatedNodes(const Graph *g = nullptr) const override;
  uint nodeValueSize() const override;
  void writeNodeDefaultValue(std::ostream &) const override;
  void writeNodeValue(std::ostream &, node) const override;
  bool readNodeDefaultValue(std::istream &) override;
  bool readNodeValue(std::istream &, node)
  override;
  tlp::Iterator<edge> *getNonDefaultValuatedEdges(const Graph *g = nullptr) const override;
  bool hasNonDefaultValuatedEdges(const Graph *g = nullptr) const override;
  uint numberOfNonDefaultValuatedEdges(const Graph * = nullptr) const override;
  uint edgeValueSize() const override;
  void writeEdgeDefaultValue(std::ostream &) const override;
  void writeEdgeValue(std::ostream &, edge) const override;
  bool readEdgeDefaultValue(std::istream &) override;
  bool readEdgeValue(std::istream &, edge)
  override;
  bool copy(const node destination, const node source, PropertyInterface *property,
            bool ifNotDefault = false) override;
  bool copy(const edge destination, const edge source, PropertyInterface *property,
            bool ifNotDefault = false) override;
  void copy(PropertyInterface *property) override;
  // for performance reason and use in GraphUpdatesRecorder
  DataMem *getNodeDefaultDataMemValue() const override;
  DataMem *getEdgeDefaultDataMemValue() const override;
  DataMem *getNodeDataMemValue(const node n) const override;
  DataMem *getEdgeDataMemValue(const edge e) const override;
  DataMem *getNonDefaultDataMemValue(const node n) const override;
  DataMem *getNonDefaultDataMemValue(const edge e) const override;
  void setNodeDataMemValue(const node n, const DataMem *v) override;
  void setEdgeDataMemValue(const edge e, const DataMem *v) override;
  void setAllNodeDataMemValue(const DataMem *v) override;
  void setAllEdgeDataMemValue(const DataMem *v) override;

  // PropertyInterface methods
  // mN is the meta node, sg is the corresponding subgraph
  // and mg is the graph owning mN
  void computeMetaValue(node n, Graph *sg, Graph *mg) override;

  // mE is the meta edge, itE is an iterator on the underlying edges
  // mg is the graph owning mE
  void computeMetaValue(edge e, tlp::Iterator<edge> *itE, Graph *mg) override;

  void setMetaValueCalculator(PropertyInterface::MetaValueCalculator *mvCalc) override;

  int compare(const node n1, const node n2) const override;
  int compare(const edge e1, const edge e2) const override;

  /**
   * @brief This class is used to delegate the computation of the values associated to meta nodes or
   *edges.
   **/
  class TLP_SCOPE MetaValueCalculator : public PropertyInterface::MetaValueCalculator {
  public:
    // computes the value of the meta node mN of the graph mg
    // for the property prop, according to the values associated
    // to the underlying nodes i.e the nodes of the subgraph sg.
    virtual void computeMetaValue(AbstractProperty<NodeType, EdgeType, PropType> *, node, Graph *,
                                  Graph *);
    // computes the value of the meta node mE of the graph mg
    // for the property prop, according to the values associated
    // to the underlying edges given by the iterator itE.
    // The method do not have to delete the iterator
    virtual void computeMetaValue(AbstractProperty<NodeType, EdgeType, PropType> *, edge,
                                  tlp::Iterator<edge> *itE, Graph *);
  };

  TYPE_CONST_REFERENCE(NodeType) operator[](node n) const {
    return getNodeValue(n);
  }

  PropertyNodeValueWrapper<NodeType, EdgeType, PropType> operator[](node n) {
    return PropertyNodeValueWrapper(this, n);
  }

  TYPE_CONST_REFERENCE(EdgeType) operator[](edge e) const {
    return getEdgeValue(e);
  }

  PropertyEdgeValueWrapper<NodeType, EdgeType, PropType> operator[](edge e) {
    return PropertyEdgeValueWrapper(this, e);
  }

protected:
  //=================================================================================
  /// Enable to clone part of sub_class
  virtual void clone_handler(const AbstractProperty<NodeType, EdgeType, PropType> &);

  MutableContainer<REAL_TYPE(NodeType), node> nodeProperties;
  MutableContainer<REAL_TYPE(EdgeType), edge> edgeProperties;
  REAL_TYPE(NodeType) nodeDefaultValue;
  REAL_TYPE(EdgeType) edgeDefaultValue;
};

template <typename VecType, typename EltType, typename PropType = VectorPropertyInterface>
class AbstractVectorProperty : public AbstractProperty<VecType, VecType, PropType> {
public:
  AbstractVectorProperty(Graph *, const std::string &name = "");

  // 5 methods inherited from VectorPropertyInterface
  bool tokenize(const std::string &str, std::vector<std::string> &vect, char openChar = '(',
                char sepChar = ',', char closeChar = ')') override;

  bool setNodeStringValueAsVector(const node n, const std::vector<std::string> &values) override;

  bool setNodeStringValueAsVector(const node, const std::string &, char, char, char) override;

  bool setEdgeStringValueAsVector(const edge e, const std::vector<std::string> &values) override;

  bool setEdgeStringValueAsVector(const edge, const std::string &, char, char, char) override;

  /**
   * @brief Sets the value for node n, at index i, to v, and notify the observers of a modification.
   *
   * @param n The node to set a value of.
   * @param i The index at which the value should be set.
   * @param v The value to set.
   *
   **/
  void setNodeEltValue(const node n, uint i, TYPE_CONST_REFERENCE(EltType) v);
  /**
   * @brief Gets the value associated to node n, at index i.
   *
   * @param n The node to set a value of.
   * @param i The index at which to set the value.
   * @return const EltType& The value at index i in the vector for node n.
   **/
  TYPE_CONST_REFERENCE(EltType)
  getNodeEltValue(const node n, uint i) const;
  /**
   * @brief Appends a new value at the end of the vector associated to node n, and notify the
   *observers of a modification.
   *
   * @param n The node to add a value to.
   * @param v The value to append at the end of the vector.
   *
   **/
  void pushBackNodeEltValue(const node n, TYPE_CONST_REFERENCE(EltType) v);
  /**
   * @brief Removes the value at the end of the vector associated to node n, and notify the
   *observers of a modification.
   *
   * @param n The node to remove a value from.
   *
   **/
  void popBackNodeEltValue(const node n);
  /**
   * @brief Resizes the vector associated to node n, and notify the observers of a modification.
   *
   * @param n The node associated to the vector to resize.
   * @param size The new size of the vector.
   * @param elt The default value to set at indices where there was no value before. Defaults to
   *EltType().
   *
   **/
  void resizeNodeValue(const node n, size_t size, REAL_TYPE(EltType) elt = EltType::defaultValue());
  /**
   * @brief Sets the value for edge e, at index i, to v, and notify the observers of a modification.
   *
   * @param e The edge to set the value of.
   * @param i The index at which the value should be set.
   * @param v The value to set.
   *
   **/
  void setEdgeEltValue(const edge e, uint i, TYPE_CONST_REFERENCE(EltType) v);
  /**
   * @brief Gets the value associated to edge e, at index i.
   *
   * @param e The edge to set a value of.
   * @param i The index at which to set the value.
   * @return const EltType& The value at index i in the vector for node n.
   **/
  TYPE_CONST_REFERENCE(EltType)
  getEdgeEltValue(const edge n, uint i) const;
  /**
   * @brief Appends a new value at the end of the vector associated to edge e, and notify the
   *observers of a modification.
   *
   * @param e The node to add a value to.
   * @param v The value to append at the end of the vector.
   *
   **/
  void pushBackEdgeEltValue(const edge e, TYPE_CONST_REFERENCE(EltType) v);
  /**
   * @brief Removes the value at the end of the vector associated to edge e, and notify the
   *observers of a modification.
   *
   * @param e The edge to remove a value from.
   *
   **/
  void popBackEdgeEltValue(const edge e);
  /**
   * @brief Resizes the vector associated to edge e, and notify the observers of a modification.
   *
   * @param e The edge associated to the vector to resize.
   * @param size The new size of the vector.
   * @param elt The default value to set at indices where there was no value before. Defaults to
   *EltType().
   *
   **/
  void resizeEdgeValue(const edge e, size_t size, REAL_TYPE(EltType) elt = EltType::defaultValue());
};

}

#include "PropertyValueWrappers.h"

#ifdef DLL_TALIPOT
#include "cxx/AbstractProperty.cxx"
#endif

#endif // TALIPOT_ABSTRACT_PROPERTY_H
