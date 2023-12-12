/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/SizeProperty.h>
#include <talipot/DrawingTools.h>

using namespace std;
using namespace tlp;

class SizeMetaValueCalculator : public AbstractSizeProperty::MetaValueCalculator {
public:
  void computeMetaValue(AbstractSizeProperty *prop, node mN, Graph *sg, Graph *) override {
    // nothing to do if the subgraph is not linked to the property graph
    if (sg != prop->getGraph() && !prop->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
      tlp::warning()
          << "Warning : " << __PRETTY_FUNCTION__
          << " does not compute any value for a subgraph not linked to the graph of the property "
          << prop->getName().c_str() << std::endl;
#endif
      return;
    }

    if (sg->isEmpty()) {
      prop->setNodeValue(mN, Size(1, 1, 1));
      return;
    }

    if (prop->getName() == "viewSize") {
      // set meta node size as the enclosed subgraph bounding box
      BoundingBox box = tlp::computeBoundingBox(sg, sg->getLayoutProperty("viewLayout"),
                                                sg->getSizeProperty("viewSize"),
                                                sg->getDoubleProperty("viewRotation"));

      prop->setNodeValue(mN, Size(box.width(), box.height(), box.depth()));
    } else {
      // between the min and max computed values for other size properties
      prop->setNodeValue(mN, (static_cast<SizeProperty *>(prop)->getMax(sg) +
                              static_cast<SizeProperty *>(prop)->getMin(sg)) /
                                 2.0f);
    }
  }
};

static SizeMetaValueCalculator mvSizeCalculator;

const string SizeProperty::propertyTypename = "size";
const string SizeVectorProperty::propertyTypename = "vector<size>";

//==============================
SizeProperty::SizeProperty(Graph *sg, const std::string &n)
    : AbstractProperty<SizeType, SizeType>(sg, n) {
  // the computed meta value will be the average value
  setMetaValueCalculator(&mvSizeCalculator);
}
//====================================================================
void SizeProperty::scale(const tlp::Vec3f &v, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  for (auto n : itN) {
    Size tmpSize = getNodeValue(n);
    tmpSize *= v;
    setNodeValue(n, tmpSize);
  }

  for (auto e : itE) {
    Size tmpSize = getEdgeValue(e);
    tmpSize *= v;
    setEdgeValue(e, tmpSize);
  }

  resetMinMax();
  Observable::unholdObservers();
}
//=============================================================================
void SizeProperty::scale(const tlp::Vec3f &v, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  if (sg->isEmpty()) {
    return;
  }

  scale(v, sg->getNodes(), sg->getEdges());
}
//=============================================================================
Size SizeProperty::getMax(const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  uint sgi = sg->getId();

  if (!minMaxOk.contains(sgi)) {
    minMaxOk[sgi] = false;
  }

  if (!minMaxOk[sgi]) {
    computeMinMax(sg);
  }

  return max[sgi];
}
//=============================================================================
Size SizeProperty::getMin(const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  uint sgi = sg->getId();

  if (!minMaxOk.contains(sgi)) {
    minMaxOk[sgi] = false;
  }

  if (!minMaxOk[sgi]) {
    computeMinMax(sg);
  }

  return min[sgi];
}
//=============================================================================
void SizeProperty::computeMinMax(const Graph *sg) {
  Size maxS = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
  Size minS = {FLT_MAX, FLT_MAX, FLT_MAX};

  for (auto itn : sg->nodes()) {
    const Size &tmpSize = getNodeValue(itn);

    for (int i = 0; i < 3; ++i) {
      maxS[i] = tmpSize[i];
      minS[i] = tmpSize[i];
    }
  }

  uint sgi = sg->getId();
  minMaxOk[sgi] = true;
  min[sgi] = minS;
  max[sgi] = maxS;
}
//=============================================================================
void SizeProperty::resetMinMax() {
  minMaxOk.clear();
  min.clear();
  max.clear();
}
//=================================================================================
void SizeProperty::setNodeValue(const node n, tlp::StoredType<Size>::ConstReference v) {

  if (!minMaxOk.empty()) {
    const Size &oldV = getNodeValue(n);

    if (v != oldV) {
      // loop on subgraph min/max
      for (const auto &[graphId, minMax] : minMaxOk) {
        const Size &minV = min[graphId];
        const Size &maxV = max[graphId];

        // check if min or max has to be updated
        if ((v < minV) || (v > maxV) || (oldV == minV) || (oldV == maxV)) {
          resetMinMax();
          break;
        }
      }
    }
  }

  AbstractSizeProperty::setNodeValue(n, v);
}
//=================================================================================
void SizeProperty::setAllNodeValue(tlp::StoredType<Size>::ConstReference v, const Graph *graph) {
  resetMinMax();
  AbstractSizeProperty::setAllNodeValue(v, graph);
}
//=============================================================================
PropertyInterface *SizeProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  SizeProperty *p = n.empty() ? new SizeProperty(g) : g->getLocalSizeProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

int SizeProperty::compare(const node n1, const node n2) const {
  const Size &s1 = getNodeValue(n1);
  const Size &s2 = getNodeValue(n2);
  float v1 = fabs(s1[0]) * fabs(s1[1]) * fabs(s1[2]);
  float v2 = fabs(s2[0]) * fabs(s2[1]) * fabs(s2[2]);
  return v1 == v2 ? 0 : (v1 > v2 ? 1 : -1);
}

//=============================================================================
PropertyInterface *SizeVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  SizeVectorProperty *p = n.empty() ? new SizeVectorProperty(g) : g->getLocalSizeVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::SizeType, tlp::SizeType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::SizeVectorType, tlp::SizeVectorType,
                                                          tlp::VectorPropertyInterface>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::SizeVectorType, tlp::SizeType>),
    TLP_TEMPLATE_DEFINE_SCOPE)