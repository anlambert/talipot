/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#if defined(__GNUC__) && __GNUC__ >= 4 && \
    ((__GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ >= 1) || (__GNUC_MINOR__ >= 3))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

#include <talipot/GraphProperty.h>

using namespace std;
using namespace tlp;

const string GraphProperty::propertyTypename = "graph";

//==============================
GraphProperty::GraphProperty(Graph *sg, const std::string &n)
    : AbstractProperty<GraphType, EdgeSetType>(sg, n) {
  setAllNodeValue(nullptr);
}
//==============================
GraphProperty::~GraphProperty() {
  if (graph) {

    for (auto n : graph->nodes()) {
      if (getNodeValue(n) != nullptr) {
        getNodeValue(n)->removeListener(this);
      }
    }

    if (getNodeDefaultValue() != nullptr) {
      getNodeDefaultValue()->removeListener(this);
    }
  }
}
//==============================
void GraphProperty::setAllNodeValue(tlp::StoredType<GraphType::RealType>::ConstReference g,
                                    const Graph *graph) {
  // remove all observed graphs if any
  for (auto n : getNonDefaultValuatedNodes(graph)) {
    getNodeValue(n)->removeListener(this);
  }

  if ((!graph || getGraph() == graph) && getNodeDefaultValue() != nullptr) {
    getNodeDefaultValue()->removeListener(this);
  }

  AbstractGraphProperty::setAllNodeValue(g, graph);

  if (g != nullptr) {
    g->addListener(this);
  }
}
//==============================
void GraphProperty::setNodeValue(const node n,
                                 tlp::StoredType<GraphType::RealType>::ConstReference sg) {
  Graph *oldGraph = getNodeValue(n);

  if (oldGraph != nullptr && oldGraph != sg) {
    set<node> &refs = referencedGraph[oldGraph];
    refs.erase(n);
    if (refs.empty()) {
      if (oldGraph != getNodeDefaultValue()) {
        oldGraph->removeListener(this);
      }
    }
  }

  AbstractGraphProperty::setNodeValue(n, sg);

  if (sg == nullptr || oldGraph == sg) {
    return;
  }

  sg->addListener(this);

  if (sg != getNodeDefaultValue()) {
    referencedGraph[sg].insert(n);
  }
}
//============================================================
PropertyInterface *GraphProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  GraphProperty *p = n.empty() ? new GraphProperty(g) : g->getLocalGraphProperty(n);

  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=============================================================
// disabled, use setNodeValue instead
bool GraphProperty::setNodeStringValue(const node, const std::string &) {
  return false;
}
//=============================================================
// disabled use setAllNodeValue instead
bool GraphProperty::setAllNodeStringValue(const std::string &, const Graph *) {
  return false;
}
//=============================================================
// disabled, use setEdgeValue instead
bool GraphProperty::setEdgeStringValue(const edge, const std::string &) {
  return false;
}
//=============================================================
// disabled use setAllEdgeValue instead
bool GraphProperty::setAllEdgeStringValue(const std::string &, const Graph *) {
  return false;
}
//=============================================================
const set<edge> &GraphProperty::getReferencedEdges(const edge e) const {
  return const_cast<GraphProperty *>(this)->edgeProperties.get(e.id);
}
//=============================================================
void GraphProperty::treatEvent(const Event &evt) {
  if (evt.type() == EventType::TLP_DELETE) {

    auto *sg = static_cast<Graph *>(evt.sender());

#ifndef NDEBUG
    tlp::warning() << "Talipot Warning : A graph pointed by metanode(s) has been deleted, the "
                      "metanode(s) pointer has been set to zero in order to prevent segmentation "
                      "fault"
                   << std::endl;
#endif

    if (getNodeDefaultValue() == sg) {
      setNodeDefaultValue(nullptr);
      for (auto n : graph->nodes()) {
        if (getNodeValue(n) == sg) {
          setNodeValue(n, nullptr);
        }
      }
    }

    const set<node> &refs = referencedGraph.at(sg);

    if (!refs.empty()) {
      // don't change values if this no longer exists (when undoing)
      if (graph->existProperty(name)) {
        for (auto n : refs) {
          AbstractGraphProperty::setNodeValue(n, nullptr);
        }
      }

      referencedGraph[sg] = set<node>();
    }
  }
}

bool GraphProperty::readNodeDefaultValue(std::istream &iss) {
  // must read 0 (see GraphType::writeb)
  uint id = 0;

  if (!bool(iss.read(reinterpret_cast<char *>(&id), sizeof(id)))) {
    return false;
  }

  assert(id == 0);
  return id == 0;
}

bool GraphProperty::readNodeValue(std::istream &iss, node n) {
  // must read the id of a subgraph
  uint id = 0;

  if (!bool(iss.read(reinterpret_cast<char *>(&id), sizeof(id)))) {
    return false;
  }

  Graph *sg = graph->getRoot()->getDescendantGraph(id);
  setNodeValue(n, sg);
  return true;
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::GraphType, tlp::EdgeSetType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)