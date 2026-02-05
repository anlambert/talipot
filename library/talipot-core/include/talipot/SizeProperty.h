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

#ifndef TALIPOT_SIZE_PROPERTY_H
#define TALIPOT_SIZE_PROPERTY_H

#include <talipot/hash.h>

#include <talipot/Coord.h>
#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

typedef AbstractProperty<SizeType, SizeType> AbstractSizeProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<SizeType, SizeType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a Size value to graph elements.
 */
class TLP_SCOPE SizeProperty : public AbstractSizeProperty {

public:
  // inner class used to extend the overloading of the operator[]
  // to set a node value
  class NodeValueProxy : public AbstractProperty<SizeType, SizeType>::NodeValueProxy {
  public:
    constexpr NodeValueProxy(const SizeProperty *prop, node n)
        : AbstractProperty<SizeType, SizeType>::NodeValueProxy(prop, n) {}

    NodeValueProxy &operator=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setNodeValue(_n, val);
      return *this;
    }

    REAL_TYPE(SizeType) operator+(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() + val;
    }

    REAL_TYPE(SizeType) operator+(float val) const {
      return getValue() + val;
    }

    NodeValueProxy &operator+=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setNodeValue(_n, getValue() + val);
      return *this;
    }

    NodeValueProxy &operator+=(float val) {
      getProperty()->setNodeValue(_n, getValue() + val);
      return *this;
    }

    REAL_TYPE(SizeType) operator*(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() * val;
    }

    REAL_TYPE(SizeType) operator*(float val) const {
      return getValue() * val;
    }

    NodeValueProxy &operator*=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setNodeValue(_n, getValue() * val);
      return *this;
    }

    NodeValueProxy &operator*=(float val) {
      getProperty()->setNodeValue(_n, getValue() * val);
      return *this;
    }

    REAL_TYPE(SizeType) operator/(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() / val;
    }

    REAL_TYPE(SizeType) operator/(float val) const {
      return getValue() / val;
    }

    NodeValueProxy &operator/=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setNodeValue(_n, getValue() / val);
      return *this;
    }

    NodeValueProxy &operator/=(float val) {
      getProperty()->setNodeValue(_n, getValue() / val);
      return *this;
    }

    REAL_TYPE(SizeType) operator-(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() - val;
    }

    REAL_TYPE(SizeType) operator-(float val) const {
      return getValue() - val;
    }

    NodeValueProxy &operator-=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setNodeValue(_n, getValue() - val);
      return *this;
    }

    NodeValueProxy &operator-=(float val) {
      getProperty()->setNodeValue(_n, getValue() - val);
      return *this;
    }

    float operator[](int i) const {
      return getValue()[i];
    }

    operator Coord() const {
      return getValue();
    }
  };

  // overload operator[] to set a node value
  constexpr NodeValueProxy operator[](node n) const {
    return NodeValueProxy(this, n);
  }

  // inner class used to extend the overloading of the operator[]
  // to set an edge value
  class EdgeValueProxy : public AbstractProperty<SizeType, SizeType>::EdgeValueProxy {
  public:
    constexpr EdgeValueProxy(const SizeProperty *prop, edge e)
        : AbstractProperty<SizeType, SizeType>::EdgeValueProxy(prop, e) {}

    EdgeValueProxy &operator=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setEdgeValue(_e, val);
      return *this;
    }

    REAL_TYPE(SizeType) operator+(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() + val;
    }

    REAL_TYPE(SizeType) operator+(float val) const {
      return getValue() + val;
    }

    EdgeValueProxy &operator+=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setEdgeValue(_e, getValue() + val);
      return *this;
    }

    EdgeValueProxy &operator+=(float val) {
      getProperty()->setEdgeValue(_e, getValue() + val);
      return *this;
    }

    REAL_TYPE(SizeType) operator*(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() * val;
    }

    REAL_TYPE(SizeType) operator*(float val) const {
      return getValue() * val;
    }

    EdgeValueProxy &operator*=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setEdgeValue(_e, getValue() * val);
      return *this;
    }

    EdgeValueProxy &operator*=(float val) {
      getProperty()->setEdgeValue(_e, getValue() * val);
      return *this;
    }

    REAL_TYPE(SizeType) operator/(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() / val;
    }

    REAL_TYPE(SizeType) operator/(float val) const {
      return getValue() / val;
    }

    EdgeValueProxy &operator/=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setEdgeValue(_e, getValue() / val);
      return *this;
    }

    EdgeValueProxy &operator/=(float val) {
      getProperty()->setEdgeValue(_e, getValue() / val);
      return *this;
    }

    REAL_TYPE(SizeType) operator-(TYPE_CONST_REFERENCE(SizeType) val) const {
      return getValue() - val;
    }

    REAL_TYPE(SizeType) operator-(float val) const {
      return getValue() - val;
    }

    EdgeValueProxy &operator-=(TYPE_CONST_REFERENCE(SizeType) val) {
      getProperty()->setEdgeValue(_e, getValue() - val);
      return *this;
    }

    EdgeValueProxy &operator-=(float val) {
      getProperty()->setEdgeValue(_e, getValue() - val);
      return *this;
    }

    float operator[](int i) const {
      return getValue()[i];
    }
  };

  // overload operator[] to set an edge value
  constexpr EdgeValueProxy operator[](edge e) const {
    return EdgeValueProxy(this, e);
  }

  SizeProperty(Graph *, const std::string &n = "");

  Size getMax(const Graph *sg = nullptr);
  Size getMin(const Graph *sg = nullptr);
  void scale(const Vec3f &, const Graph *sg = nullptr);
  void scale(const Vec3f &, Iterator<node> *, Iterator<edge> *);

  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  // redefinition of some AbstractProperty methods
  void setNodeValue(const node n, StoredType<Size>::ConstReference v) override;
  void setAllNodeValue(StoredType<Size>::ConstReference v, const Graph *graph = nullptr) override;

  int compare(const node n1, const node n2) const override;

protected:
  void resetMinMax();

private:
  flat_hash_map<uint, Size> max, min;
  flat_hash_map<uint, bool> minMaxOk;
  void computeMinMax(const Graph *sg = nullptr);
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<SizeVectorType, SizeVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<SizeVectorType, SizeType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<Size> value to graph elements.
 */
class TLP_SCOPE SizeVectorProperty : public AbstractVectorProperty<SizeVectorType, SizeType> {
public:
  SizeVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<SizeVectorType, SizeType>(g, n) {}

  // redefinition of some PropertyInterface methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}
#endif // TALIPOT_SIZE_PROPERTY_H
