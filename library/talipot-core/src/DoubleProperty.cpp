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

#include <talipot/DoubleProperty.h>
#include <talipot/GraphTools.h>

using namespace std;
using namespace tlp;

const string DoubleProperty::propertyTypename = "double";
const string DoubleVectorProperty::propertyTypename = "vector<double>";

//=============================================================
// Predefined Meta Value Calculators
//=============================================================
typedef void (*DoubleNodePredefinedCalculator)(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, node mN,
    Graph *sg);

typedef void (*DoubleEdgePredefinedCalculator)(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, edge mE,
    Iterator<edge> *itE);

// average values
static void computeNodeAvgValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, node mN,
    Graph *sg) {
  // nothing to do if the subgraph is not linked to the property graph
  if (sg != metric->getGraph() && !metric->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
    tlp::warning()
        << "Warning : " << __PRETTY_FUNCTION__
        << " does not compute any value for a subgraph not linked to the graph of the property "
        << metric->getName().c_str() << std::endl;
#endif
    return;
  }

  double value = 0;
  uint nbNodes = 0;
  for (auto n : sg->nodes()) {
    ++nbNodes;
    value += metric->getNodeValue(n);
  }

  if (nbNodes) {
    metric->setNodeValue(mN, value / nbNodes);
  }
}

static void computeEdgeAvgValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, edge mE,
    Iterator<edge> *itE) {
  double value = 0;
  uint nbEdges = 0;

  for (auto e : itE) {
    ++nbEdges;
    value += metric->getEdgeValue(e);
  }

  if (nbEdges) {
    metric->setEdgeValue(mE, value / nbEdges);
  }
}

// sum values
static void computeNodeSumValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, node mN,
    Graph *sg) {
  // nothing to do if the subgraph is not linked to the property graph
  if (sg != metric->getGraph() && !metric->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
    tlp::warning()
        << "Warning : " << __PRETTY_FUNCTION__
        << " does not compute any value for a subgraph not linked to the graph of the property "
        << metric->getName().c_str() << std::endl;
#endif
    return;
  }

  double value = 0;
  for (auto n : sg->nodes()) {
    value += metric->getNodeValue(n);
  }
  metric->setNodeValue(mN, value);
}

static void computeEdgeSumValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, edge mE,
    Iterator<edge> *itE) {
  double value = 0;

  for (auto e : itE) {
    value += metric->getEdgeValue(e);
  }

  metric->setEdgeValue(mE, value);
}

// max values
static void computeNodeMaxValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, node mN,
    Graph *sg) {
  // nothing to do if the subgraph is not linked to the property graph
  if (sg != metric->getGraph() && !metric->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
    tlp::warning()
        << "Warning : " << __PRETTY_FUNCTION__
        << " does not compute any value for a subgraph not linked to the graph of the property "
        << metric->getName().c_str() << std::endl;
#endif
    return;
  }

  double value = -DBL_MAX;
  for (auto n : sg->nodes()) {
    double nVal = metric->getNodeValue(n);

    if (nVal > value) {
      value = nVal;
    }
  }
  metric->setNodeValue(mN, value);
}

static void computeEdgeMaxValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, edge mE,
    Iterator<edge> *itE) {
  double value = -DBL_MAX;

  for (auto e : itE) {
    double eVal = metric->getEdgeValue(e);

    if (eVal > value) {
      value = eVal;
    }
  }

  metric->setEdgeValue(mE, value);
}

// min values
static void computeNodeMinValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, node mN,
    Graph *sg) {
  // nothing to do if the subgraph is not linked to the property graph
  if (sg != metric->getGraph() && !metric->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
    tlp::warning()
        << "Warning : " << __PRETTY_FUNCTION__
        << " does not compute any value for a subgraph not linked to the graph of the property "
        << metric->getName().c_str() << std::endl;
#endif
    return;
  }

  double value = DBL_MAX;
  for (auto n : sg->nodes()) {
    double nVal = metric->getNodeValue(n);

    if (nVal < value) {
      value = nVal;
    }
  }
  metric->setNodeValue(mN, value);
}

static void computeEdgeMinValue(
    AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric, edge mE,
    Iterator<edge> *itE) {
  double value = DBL_MAX;

  for (auto e : itE) {
    double eVal = metric->getEdgeValue(e);

    if (eVal < value) {
      value = eVal;
    }
  }

  metric->setEdgeValue(mE, value);
}

// 2 arrays to hold the predefined functions
DoubleNodePredefinedCalculator nodeCalculators[] = {
    nullptr, computeNodeAvgValue, computeNodeSumValue, computeNodeMaxValue, computeNodeMinValue};

DoubleEdgePredefinedCalculator edgeCalculators[] = {
    nullptr, computeEdgeAvgValue, computeEdgeSumValue, computeEdgeMaxValue, computeEdgeMinValue};

class DoublePropertyPredefinedCalculator
    : public AbstractProperty<tlp::DoubleType, tlp::DoubleType,
                              tlp::NumericProperty>::MetaValueCalculator {
  DoubleNodePredefinedCalculator nodeCalc;
  DoubleEdgePredefinedCalculator edgeCalc;

public:
  DoublePropertyPredefinedCalculator(
      DoubleProperty::PredefinedMetaValueCalculator nCalc = DoubleProperty::AVG_CALC,
      DoubleProperty::PredefinedMetaValueCalculator eCalc = DoubleProperty::AVG_CALC)
      : AbstractProperty<tlp::DoubleType, tlp::DoubleType,
                         tlp::NumericProperty>::MetaValueCalculator(),
        nodeCalc(nodeCalculators[nCalc]), edgeCalc(edgeCalculators[eCalc]) {}

  void
  computeMetaValue(AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric,
                   node mN, Graph *sg, Graph *) override {
    if (nodeCalc) {
      nodeCalc(metric, mN, sg);
    }
  }

  void
  computeMetaValue(AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty> *metric,
                   edge mE, Iterator<edge> *itE, Graph *) override {
    if (edgeCalc) {
      edgeCalc(metric, mE, itE);
    } else {
      delete itE;
    }
  }
};

// default calculator
static DoublePropertyPredefinedCalculator avgCalculator;

class ViewBorderWidthCalculator : public DoubleMinMaxProperty::MetaValueCalculator {
public:
  void computeMetaValue(AbstractProperty<DoubleType, DoubleType, NumericProperty> *width, node mN,
                        Graph *, Graph *) override {
    // meta node border width is 1
    width->setNodeValue(mN, 1);
  }
};

// meta value calculator for viewBorderWidth
static ViewBorderWidthCalculator vWidthCalc;

//==============================
/// Constructeur d'un DoubleProperty
DoubleProperty::DoubleProperty(Graph *g, const std::string &n)
    : DoubleMinMaxProperty(g, n, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX) {
  assert(g != nullptr);

  // the computed meta value will be the average value
  if (n != "viewBorderWidth") {
    setMetaValueCalculator(&avgCalculator);
  } else {
    setMetaValueCalculator(&vWidthCalc);
  }
}

//===============================================================
void DoubleProperty::nodesUniformQuantification(uint k) {
  std::map<double, int> nodeMapping;
  buildNodesUniformQuantification(graph, this, k, nodeMapping);

  for (auto itn : graph->nodes()) {
    setNodeValue(itn, nodeMapping[getNodeValue(itn)]);
  }
}
//===============================================================
void DoubleProperty::edgesUniformQuantification(uint k) {
  std::map<double, int> edgeMapping;
  buildEdgesUniformQuantification(graph, this, k, edgeMapping);

  for (auto ite : graph->edges()) {
    setEdgeValue(ite, edgeMapping[getEdgeValue(ite)]);
  }
}
//====================================================================
void DoubleProperty::clone_handler(
    const AbstractProperty<DoubleType, DoubleType, tlp::NumericProperty> &proxyC) {
  auto *proxy = static_cast<const DoubleProperty *>(&proxyC);
  _minMaxNode = proxy->_minMaxNode;
  _minMaxEdge = proxy->_minMaxEdge;
}
//=================================================================================
void DoubleProperty::setNodeValue(const node n, tlp::StoredType<double>::ConstReference v) {
  DoubleMinMaxProperty::updateNodeValue(n, v);
  DoubleMinMaxProperty::setNodeValue(n, v);
}
//=================================================================================
void DoubleProperty::setEdgeValue(const edge e, tlp::StoredType<double>::ConstReference v) {
  DoubleMinMaxProperty::updateEdgeValue(e, v);
  DoubleMinMaxProperty::setEdgeValue(e, v);
}
//=================================================================================
void DoubleProperty::setAllNodeValue(tlp::StoredType<double>::ConstReference v,
                                     const Graph *graph) {
  DoubleMinMaxProperty::updateAllNodesValues(v);
  DoubleMinMaxProperty::setAllNodeValue(v, graph);
}
//=================================================================================
void DoubleProperty::setAllEdgeValue(tlp::StoredType<double>::ConstReference v,
                                     const Graph *graph) {
  DoubleMinMaxProperty::updateAllEdgesValues(v);
  DoubleMinMaxProperty::setAllEdgeValue(v, graph);
}
//=================================================================================
PropertyInterface *DoubleProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  DoubleProperty *p = n.empty() ? new DoubleProperty(g) : g->getLocalDoubleProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=================================================================================
PropertyInterface *DoubleVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  DoubleVectorProperty *p =
      n.empty() ? new DoubleVectorProperty(g) : g->getLocalDoubleVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=============================================================
void DoubleProperty::setMetaValueCalculator(PredefinedMetaValueCalculator nodeCalc,
                                            PredefinedMetaValueCalculator edgeCalc) {
  setMetaValueCalculator(new DoublePropertyPredefinedCalculator(nodeCalc, edgeCalc));
}
//=============================================================
void DoubleProperty::setMetaValueCalculator(PropertyInterface::MetaValueCalculator *calc) {
  if (metaValueCalculator && metaValueCalculator != &avgCalculator &&
      typeid(metaValueCalculator) == typeid(DoublePropertyPredefinedCalculator)) {
    delete metaValueCalculator;
  }

  metaValueCalculator = calc;
}
//=============================================================
void DoubleProperty::treatEvent(const Event &evt) {
  DoubleMinMaxProperty::treatEvent(evt);
}

INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::MinMaxProperty<tlp::DoubleType, tlp::DoubleType, tlp::NumericProperty>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::DoubleVectorType, tlp::DoubleVectorType,
                                     tlp::VectorPropertyInterface>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::DoubleVectorType, tlp::DoubleType>),
    TLP_TEMPLATE_DEFINE_SCOPE)
