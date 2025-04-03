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
