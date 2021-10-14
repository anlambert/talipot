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

#include <talipot/StringProperty.h>
#include <talipot/DoubleProperty.h>

using namespace std;
using namespace tlp;

const string StringProperty::propertyTypename = "string";
const string StringVectorProperty::propertyTypename = "vector<string>";

// viewLabel
class ViewLabelCalculator : public AbstractStringProperty::MetaValueCalculator {
public:
  // set the meta node label to label of viewMetric max corresponding node
  void computeMetaValue(AbstractStringProperty *label, node mN, Graph *sg, Graph *) override {
    // nothing to do if viewMetric does not exist
    if (!sg->existProperty("viewMetric")) {
      return;
    }

    node viewMetricMaxNode;
    double vMax = -DBL_MAX;
    DoubleProperty *metric = sg->getDoubleProperty("viewMetric");

    for (auto itn : sg->nodes()) {
      double value = (*metric)[itn];

      if (value > vMax) {
        vMax = value;
        viewMetricMaxNode = itn;
      }
    }

    if (viewMetricMaxNode.isValid()) {
      label->setNodeValue(mN, (*label)[viewMetricMaxNode]);
    }
  }
};

// meta value calculator for viewLabel
static ViewLabelCalculator vLabelCalc;

//=================================================================================
StringProperty::StringProperty(Graph *g, const std::string &n) : AbstractStringProperty(g, n) {
  if (n == "viewLabel") {
    setMetaValueCalculator(&vLabelCalc);
  }
}
//=================================================================================
PropertyInterface *StringProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  StringProperty *p = n.empty() ? new StringProperty(g) : g->getLocalStringProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=================================================================================
int StringProperty::compare(const node n1, const node n2) const {
  return getNodeValue(n1).compare(getNodeValue(n2));
}
//=================================================================================
int StringProperty::compare(const edge e1, const edge e2) const {
  return getEdgeValue(e1).compare(getEdgeValue(e2));
}
//=================================================================================
PropertyInterface *StringVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  StringVectorProperty *p =
      n.empty() ? new StringVectorProperty(g) : g->getLocalStringVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::StringType, tlp::StringType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::StringVectorType, tlp::StringVectorType,
                                     tlp::VectorPropertyInterface>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::StringVectorType, tlp::StringType>),
    TLP_TEMPLATE_DEFINE_SCOPE)
