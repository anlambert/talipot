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

#ifndef TALIPOT_COLOR_PROPERTY_H
#define TALIPOT_COLOR_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

typedef AbstractProperty<ColorType, ColorType> AbstractColorProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(tlp::AbstractProperty<tlp::ColorType, tlp::ColorType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a tlp::Color value to graph elements.
 */
class TLP_SCOPE ColorProperty : public AbstractColorProperty {
public:
  // inner class used to extend the overloading of the operator[]
  // to set an edge value
  class NodeValueProxy : public AbstractProperty<ColorType, ColorType>::NodeValueProxy {
  public:
    constexpr NodeValueProxy(const ColorProperty *prop, node n)
        : AbstractProperty<ColorType, ColorType>::NodeValueProxy(prop, n) {}

    NodeValueProxy &operator=(TYPE_CONST_REFERENCE(ColorType) val) {
      getProperty()->setNodeValue(_n, val);
      return *this;
    }

    unsigned char operator[](int i) const {
      return getValue()[i];
    }
  };

  // overload operator[] to set an edge value
  constexpr NodeValueProxy operator[](node n) const {
    return NodeValueProxy(this, n);
  }

  // inner class used to extend the overloading of the operator[]
  // to set an edge value
  class EdgeValueProxy : public AbstractProperty<ColorType, ColorType>::EdgeValueProxy {
  public:
    constexpr EdgeValueProxy(const ColorProperty *prop, edge e)
        : AbstractProperty<ColorType, ColorType>::EdgeValueProxy(prop, e) {}

    EdgeValueProxy &operator=(TYPE_CONST_REFERENCE(ColorType) val) {
      getProperty()->setEdgeValue(_e, val);
      return *this;
    }

    unsigned char operator[](int i) const {
      return getValue()[i];
    }
  };

  // overload operator[] to set an edge value
  constexpr EdgeValueProxy operator[](edge e) const {
    return EdgeValueProxy(this, e);
  }

  ColorProperty(Graph *g, const std::string &n = "");
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }

  int compare(const node n1, const node n2) const override;
  int compare(const edge e1, const edge e2) const override;
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<ColorVectorType, ColorVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<ColorVectorType, ColorType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<tlp::Color> value to graph elements.
 */
class TLP_SCOPE ColorVectorProperty : public AbstractVectorProperty<ColorVectorType, ColorType> {
public:
  ColorVectorProperty(Graph *g, const std::string &n = "")
      : AbstractVectorProperty<ColorVectorType, ColorType>(g, n) {}
  // PropertyInterface inherited methods
  PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
  static const std::string propertyTypename;
  const std::string &getTypename() const override {
    return propertyTypename;
  }
};
}
#endif // TALIPOT_COLOR_PROPERTY_H
