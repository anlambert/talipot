/**
 *
 * Copyright (C) 2019-2026  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_BOOLEAN_PROPERTY_H
#define TALIPOT_BOOLEAN_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<BooleanType, BooleanType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a boolean value to graph elements.
 */
class TLP_SCOPE BooleanProperty : public AbstractProperty<BooleanType, BooleanType> {
public:
  class NodeValueProxy : public AbstractProperty<BooleanType, BooleanType>::NodeValueProxy {
  public:
    constexpr NodeValueProxy(const BooleanProperty *prop, node n)
        : AbstractProperty<BooleanType, BooleanType>::NodeValueProxy(prop, n) {}

    NodeValueProxy &operator=(REAL_TYPE(BooleanType) val) {
      this->getProperty()->setNodeValue(_n, val);
      return *this;
    }

    constexpr bool operator==(REAL_TYPE(BooleanType) val) const {
      return this->getValue() == val;
    }

    constexpr bool operator==(const NodeValueProxy &ref) const {
      return this->getValue() == ref.getValue();
    }

    constexpr bool operator==(const EdgeValueProxy &ref) const {
      return this->getValue() == ref.getValue();
    }
  };

  // overload operator[] to set an edge value
  constexpr NodeValueProxy operator[](node n) const {
    return NodeValueProxy(this, n);
  }

  class EdgeValueProxy : public AbstractProperty<BooleanType, BooleanType>::EdgeValueProxy {
  public:
    constexpr EdgeValueProxy(const BooleanProperty *prop, edge e)
        : AbstractProperty<BooleanType, BooleanType>::EdgeValueProxy(prop, e) {}

    EdgeValueProxy &operator=(REAL_TYPE(BooleanType) val) {
      this->getProperty()->setEdgeValue(_e, val);
      return *this;
    }

    constexpr bool operator==(REAL_TYPE(BooleanType) val) const {
      return this->getValue() == val;
    }

    constexpr bool operator==(const EdgeValueProxy &ref) const {
      return this->getValue() == ref.getValue();
    }

    constexpr bool operator==(const NodeValueProxy &ref) const {
      return this->getValue() == ref.getValue();
    }
  };

  // overload operator[] to set an edge value
  constexpr EdgeValueProxy operator[](edge e) const {
    return EdgeValueProxy(this, e);
  }
  BooleanProperty(Graph *g, const std::string &n = "")
      : AbstractProperty<BooleanType, BooleanType>(g, n) {}
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  /**
   * Reverses all values associated to graph elements,
   * i.e true => false, false => true.
   * If sg is nullptr, the graph given when creating the property is considered.
   */
  void reverse(const Graph *sg = nullptr);

  /**
   * Reverses all the direction of edges of the visible graph
   * which are true in this BooleanProperty.
   * * If sg is nullptr, the graph given when creating the property is considered.
   */
  void reverseEdgeDirection(Graph *sg = nullptr);
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<BooleanVectorType, BooleanVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<BooleanVectorType, BooleanType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<bool> value to graph elements.
 */
class TLP_SCOPE BooleanVectorProperty
    : public AbstractVectorProperty<BooleanVectorType, BooleanType> {
public:
  BooleanVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<BooleanVectorType, BooleanType>(g, n) {}
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}

#endif // TALIPOT_BOOLEAN_PROPERTY_H
