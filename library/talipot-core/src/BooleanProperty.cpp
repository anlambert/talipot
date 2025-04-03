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

#include <talipot/BooleanProperty.h>

using namespace std;
using namespace tlp;

const string BooleanProperty::propertyTypename = "bool";
const string BooleanVectorProperty::propertyTypename = "vector<bool>";

//=================================================================================
void BooleanProperty::reverse(const Graph *sg) {
    if (sg == nullptr) {
        sg = graph;
    }

    for (auto n : sg->nodes()) {
        notifyBeforeSetNodeValue(n);
        nodeProperties.invertBooleanValue(n);
        notifyAfterSetNodeValue(n);
    }

    for (auto e : sg->edges()) {
        notifyBeforeSetEdgeValue(e);
        edgeProperties.invertBooleanValue(e);
        notifyAfterSetEdgeValue(e);
    }
}

//=================================================================================
void BooleanProperty::reverseEdgeDirection(Graph *sg) {
    if (sg == nullptr) {
        sg = graph;
    }

    for (auto e : sg->edges()) {
        if (getEdgeValue(e)) {
            sg->reverse(e);
        }
    }
}

//=================================================================================
PropertyInterface *BooleanProperty::clonePrototype(Graph *g, const std::string &n) const {
    if (g == nullptr) {
        return nullptr;
    }

    // allow to get an unregistered property (empty name)
    BooleanProperty *p = n.empty() ? new BooleanProperty(g) : g->getLocalBooleanProperty(n);
    p->setAllNodeValue(getNodeDefaultValue());
    p->setAllEdgeValue(getEdgeDefaultValue());
    return p;
}
//=================================================================================
PropertyInterface *BooleanVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
    if (!g) {
        return nullptr;
    }

    // allow to get an unregistered property (empty name)
    BooleanVectorProperty *p =
        n.empty() ? new BooleanVectorProperty(g) : g->getLocalBooleanVectorProperty(n);
    p->setAllNodeValue(getNodeDefaultValue());
    p->setAllEdgeValue(getEdgeDefaultValue());
    return p;
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::BooleanType, tlp::BooleanType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::BooleanVectorType, tlp::BooleanVectorType,
                                     tlp::VectorPropertyInterface>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::BooleanVectorType, tlp::BooleanType>),
    TLP_TEMPLATE_DEFINE_SCOPE)