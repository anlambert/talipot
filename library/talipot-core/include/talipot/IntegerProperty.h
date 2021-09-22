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
  void setEdgeValue(const edge e, StoredType<int>::ConstReference v) override;
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

  void nodesUniformQuantification(uint)
  override;

  void edgesUniformQuantification(uint)
  override;

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
