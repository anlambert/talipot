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

#include <talipot/ColorProperty.h>

using namespace std;
using namespace tlp;

const string ColorProperty::propertyTypename = "color";
const string ColorVectorProperty::propertyTypename = "vector<color>";

class ViewColorCalculator : public AbstractColorProperty::MetaValueCalculator {
public:
  void computeMetaValue(AbstractColorProperty *color, node mN, Graph *, Graph *) override {
    // meta node color is half opaque white
    (*color)[mN] = Color(255, 255, 255, 127);
  }

  void computeMetaValue(AbstractColorProperty *color, edge mE, Iterator<edge> *itE,
                        Graph *) override {
    // meta edge color is the color of the first underlying edge
    (*color)[mE] = (*color)[itE->next()];
    delete itE;
  }
};

// meta value calculator for viewColor
static ViewColorCalculator vColorCalc;

// Comparison of colors using hsv color space
// Return 0 if the colors are equal otherwise return -1 if the first object is lower than the second
// and 1 if the first object is greater than the second.
static int compareHSVValues(const Color &c1, const Color &c2);

//=================================================================================
ColorProperty::ColorProperty(Graph *g, const std::string &n) : AbstractColorProperty(g, n) {
  if (n == "viewColor") {
    setMetaValueCalculator(&vColorCalc);
  }
}
//=================================================================================
PropertyInterface *ColorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  ColorProperty *p = n.empty() ? new ColorProperty(g) : g->getLocalColorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=================================================================================
PropertyInterface *ColorVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  ColorVectorProperty *p =
      n.empty() ? new ColorVectorProperty(g) : g->getLocalColorVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

int ColorProperty::compare(const node n1, const node n2) const {
  return compareHSVValues(getNodeValue(n1), getNodeValue(n2));
}

int ColorProperty::compare(const edge e1, const edge e2) const {
  return compareHSVValues(getEdgeValue(e1), getEdgeValue(e2));
}

int compareHSVValues(const Color &c1, const Color &c2) {
  if (c1.getH() == c2.getH()) {
    if (c1.getS() == c2.getS()) {
      if (c1.getV() == c2.getV()) {
        return 0;
      } else {
        return c1.getV() < c2.getV() ? -1 : 1;
      }
    } else {
      return c1.getS() < c2.getS() ? -1 : 1;
    }
  } else {
    return c1.getH() < c2.getH() ? -1 : 1;
  }
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::ColorType, tlp::ColorType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::ColorVectorType, tlp::ColorVectorType,
                                     tlp::VectorPropertyInterface>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::ColorVectorType, tlp::ColorType>),
    TLP_TEMPLATE_DEFINE_SCOPE)
