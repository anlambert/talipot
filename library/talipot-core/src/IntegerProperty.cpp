/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <climits>
#include <talipot/IntegerProperty.h>
#include <talipot/GraphTools.h>

using namespace std;
using namespace tlp;

const string IntegerProperty::propertyTypename = "int";
const string IntegerVectorProperty::propertyTypename = "vector<int>";

//==============================
/// Constructeur d'un IntegerProperty
IntegerProperty::IntegerProperty(Graph *g, const std::string &n)
    : IntegerMinMaxProperty(g, n, -INT_MAX, INT_MAX, -INT_MAX, INT_MAX) {}
//====================================================================
void IntegerProperty::clone_handler(
    AbstractProperty<tlp::IntegerType, tlp::IntegerType, tlp::NumericProperty> &proxyC) {
  if (typeid(this) == typeid(&proxyC)) {
    IntegerProperty *proxy = static_cast<IntegerProperty *>(&proxyC);
    minMaxNode = proxy->minMaxNode;
    minMaxEdge = proxy->minMaxEdge;
  }
}
//=================================================================================
PropertyInterface *IntegerProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g)
    return nullptr;

  // allow to get an unregistered property (empty name)
  IntegerProperty *p = n.empty() ? new IntegerProperty(g) : g->getLocalIntegerProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=================================================================================
void IntegerProperty::setNodeValue(const node n, tlp::StoredType<int>::ReturnedConstValue v) {
  IntegerMinMaxProperty::updateNodeValue(n, v);
  IntegerMinMaxProperty::setNodeValue(n, v);
}
//=================================================================================
void IntegerProperty::setEdgeValue(const edge e, tlp::StoredType<int>::ReturnedConstValue v) {
  IntegerMinMaxProperty::updateEdgeValue(e, v);
  IntegerMinMaxProperty::setEdgeValue(e, v);
}
//=================================================================================
void IntegerProperty::setAllNodeValue(tlp::StoredType<int>::ReturnedConstValue v,
                                      const Graph *graph) {
  IntegerMinMaxProperty::updateAllNodesValues(v);
  IntegerMinMaxProperty::setAllNodeValue(v, graph);
}
//=================================================================================
void IntegerProperty::setAllEdgeValue(tlp::StoredType<int>::ReturnedConstValue v,
                                      const Graph *graph) {
  IntegerMinMaxProperty::updateAllEdgesValues(v);
  IntegerMinMaxProperty::setAllEdgeValue(v, graph);
}
//=============================================================
void IntegerProperty::treatEvent(const Event &evt) {
  IntegerMinMaxProperty::treatEvent(evt);
}
//=================================================================================
int IntegerProperty::compare(const node n1, const node n2) const {
  return getNodeValue(n1) - getNodeValue(n2);
}
//=================================================================================
int IntegerProperty::compare(const edge e1, const edge e2) const {
  return getEdgeValue(e1) - getEdgeValue(e2);
}
//=================================================================================
PropertyInterface *IntegerVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g)
    return nullptr;

  // allow to get an unregistered property (empty name)
  IntegerVectorProperty *p =
      n.empty() ? new IntegerVectorProperty(g) : g->getLocalIntegerVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

//===============================================================
void IntegerProperty::nodesUniformQuantification(unsigned int k) {
  std::map<double, int> nodeMapping;
  buildNodesUniformQuantification(graph, this, k, nodeMapping);

  for (auto itn : graph->nodes()) {
    setNodeValue(itn, nodeMapping[getNodeValue(itn)]);
  }
}

//===============================================================
void IntegerProperty::edgesUniformQuantification(unsigned int k) {
  std::map<double, int> edgeMapping;
  buildEdgesUniformQuantification(graph, this, k, edgeMapping);

  for (auto ite : graph->edges()) {
    setEdgeValue(ite, edgeMapping[getEdgeValue(ite)]);
  }
}
