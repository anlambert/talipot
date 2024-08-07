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

%Include PropertyInterface.sip

namespace tlp {

class GraphProperty : tlp::PropertyInterface /NoDefaultCtors/ {
%TypeHeaderCode
#include <talipot/GraphProperty.h>
%End
public:

  GraphProperty(tlp::Graph *graph, std::string name="");

  virtual ~GraphProperty();

  tlp::Graph* getNodeDefaultValue();

  std::set<tlp::edge> getEdgeDefaultValue();
%Docstring
tlp.GraphProperty.getEdgeDefaultValue() -> list of tlp.edge
%End

  tlp::Graph* getNodeValue(const tlp::node node);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = sipCpp->getNodeValue(*a0);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

  const std::set<tlp::edge> getEdgeValue(const tlp::edge edge);
%Docstring
tlp.GraphProperty.getEdgeValue(tlp.edge) -> set of tlp.edge
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::set<tlp::edge>(sipCpp->getEdgeValue(*a0));
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

  void setNodeValue(const tlp::node node, tlp::Graph *val);
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setNodeValue(*a0, a1);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

  void setEdgeValue(const tlp::edge edge, const std::set<tlp::edge> &val);
%Docstring
tlp.GraphProperty.setEdgeValue(tlp.edge, set of tlp.edge)
%End

%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setEdgeValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

  void setAllNodeValue(tlp::Graph *val);

  void setAllEdgeValue(const std::set<tlp::edge> &val);
%Docstring
tlp.GraphProperty.setAllEdgeValue(set of tlp.edge)
%End

  tlp::Graph* __getitem__(const tlp::node n) const;
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = sipCpp->getNodeValue(*a0);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

  void __setitem__(const tlp::node n, tlp::Graph* g) const;
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setNodeValue(*a0, a1);
} else {
  sipIsErr = throwInvalidNodeException(sipCpp->getGraph(), *a0);
}
%End

  std::set<tlp::edge> __getitem__(const tlp::edge e) const;
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipRes = new std::set<tlp::edge>(sipCpp->getEdgeValue(*a0));
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

  void __setitem__(const tlp::edge e, const std::set<tlp::edge> &se) const;
%MethodCode
if (sipCpp->getGraph()->isElement(*a0)) {
  sipCpp->setEdgeValue(*a0, *a1);
} else {
  sipIsErr = throwInvalidEdgeException(sipCpp->getGraph(), *a0);
}
%End

  std::string __repr__() const;
%MethodCode
std::ostringstream oss;
oss << "<graph property " << sipCpp->getName() << ">";
sipRes = new std::string(oss.str());
%End

};
};
