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

#ifndef TALIPOT_DOUBLE_PROPERTY_H
#define TALIPOT_DOUBLE_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/MinMaxProperty.h>
#include <talipot/NumericProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

typedef MinMaxProperty<DoubleType, DoubleType, NumericProperty> DoubleMinMaxProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<DoubleType, DoubleType, NumericProperty>),
                              TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(MinMaxProperty<DoubleType, DoubleType, NumericProperty>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a double value to graph elements.
 */
class TLP_SCOPE DoubleProperty : public DoubleMinMaxProperty {
public:
  DoubleProperty(Graph *, const std::string &n = "");

  void clone_handler(const AbstractProperty<DoubleType, DoubleType, NumericProperty> &) override;

  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  void setNodeValue(const node n, StoredType<double>::ConstReference v) override;
  void setEdgeValue(const edge e, StoredType<double>::ConstReference v) override;
  void setAllNodeValue(StoredType<double>::ConstReference v, const Graph *graph = nullptr) override;
  void setAllEdgeValue(StoredType<double>::ConstReference v, const Graph *graph = nullptr) override;

  enum PredefinedMetaValueCalculator {
    NO_CALC = 0,
    AVG_CALC = 1,
    SUM_CALC = 2,
    MAX_CALC = 3,
    MIN_CALC = 4
  };

  // setMetaValueCalculator overloading
  void setMetaValueCalculator(PropertyInterface::MetaValueCalculator *calc) override;
  void setMetaValueCalculator(PredefinedMetaValueCalculator nodeCalc = AVG_CALC,
                              PredefinedMetaValueCalculator edgeCalc = AVG_CALC);

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
    auto *newProp = new DoubleProperty(g);
    newProp->copy(this);

    return newProp;
  }

private:
  // override Observable::treatEvent
  void treatEvent(const Event &) override;
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<DoubleVectorType, DoubleVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<DoubleVectorType, DoubleType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<double> value to graph elements.
 */

class TLP_SCOPE DoubleVectorProperty : public AbstractVectorProperty<DoubleVectorType, DoubleType> {
public:
  DoubleVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<DoubleVectorType, DoubleType>(g, n) {}
  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}
#endif // TALIPOT_DOUBLE_PROPERTY_H
