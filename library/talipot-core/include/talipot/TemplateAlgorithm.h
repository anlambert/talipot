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

#ifndef TALIPOT_TEMPLATE_ALGORITHM_H
#define TALIPOT_TEMPLATE_ALGORITHM_H

#include <talipot/Algorithm.h>
#include <talipot/Graph.h>

#include <sstream>

namespace tlp {
class PluginContext;
static const std::string PROPERTY_ALGORITHM_CATEGORY = "Property";

/**
 * @ingroup Plugins
 * @brief A non-template interface for tlp::TemplateAlgorithm
 * @see tlp::TemplateAlgorithm
 **/
class TLP_SCOPE PropertyAlgorithm : public tlp::Algorithm {
  public:
    PropertyAlgorithm(const tlp::PluginContext *context) : Algorithm(context) {}
    std::string category() const override {
        return PROPERTY_ALGORITHM_CATEGORY;
    }
};

/**
 * @ingroup Plugins
 * @brief The TemplateAlgorithm class describes a plugin that can operate on a single graph's
 * property.
 * @param Property The property template arguments gives the type of the property the algorithm
 * operates on.
 *
 * A TemplateAlgorithm takes a graph as input (plus additional parameters defined via
 * tlp::WithParameter) and outputs its results in a tlp::PropertyInterface subclass.
 * The output property is defined as an output parameter named "result" and as a class member called
 * result.
 *
 * @warning Subclassing TemplateAlgorithm is not recommended since template specifications are
 * available for every Talipot property types.
 *
 * @see tlp::BooleanAlgorithm
 * @see tlp::StringAlgorithm
 * @see tlp::DoubleAlgorithm
 * @see tlp::IntegerAlgorithm
 * @see tlp::LayoutAlgorithm
 * @see tlp::SizeAlgorithm
 */
template <class Property>
class TLP_SCOPE TemplateAlgorithm : public PropertyAlgorithm {
  public:
    Property *result;
    TemplateAlgorithm(const tlp::PluginContext *context)
        : tlp::PropertyAlgorithm(context), result(nullptr) {
        if (dataSet != nullptr) {
            if (!dataSet->exists("result")) {
                std::stringstream propname;
                propname << "result";
                unsigned number = 0;

                while (graph->existProperty(propname.str())) {
                    propname.clear();
                    propname << "result" << number;
                    ++number;
                }

                result = graph->getProperty<Property>(propname.str());
            } else {
                dataSet->get("result", result);
            }
        }
    }
};
}
#endif // TALIPOT_TEMPLATE_ALGORITHM_H
