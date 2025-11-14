/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_INTEGER_PROPERTY_H
#define TALIPOT_INTEGER_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/MinMaxProperty.h>
#include <talipot/NumericProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class Graph;
class PropertyContext;

typedef MinMaxProperty<IntegerType, IntegerType, NumericProperty> IntegerMinMaxProperty;

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<IntegerType, IntegerType, NumericProperty>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MinMaxProperty<IntegerType, IntegerType, NumericProperty>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps an integer value to graph elements.
 */
class TLP_SCOPE IntegerProperty : public IntegerMinMaxProperty {

public:
  IntegerProperty(Graph *, const std::string &n = "");

  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
  void setNodeValue(const node n, StoredType<int>::ConstReference v) override;

  // inner class used to extend the overloading of the operator[]

  // to set a node value

  class NodeValueProxy : public AbstractProperty<tlp::IntegerType, tlp::IntegerType,
                                                 tlp::NumericProperty>::NodeValueProxy {
  public:
    constexpr NodeValueProxy(IntegerProperty *prop, node n)
        : AbstractProperty<tlp::IntegerType, tlp::IntegerType,
                           tlp::NumericProperty>::NodeValueProxy(prop, n) {}

    NodeValueProxy &operator=(StoredType<int>::ConstReference val) {
      _prop->setNodeValue(_n, val);
      return *this;
    }

    // prefix increment
    NodeValueProxy &operator++() {
      _prop->setNodeValue(_n, getValue() + 1);
      return *this;
    }

    // postfix increment
    auto operator++(int) {
      auto val = getValue();
      _prop->setNodeValue(_n, val + 1);
      return val;
    }

    // increment and assign
    NodeValueProxy &operator+=(int val) {
      _prop->setNodeValue(_n, getValue() + val);
      return *this;
    }

    // prefix decrement
    NodeValueProxy &operator--() {
      _prop->setNodeValue(_n, getValue() - 1);
      return *this;
    }

    // postfix decrement
    auto operator--(int) {
      auto val = getValue();
      _prop->setNodeValue(_n, val - 1);
      return val;
    }

    // decrement and assign
    NodeValueProxy &operator-=(int val) {
      _prop->setNodeValue(_n, getValue() - val);
      return *this;
    }
  };

  // overload operator[] to set a node value
  constexpr NodeValueProxy operator[](node n) {
    return NodeValueProxy(this, n);
  }

  void setEdgeValue(const edge e, StoredType<int>::ConstReference v) override;

  // inner class used to extend the overloading of the operator[]
  // to set an edge value
  class EdgeValueProxy : public AbstractProperty<tlp::IntegerType, tlp::IntegerType,
                                                 tlp::NumericProperty>::EdgeValueProxy {

  public:
    constexpr EdgeValueProxy(IntegerProperty *prop, edge e)
        : AbstractProperty<tlp::IntegerType, tlp::IntegerType,
                           tlp::NumericProperty>::EdgeValueProxy(prop, e) {}

    EdgeValueProxy &operator=(StoredType<int>::ConstReference val) {
      _prop->setEdgeValue(_e, val);
      return *this;
    }

    // prefix increment
    EdgeValueProxy &operator++() {
      _prop->setEdgeValue(_e, getValue() + 1);
      return *this;
    }

    // postfix increment
    auto operator++(int) {
      auto val = getValue();
      _prop->setEdgeValue(_e, val + 1);
      return val;
    }

    // increase value
    EdgeValueProxy &operator+=(int val) {
      _prop->setEdgeValue(_e, getValue() + val);
      return *this;
    }

    // prefix decrement
    EdgeValueProxy &operator--() {
      _prop->setEdgeValue(_e, getValue() - 1);
      return *this;
    }

    // postfix decrement
    auto operator--(int) {
      auto val = getValue();
      _prop->setEdgeValue(_e, val - 1);
      return val;
    }

    // decrease value
    EdgeValueProxy &operator-=(int val) {
      _prop->setEdgeValue(_e, getValue() - val);
      return *this;
    }
  };

  // overload operator[] to set an edge value
  constexpr EdgeValueProxy operator[](edge e) {
    return EdgeValueProxy(this, e);
  }

  void setAllNodeValue(StoredType<int>::ConstReference v, const Graph *graph = nullptr) override;
  void setAllEdgeValue(StoredType<int>::ConstReference v, const Graph *graph = nullptr) override;

  int compare(const node n1, const node n2) const override;
  int compare(const edge e1, const edge e2) const override;

  // NumericProperty interface
  double getNodeDoubleValue(const node n) const override {
    return getNodeValue(n);
  }
  double getNodeDoubleDefaultValue() const override {
    return getNodeDefaultValue();
  }
  double getNodeDoubleMin(const Graph *g = nullptr) override {
    return getNodeMin(g);
  }
  double getNodeDoubleMax(const Graph *g = nullptr) override {
    return getNodeMax(g);
  }
  double getEdgeDoubleValue(const edge e) const override {
    return getEdgeValue(e);
  }
  double getEdgeDoubleDefaultValue() const override {
    return getEdgeDefaultValue();
  }
  double getEdgeDoubleMin(const Graph *g = nullptr) override {
    return getEdgeMin(g);
  }
  double getEdgeDoubleMax(const Graph *g = nullptr) override {
    return getEdgeMax(g);
  }

  void nodesUniformQuantification(uint) override;

  void edgesUniformQuantification(uint) override;

  NumericProperty *copyProperty(Graph *g) override {
    auto *newProp = new IntegerProperty(g);
    newProp->copy(this);

    return newProp;
  }

protected:
  void clone_handler(const AbstractProperty<IntegerType, IntegerType, NumericProperty> &) override;

private:
  // override Observable::treatEvent
  void treatEvent(const Event &) override;
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<IntegerVectorType, IntegerVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<IntegerVectorType, IntegerType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<int> value to graph elements.
 */
class TLP_SCOPE IntegerVectorProperty
    : public AbstractVectorProperty<IntegerVectorType, IntegerType> {
public:
  IntegerVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<IntegerVectorType, IntegerType>(g, n) {}
  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}
#endif // TALIPOT_INTEGER_PROPERTY_H
