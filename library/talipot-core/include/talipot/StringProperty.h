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

#ifndef TALIPOT_STRING_PROPERTY_H
#define TALIPOT_STRING_PROPERTY_H

#include <talipot/PropertyTypes.h>
#include <talipot/AbstractProperty.h>
#include <talipot/TlpTools.h>

namespace tlp {

class PropertyContext;

typedef AbstractProperty<StringType, StringType> AbstractStringProperty;
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractProperty<StringType, StringType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::string value to graph elements.
 */
class TLP_SCOPE StringProperty : public AbstractStringProperty {
  public:
    StringProperty(Graph *g, const std::string &n = "");

    // redefinition of some PropertyInterface methods
    PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
    static const std::string propertyTypename;
    const std::string &getTypename() const override {
        return propertyTypename;
    }
    // the value size of a StringVectorProperty is not fixed
    uint nodeValueSize() const override {
        return 0;
    }
    uint edgeValueSize() const override {
        return 0;
    }
    int compare(const node n1, const node n2) const override;
    int compare(const edge e1, const edge e2) const override;
};

DECLARE_DLL_TEMPLATE_INSTANCE(
    SINGLE_ARG(AbstractProperty<StringVectorType, StringVectorType, VectorPropertyInterface>),
    TLP_TEMPLATE_DECLARE_SCOPE)
DECLARE_DLL_TEMPLATE_INSTANCE(SINGLE_ARG(AbstractVectorProperty<StringVectorType, StringType>),
                              TLP_TEMPLATE_DECLARE_SCOPE)

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<std::string> value to graph elements.
 */
class TLP_SCOPE StringVectorProperty : public AbstractVectorProperty<StringVectorType, StringType> {
  public:
    StringVectorProperty(Graph *g, const std::string &n = "")
        : AbstractVectorProperty<StringVectorType, StringType>(g, n) {}

    // redefinition of some PropertyInterface methods
    PropertyInterface *clonePrototype(Graph *, const std::string &) const override;
    static const std::string propertyTypename;
    const std::string &getTypename() const override {
        return propertyTypename;
    }
    // the value size of a StringVectorProperty is not fixed
    uint nodeValueSize() const override {
        return 0;
    }
    uint edgeValueSize() const override {
        return 0;
    }
};
}
#endif // TALIPOT_STRING_PROPERTY_H
