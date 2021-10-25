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

#include <talipot/LayoutProperty.h>

using namespace std;
using namespace tlp;

inline double sqr(double x) {
  return (x * x);
}

const string LayoutProperty::propertyTypename = "layout";
const string CoordVectorProperty::propertyTypename = "vector<coord>";

// define a specific MetaValueCalculator
class LayoutMetaValueCalculator : public AbstractLayoutProperty::MetaValueCalculator {
public:
  void computeMetaValue(AbstractLayoutProperty *layout, node mN, Graph *sg, Graph *) override {
    // nothing to do if the subgraph is not linked to the property graph
    if (sg != layout->getGraph() && !layout->getGraph()->isDescendantGraph(sg)) {
#ifndef NDEBUG
      tlp::warning()
          << "Warning : " << __PRETTY_FUNCTION__
          << " does not compute any value for a subgraph not linked to the graph of the property "
          << layout->getName().c_str() << std::endl;
#endif
      return;
    }

    switch (sg->numberOfNodes()) {
    case 0:
      layout->setNodeValue(mN, Coord(0, 0, 0));
      return;

    case 1:
      layout->setNodeValue(mN, static_cast<LayoutProperty *>(layout)->getMax(sg));
      return;

    default:
      // between the min and max computed values
      layout->setNodeValue(mN, (static_cast<LayoutProperty *>(layout)->getMax(sg) +
                                static_cast<LayoutProperty *>(layout)->getMin(sg)) /
                                   2.0f);
    }
  }
};

static LayoutMetaValueCalculator mvLayoutCalculator;

//======================================================
LayoutProperty::LayoutProperty(Graph *sg, const std::string &n)
    : LayoutMinMaxProperty(sg, n, Coord(FLT_MAX, FLT_MAX, FLT_MAX),
                           Coord(-FLT_MAX, -FLT_MAX, -FLT_MAX), tlp::LineType::RealType(),
                           tlp::LineType::RealType()),
      nbBendedEdges(0) {
  // set default MetaValueCalculator
  setMetaValueCalculator(&mvLayoutCalculator);
}
//======================================================
Coord LayoutProperty::getMax(const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));
  return LayoutMinMaxProperty::getNodeMax(sg);
}
//======================================================
Coord LayoutProperty::getMin(const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));
  return LayoutMinMaxProperty::getNodeMin(sg);
}
//=================================================================================
#define X_ROT 0
#define Y_ROT 1
#define Z_ROT 2
static void rotateVector(Coord &vec, double alpha, int rot) {
  Coord backupVec = vec;
  double aRot = 2.0 * M_PI * alpha / 360.0;
  auto cosA = float(cos(aRot));
  auto sinA = float(sin(aRot));

  switch (rot) {
  case Z_ROT:
    vec[0] = backupVec[0] * cosA - backupVec[1] * sinA;
    vec[1] = backupVec[0] * sinA + backupVec[1] * cosA;
    break;

  case Y_ROT:
    vec[0] = backupVec[0] * cosA + backupVec[2] * sinA;
    vec[2] = backupVec[2] * cosA - backupVec[0] * sinA;
    break;

  case X_ROT:
    vec[1] = backupVec[1] * cosA - backupVec[2] * sinA;
    vec[2] = backupVec[1] * sinA + backupVec[2] * cosA;
    break;
  }
}
//=================================================================================
void LayoutProperty::rotate(double alpha, int rot, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  if (itN) {
    for (auto itn : itN) {
      Coord tmpCoord = getNodeValue(itn);
      rotateVector(tmpCoord, alpha, rot);
      setNodeValue(itn, tmpCoord);
    }
  }

  if (itE) {
    for (auto ite : itE) {
      auto vc = getEdgeValue(ite);
      if (!vc.empty()) {
        for (auto &c : vc) {
          rotateVector(c, alpha, rot);
        }
        setEdgeValue(ite, vc);
      }
    }
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::rotateX(double alpha, Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, X_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateY(double alpha, Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, Y_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateZ(double alpha, Iterator<node> *itN, Iterator<edge> *itE) {
  rotate(alpha, Z_ROT, itN, itE);
}
//=================================================================================
void LayoutProperty::rotateX(double alpha, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  rotateX(alpha, sg->getNodes(), sg->getEdges());
}
//=================================================================================
void LayoutProperty::rotateY(double alpha, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  rotateY(alpha, sg->getNodes(), sg->getEdges());
}
//=================================================================================
void LayoutProperty::rotateZ(double alpha, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  rotateZ(alpha, sg->getNodes(), sg->getEdges());
}
//=================================================================================
void LayoutProperty::scale(const tlp::Vec3f &v, Iterator<node> *itN, Iterator<edge> *itE) {
  Observable::holdObservers();

  for (auto itn : itN) {
    Coord tmpCoord = getNodeValue(itn);
    tmpCoord *= v;
    setNodeValue(itn, tmpCoord);
  }

  for (auto ite : itE) {
    auto vc = getEdgeValue(ite);
    if (!vc.empty()) {
      for (auto &c : vc) {
        c *= v;
      }

      setEdgeValue(ite, vc);
    }
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::scale(const tlp::Vec3f &v, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  scale(v, sg->getNodes(), sg->getEdges());
}
//=================================================================================
void LayoutProperty::translate(const tlp::Vec3f &v, Iterator<node> *itN, Iterator<edge> *itE) {

  // nothing to do if it is the null vector
  // or if there is no nodes or bends of edges to translate
  if ((v == tlp::Vec3f(0.0f)) || (itE == nullptr && itN == nullptr)) {
    return;
  }

  Observable::holdObservers();

  // invalidate the previously existing min/max computation
  resetBoundingBox();

  if (itN != nullptr) {
    for (auto itn : itN) {
      Coord tmpCoord = getNodeValue(itn);
      tmpCoord += v;
      // minimize computation time
      LayoutMinMaxProperty::setNodeValue(itn, tmpCoord);
    }
  }

  if (itE != nullptr && (nbBendedEdges > 0)) {
    for (auto ite : itE) {
      auto vc = getEdgeValue(ite);
      if (!vc.empty()) {
        for (auto &c : vc) {
          c += v;
        }

        // minimize computation time
        LayoutMinMaxProperty::setEdgeValue(ite, vc);
      }
    }
  }

  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::translate(const tlp::Vec3f &v, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  translate(v, sg->getNodes(), sg->getEdges());
}
//=================================================================================
void LayoutProperty::center(const Graph *sg) {

  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  Observable::holdObservers();
  Coord tr = getMax(sg) + getMin(sg);
  tr /= -2.0;
  translate(tr, sg);
  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::center(const Vec3f &newCenter, const Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  Observable::holdObservers();
  Coord curCenter = (getMax(sg) + getMin(sg)) / 2.0f;
  translate(newCenter - curCenter, sg);
  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::normalize(const Graph *sg) {

  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->isEmpty()) {
    return;
  }

  Observable::holdObservers();
  center();
  double dtmpMax = 1.0;

  for (auto itn : sg->nodes()) {
    const Coord &tmpCoord = getNodeValue(itn);
    dtmpMax = std::max(dtmpMax, sqr(tmpCoord[0]) + sqr(tmpCoord[1]) + sqr(tmpCoord[2]));
  }

  dtmpMax = 1.0 / sqrt(dtmpMax);
  scale(Coord(float(dtmpMax), float(dtmpMax), float(dtmpMax)), sg);
  resetBoundingBox();
  Observable::unholdObservers();
}
//=================================================================================
void LayoutProperty::perfectAspectRatio(const Graph *subgraph) {

  if (graph->isEmpty()) {
    return;
  }

  Observable::holdObservers();
  center(subgraph);
  double scaleX, scaleY, scaleZ;
  double deltaX, deltaY, deltaZ;
  deltaX = double(getMax()[0]) - double(getMin()[0]);
  deltaY = double(getMax()[1]) - double(getMin()[1]);
  deltaZ = double(getMax()[2]) - double(getMin()[2]);
  double delta = std::max(deltaX, deltaY);
  delta = std::max(delta, deltaZ);

  if (delta < 0.001) {
    return;
  }

  if (deltaX < 0.001) {
    deltaX = delta;
  }

  if (deltaY < 0.001) {
    deltaY = delta;
  }

  if (deltaZ < 0.001) {
    deltaZ = delta;
  }

  scaleX = delta / deltaX;
  scaleY = delta / deltaY;
  scaleZ = delta / deltaZ;
  scale(Coord(float(scaleX), float(scaleY), float(scaleZ)), subgraph);
  Observable::unholdObservers();
}

//=================================================================================
void LayoutProperty::clone_handler(const AbstractProperty<tlp::PointType, tlp::LineType> &proxyC) {
  if (typeid(this) == typeid(&proxyC)) {
    auto *proxy = static_cast<const LayoutProperty *>(&proxyC);
    _minMaxNode = proxy->_minMaxNode;
  }
}
//=================================================================================
void LayoutProperty::resetBoundingBox() {
  _minMaxNode.clear();
  _minMaxEdge.clear();
}
//================================================================================
void LayoutProperty::setNodeValue(const node n, tlp::StoredType<Coord>::ConstReference v) {
  LayoutMinMaxProperty::updateNodeValue(n, v);
  LayoutMinMaxProperty::setNodeValue(n, v);
}
//================================================================================
void LayoutProperty::setEdgeValue(const edge e,
                                  tlp::StoredType<std::vector<Coord>>::ConstReference v) {
  LayoutMinMaxProperty::updateEdgeValue(e, v);
  LayoutMinMaxProperty::setEdgeValue(e, v);
}
//=================================================================================
void LayoutProperty::setAllNodeValue(tlp::StoredType<Coord>::ConstReference v, const Graph *graph) {
  resetBoundingBox();
  LayoutMinMaxProperty::setAllNodeValue(v, graph);
}
//=================================================================================
void LayoutProperty::setAllEdgeValue(tlp::StoredType<std::vector<Coord>>::ConstReference v,
                                     const Graph *graph) {
  resetBoundingBox();
  LayoutMinMaxProperty::setAllEdgeValue(v, graph);
}
//=================================================================================
double LayoutProperty::averageAngularResolution(const Graph *sg) const {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  double result = 0;

  for (auto n : sg->nodes()) {
    result += averageAngularResolution(n, sg);
  }

  return result / double(sg->numberOfNodes());
}
//=================================================================================
struct AngularOrder {
  bool operator()(const Coord &c1, const Coord &c2) {
    return atan2(c1[1], c1[0]) < atan2(c2[1], c2[0]);
  }
  bool operator()(const pair<Coord, edge> &c1, const pair<Coord, edge> &c2) {
    return this->operator()(c1.first, c2.first);
  }
};

/*
 * TODO check code duplication with angularresolution function
 */
void LayoutProperty::computeEmbedding(Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  for (auto n : sg->nodes()) {
    computeEmbedding(n, sg);
  }
}

void LayoutProperty::computeEmbedding(const node n, Graph *sg) {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  if (sg->deg(n) < 2) {
    return;
  }

  //===========
  typedef pair<Coord, edge> pCE;

  list<pCE> adjCoord;
  // Extract all adjacent edges, the bends are taken
  // into account.
  for (auto ite : sg->incidence(n)) {
    if (!getEdgeValue(ite).empty()) {
      if (sg->source(ite) == n) {
        adjCoord.push_back(pCE(getEdgeValue(ite).front(), ite));
      } else {
        adjCoord.push_back(pCE(getEdgeValue(ite).back(), ite));
      }
    } else {
      adjCoord.push_back(pCE(getNodeValue(sg->opposite(ite, n)), ite));
    }
  }

  const Coord &center = getNodeValue(n);

  for (auto it = adjCoord.begin(); it != adjCoord.end();) {
    it->first -= center;
    float norm = it->first.norm();

    if (norm < 1E-5) {
      it = adjCoord.erase(it);
      cerr << "[ERROR]:" << __PRETTY_FUNCTION__ << " :: norms are too small for node:" << n << endl;
    } else {
      ++it;
    }
  }

  adjCoord.sort(AngularOrder());
  vector<edge> tmpOrder;

  for (const auto &[c, e] : adjCoord) {
    tmpOrder.push_back(e);
  }

  sg->setEdgeOrder(n, tmpOrder);
}
//=================================================================================
vector<double> LayoutProperty::angularResolutions(const node n, const Graph *sg) const {
  vector<double> result;

  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  double degree = sg->deg(n);

  if (sg->deg(n) == 0) {
    return result;
  }

  if (sg->deg(n) == 1) {
    result.push_back(0.0);
    return result;
  }

  //===========
  list<Coord> adjCoord;
  // Extract all adjacent edges, the bends are taken
  // into account.
  for (auto ite : sg->incidence(n)) {
    if (!getEdgeValue(ite).empty()) {
      if (sg->source(ite) == n) {
        adjCoord.push_back(getEdgeValue(ite).front());
      } else {
        adjCoord.push_back(getEdgeValue(ite).back());
      }
    } else {
      adjCoord.push_back(getNodeValue(sg->opposite(ite, n)));
    }
  }

  // Compute normalized vectors associated to incident edges.
  const Coord &center = getNodeValue(n);

  for (auto it = adjCoord.begin(); it != adjCoord.end();) {
    (*it) -= center;
    float norm = (*it).norm();

    if (norm) {
      (*it) /= norm;
      ++it;
    } else { // remove null vector
      it = adjCoord.erase(it);
    }
  }

  // Sort the vector to compute angles between two edges
  // Correctly.
  adjCoord.sort(AngularOrder());
  // Compute the angles
  auto it = adjCoord.begin();
  Coord first = (*it);
  Coord current = first;
  ++it;

  int stop = 2;

  for (; stop > 0;) {
    Coord next = *it;
    double cosTheta = current.dotProduct(next); // current * next;
    double sinTheta = (current ^ next)[2];

    if (cosTheta + 0.0001 > 1) {
      cosTheta -= 0.0001;
    }

    if (cosTheta - 0.0001 < -1) {
      cosTheta += 0.0001;
    }

    if (sinTheta + 0.0001 > 1) {
      sinTheta -= 0.0001;
    }

    if (sinTheta - 0.0001 < -1) {
      sinTheta += 0.0001;
    }

    if (sinTheta >= 0) {
      result.push_back(2.0 * M_PI / degree - acos(cosTheta));
    } else {
      result.push_back(2.0 * M_PI / degree - (2.0 * M_PI - acos(cosTheta)));
    }

    current = next;
    ++it;

    if (stop < 2) {
      stop = 0;
    }

    if (it == adjCoord.end()) {
      it = adjCoord.begin();
      stop--;
    }
  }

  return result;
}
//=================================================================================
double LayoutProperty::averageAngularResolution(const node n, const Graph *sg) const {
  vector<double> tmp(angularResolutions(n, sg));

  if (tmp.empty()) {
    return 0.;
  }

  double sum = 0.;

  for (auto d : tmp) {
    sum += d;
  }

  return sum / double(tmp.size());
}
//=================================================================================
double LayoutProperty::edgeLength(const edge e) const {
  const auto &[src, tgt] = graph->ends(e);
  Coord start = getNodeValue(src);
  const Coord &end = getNodeValue(tgt);
  double result = 0;
  const vector<Coord> &bends = getEdgeValue(e);

  for (const auto &p : bends) {
    result += (p - start).norm();
    start = p;
  }

  result += (end - start).norm();
  return result;
}
//=================================================================================
double LayoutProperty::averageEdgeLength(const Graph *sg) const {
  if (sg == nullptr) {
    sg = graph;
  }

  assert(sg == graph || graph->isDescendantGraph(sg));

  double ret = 0;
  for (auto e : sg->edges()) {
    ret += edgeLength(e);
  }
  return (ret / sg->numberOfEdges());
}
//=================================================================================
// removed until we have a working implementation
// uint LayoutProperty::crossingNumber() const {
//  tlp::warning() << "!!! Warning: Not Implemented function :";
//  tlp::warning() << __PRETTY_FUNCTION__ << std::endl;
//  return 0;
//}
//=================================================================================
PropertyInterface *LayoutProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  LayoutProperty *p = n.empty() ? new LayoutProperty(g) : g->getLocalLayoutProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}
//=============================================================
//=============================================================
void LayoutProperty::treatEvent(const Event &evt) {
  const auto *graphEvent = dynamic_cast<const tlp::GraphEvent *>(&evt);

  if (graphEvent) {
    switch (graphEvent->getType()) {
    case GraphEvent::TLP_ADD_NODE:
    case GraphEvent::TLP_DEL_NODE:
      LayoutMinMaxProperty::treatEvent(evt);
      break;

    case GraphEvent::TLP_REVERSE_EDGE: {
      std::vector<Coord> bends = getEdgeValue(graphEvent->getEdge());

      // reverse bends if needed
      if (bends.size() > 1) {
        uint halfSize = bends.size() / 2;

        for (uint i = 0, j = bends.size() - 1; i < halfSize; ++i, --j) {
          Coord tmp = bends[i];
          bends[i] = bends[j];
          bends[j] = tmp;
        }

        setEdgeValue(graphEvent->getEdge(), bends);
      }
    }

    default:
      break;
    }
  }
}
//=================================================================================
PropertyInterface *CoordVectorProperty::clonePrototype(Graph *g, const std::string &n) const {
  if (!g) {
    return nullptr;
  }

  // allow to get an unregistered property (empty name)
  CoordVectorProperty *p =
      n.empty() ? new CoordVectorProperty(g) : g->getLocalCoordVectorProperty(n);
  p->setAllNodeValue(getNodeDefaultValue());
  p->setAllEdgeValue(getEdgeDefaultValue());
  return p;
}

static inline void maxV(tlp::Coord &res, const tlp::Coord &cmp) {
  for (uint i = 0; i < 3; ++i) {
    res[i] = std::max(res[i], cmp[i]);
  }
}

static inline void minV(tlp::Coord &res, const tlp::Coord &cmp) {
  for (uint i = 0; i < 3; ++i) {
    res[i] = std::min(res[i], cmp[i]);
  }
}

/**
 * @brief Provides specific computation for min and max values of
 *Layout properties (they are specific in that they use the control points of the edges)
 **/
std::pair<tlp::Coord, tlp::Coord> LayoutProperty::computeMinMaxNode(const Graph *sg) {

  tlp::Coord maxT = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
  tlp::Coord minT = {FLT_MAX, FLT_MAX, FLT_MAX};

  for (auto itn : sg->nodes()) {
    const Coord &tmpCoord = this->getNodeValue(itn);
    maxV(maxT, tmpCoord);
    minV(minT, tmpCoord);
  }

  if (static_cast<LayoutProperty *>(this)->nbBendedEdges > 0) {
    for (auto ite : sg->edges()) {
      const LineType::RealType &value = this->getEdgeValue(ite);

      for (const auto &coord : value) {
        maxV(maxT, coord);
        minV(minT, coord);
      }
    }
  }

  uint sgi = sg->getId();

  // graph observation is now delayed
  // until we need to do some minmax computation
  if (_minMaxNode.find(sgi) == _minMaxNode.end()) {
    // launch graph hierarchy observation
    graph->addListener(this);
  }

  return _minMaxNode[sgi] = {minT, maxT};
}

/**
 * @brief Provides specific computation for min and max values of
 *Layout properties (they are specific in that they use the control points of the edges)
 **/
void LayoutProperty::updateEdgeValue(tlp::edge e,
                                     StoredType<LineType::RealType>::ConstReference newValue) {

  const std::vector<Coord> &oldV = this->getEdgeValue(e);

  if (newValue == oldV) {
    return;
  }

  static_cast<LayoutProperty *>(this)->nbBendedEdges +=
      (newValue.empty() ? 0 : 1) - (oldV.empty() ? 0 : 1);

  if (!_minMaxNode.empty()) {
    // loop on subgraph min/max
    for (const auto &[graphId, minMax] : _minMaxNode) {
      const auto &[minV, maxV] = minMax;

      bool reset = false;

      // check if min has to be updated
      for (const auto &v : newValue) {
        if (minV > v) {
          reset = true;
          break;
        }
      }

      if (!reset) {
        // check if max has to be updated
        for (const auto &v : newValue) {
          if (maxV < v) {
            reset = true;
            break;
          }
        }
      }

      if (!reset) {
        // check if minV belongs to oldV
        for (const auto &v : oldV) {
          if (minV == v) {
            reset = false;
            break;
          }
        }
      }

      if (!reset) {
        // check if maxV belongs to oldV
        for (const auto &v : oldV) {
          if (maxV == v) {
            reset = false;
            break;
          }
        }
      }

      // reset bounding box if needed
      if (reset) {
        _needGraphListener = static_cast<LayoutProperty *>(this)->nbBendedEdges > 0;
        removeListenersAndClearNodeMap();
        return;
      }
    }
  }

  // we need to observe the graph as soon as there is an edge
  // with bends
  if (!_needGraphListener &&
      (_needGraphListener = (static_cast<LayoutProperty *>(this)->nbBendedEdges > 0)) &&
      (_minMaxNode.find(graph->getId()) == _minMaxNode.end())) {
    graph->addListener(this);
  }
}

INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::AbstractProperty<tlp::PointType, tlp::LineType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(SINGLE_ARG(tlp::MinMaxProperty<tlp::PointType, tlp::LineType>),
                         TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractProperty<tlp::CoordVectorType, tlp::CoordVectorType,
                                     tlp::VectorPropertyInterface>),
    TLP_TEMPLATE_DEFINE_SCOPE)
INSTANTIATE_DLL_TEMPLATE(
    SINGLE_ARG(tlp::AbstractVectorProperty<tlp::CoordVectorType, tlp::PointType>),
    TLP_TEMPLATE_DEFINE_SCOPE)