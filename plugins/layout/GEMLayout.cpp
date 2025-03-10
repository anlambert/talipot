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

#include "GEMLayout.h"
// An implementation of the GEM3D layout algorithm, based on
// code by Arne Frick placed in the public domain.  See GEMLayout.h for further details.

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // 3D
    "If true, the layout is in 3D else it is computed in 2D.",

    // edge length
    "This metric is used to compute the length of edges.",

    // initial layout
    "The layout property used to compute the initial position of the graph elements. If none is "
    "given the initial position will be computed by the algorithm.",

    // selection of unmovable nodes
    "This property is used to indicate the unmovable nodes, the ones for which a new position will "
    "not be computed by the algorithm. This property is taken into account only if a layout "
    "property has been given to get the initial position of the unmovable nodes.",

    // max iterations
    "This parameter allows to choose the number of iterations. The default value of 0 corresponds "
    "to (3 * nb_nodes * nb_nodes) if the graph has more than 100 nodes."
    " For smaller graph, the number of iterations is set to 30 000."};

/*
 * GEM3D Constants
 */

static const float EDGELENGTH = 10;
static const float MAXATTRACT = 8192;

/*
 * GEM3D Default Parameter Values
 */
static const float IMAXTEMPDEF = 1.0f;
static const float ISTARTTEMPDEF = 0.3f;
static const float IFINALTEMPDEF = 0.05f;
static const int IMAXITERDEF = 10;
static const float IGRAVITYDEF = 0.05f;
static const float IOSCILLATIONDEF = 0.5f;
static const float IROTATIONDEF = 0.5f;
static const float ISHAKEDEF = 0.2f;

static const float AMAXTEMPDEF = 1.5f;
static const float ASTARTTEMPDEF = 1.0f;
static const float AFINALTEMPDEF = 0.02f;
static const int AMAXITERDEF = 3;
static const uint MIN_ITER =
    30000; // minimum number of iteration (equivalent to a graph with 100 nodes)
static const float AGRAVITYDEF = 0.1f;
static const float AOSCILLATIONDEF = 1.f;
static const float AROTATIONDEF = 1.f;
static const float ASHAKEDEF = 0.3f;

PLUGIN(GEMLayout)

GEMLayout::GEMLayout(const tlp::PluginContext *context)
    : LayoutAlgorithm(context), Iteration(0), _temperature(0), _maxtemp(0), _oscillation(0),
      _rotation(0), i_maxtemp(IMAXTEMPDEF), a_maxtemp(AMAXTEMPDEF), i_starttemp(ISTARTTEMPDEF),
      a_starttemp(ASTARTTEMPDEF), i_finaltemp(IFINALTEMPDEF), a_finaltemp(AFINALTEMPDEF),
      i_maxiter(IMAXITERDEF), a_maxiter(AMAXITERDEF), i_gravity(IGRAVITYDEF),
      a_gravity(AGRAVITYDEF), i_oscillation(IOSCILLATIONDEF), a_oscillation(AOSCILLATIONDEF),
      i_rotation(IROTATIONDEF), a_rotation(AROTATIONDEF), i_shake(ISHAKEDEF), a_shake(ASHAKEDEF),
      _dim(2), _nbNodes(0), _useLength(false), metric(nullptr), fixedNodes(nullptr), max_iter(0) {
  addInParameter<bool>("3D layout", paramHelp[0].data(), "false");
  addInParameter<NumericProperty *>("edge length", paramHelp[1].data(), "", false);
  addInParameter<LayoutProperty>("initial layout", paramHelp[2].data(), "", false);
  addInParameter<BooleanProperty>("unmovable nodes", paramHelp[3].data(), "", false);
  addInParameter<uint>("max iterations", paramHelp[4].data(), "0");
  addDependency("Connected Components Packing", "1.0");
}
//=========================================================
GEMLayout::~GEMLayout() = default;
//=========================================================
uint GEMLayout::select() {
  return randomNumber(graph->numberOfNodes() - 1);
}
//=========================================================
void GEMLayout::vertexdata_init(const float starttemp) {
  _temperature = 0;
  _center.fill(0);

  for (auto &p : _particules) {
    p.heat = starttemp;
    _temperature += p.heat * p.heat;
    p.imp.fill(0);
    p.dir = 0;
    p.mass = 1.f + p.mass / 3.f;
    _center += p.pos;
  }
}
//=========================================================
void GEMLayout::updateLayout() {
  for (uint i = 0; i < graph->numberOfNodes(); ++i) {
    //    tlp::warning() << "pos up ==> :" << _particules[i].pos << endl;
    result->setNodeValue(_particules[i].n, _particules[i].pos);
  }
}
//=========================================================
/*
 * compute force exerced on node v
 * if testPlaced is equal to true, only already placed nodes
 * are considered
 */
Coord GEMLayout::computeForces(uint v, float shake, float gravity, bool testPlaced) {
  Coord force;
  Coord vPos = _particules[v].pos;
  float vMass = _particules[v].mass;
  node vNode = _particules[v].n;

  // Init force in a random position
  for (uint cnt = 0; cnt < _dim; ++cnt) {
    force[cnt] = shake - float(randomNumber(2. * shake));
  }

  // Add central force
  force += (_center / float(_nbNodes) - vPos) * vMass * gravity;
  //
  double maxEdgeLength;

  if (_useLength) {
    maxEdgeLength = std::max(2.0, metric->getEdgeDoubleMin(graph));
  } else {
    maxEdgeLength = EDGELENGTH;
  }

  maxEdgeLength *= maxEdgeLength;

  // repulsive forces (magnetic)
  for (uint u = 0; u < _nbNodes; ++u) {
    if (!testPlaced || _particules[u].in > 0) { // test whether the node is already placed
      Coord d = vPos - _particules[u].pos;
      float n = d[0] * d[0] + d[1] * d[1] + d[2] * d[2]; // d.norm() * d.norm();

      if (n > 0.) {
        force += d * float(maxEdgeLength) / n;
      }
    }
  }

  // attractive forces
  for (auto e : graph->incidence(vNode)) {
    node uNode = graph->opposite(e, vNode);

    if (uNode == vNode) {
      // nothing to do if it is a self loop
      continue;
    }

    const GEMparticule &gemQ = _particules[graph->nodePos(uNode)];

    if (!testPlaced || gemQ.in > 0) { // test whether the node is already placed
      float edgeLength;

      if (_useLength) {
        edgeLength = float(metric->getEdgeDoubleValue(e));
      } else {
        edgeLength = EDGELENGTH;
      }

      Coord d = vPos - gemQ.pos;
      float n = d.norm() / vMass;
      n = std::min(n, MAXATTRACT); //   1048576L
      force -= (d * n) / (edgeLength * edgeLength + 1.f);
    }
  }
  return force;
}
//==========================================================================
void GEMLayout::insert() {
  int startNode;

  this->vertexdata_init(i_starttemp);

  _oscillation = i_oscillation;
  _rotation = i_rotation;
  _maxtemp = i_maxtemp;

  node nCenter = graphCenterHeuristic(graph);
  uint v = _particules[graph->nodePos(nCenter)].id;

  for (uint i = 0; i < _nbNodes; ++i) {
    _particules[i].in = 0;
  }

  _particules[v].in = -1;

  startNode = -1;

  for (uint i = 0; i < _nbNodes; ++i) {
    if (pluginProgress->isPreviewMode()) {
      updateLayout();
    }

    if (pluginProgress->progress(i, _nbNodes) != ProgressState::TLP_CONTINUE) {
      return;
    }

    // choose particule with the minimum value
    int d = 0;

    for (uint j = 0; j < _nbNodes; ++j) {
      if (_particules[j].in < d) {
        d = _particules[j].in;
        v = j;
      }
    }

    //
    _particules[v].in = 1;
    node vNode = _particules[v].n;

    // nothing to do if vNode is a fixed node
    if (fixedNodes && fixedNodes->getNodeValue(vNode)) {
      continue;
    }

    // remove one to non-visited nodes
    for (auto uNode : graph->getInOutNodes(vNode)) {
      if (uNode == vNode) {
        // nothing to do if it is a self loop
        continue;
      }

      GEMparticule &gemQ = _particules[graph->nodePos(uNode)];
      if (gemQ.in <= 0) {
        --gemQ.in;
      }
    }

    GEMparticule &gemP = _particules[v];
    gemP.pos.fill(0);

    if (startNode >= 0) {
      int d = 0;
      for (auto uNode : graph->getInOutNodes(vNode)) {
        if (uNode == vNode) {
          // nothing to do if it a self loop
          continue;
        }

        GEMparticule &gemQ = _particules[graph->nodePos(uNode)];
        if (gemQ.in > 0) {
          gemP.pos += gemQ.pos;
          ++d;
        }
      }

      if (d > 1) {
        gemP.pos /= float(d);
      }

      d = 0;

      while ((d++ < i_maxiter) && (gemP.heat > i_finaltemp)) {
        this->displace(v, computeForces(v, i_shake, i_gravity, true));
      }
    } else {
      startNode = i;
    }
  }
}
//==========================================================================
void GEMLayout::displace(uint v, Coord imp) {

  float nV = imp.norm();

  if (nV > 0) {
    float t = _particules[v].heat;
    imp /= nV; // normalize imp

    _temperature -= t * t;

    // oscillation
    float cosA = imp.dotProduct(_particules[v].imp);
    t += _oscillation * cosA * t;
    t = std::min(t, _maxtemp);
    // rotation
    float sinA = (imp ^ _particules[v].imp).norm();
    t -= _rotation * sinA * t;
    t = std::max(t, 0.01f);

    _temperature += t * t;

    _particules[v].heat = t;
    _particules[v].pos += imp * t;
    _center += imp * t;
    _particules[v].imp = imp;
  }
}
//==========================================================================
void GEMLayout::a_round() {
  for (uint i = 0; i < _nbNodes; ++i) {
    uint v = this->select();
    node vNode = _particules[v].n;

    // nothing to do if vNode is a fixed node
    if (fixedNodes && fixedNodes->getNodeValue(vNode)) {
      continue;
    }

    Coord force = computeForces(v, a_shake, a_gravity, false);
    this->displace(v, force);
    Iteration++;
  }
}
//============================================================================
void GEMLayout::arrange() {
  float stop_temperature;

  double maxEdgeLength;

  if (_useLength) {
    maxEdgeLength = std::max(2.0, metric->getEdgeDoubleMin(graph));
  } else {
    maxEdgeLength = EDGELENGTH;
  }

  maxEdgeLength *= maxEdgeLength;

  this->vertexdata_init(a_starttemp);

  _oscillation = a_oscillation;
  _rotation = a_rotation;
  _maxtemp = a_maxtemp;
  stop_temperature = float(a_finaltemp * a_finaltemp * maxEdgeLength * _nbNodes);
  Iteration = 0;

  while (_temperature > stop_temperature && Iteration < max_iter) {
    //    tlp::warning() << "t°:"<< _temperature << "/" << stop_temperature << " it:" << Iteration
    //    << endl;
    if (pluginProgress->progress(Iteration, max_iter / 2) != ProgressState::TLP_CONTINUE) {
      return;
    }

    if (pluginProgress->isPreviewMode()) {
      updateLayout();
    }

    this->a_round();
  }
}
//============================================================================
bool GEMLayout::run() {
  if (!ConnectedTest::isConnected(graph)) {
    // for each component draw
    string err;
    auto components = ConnectedTest::computeConnectedComponents(graph);

    for (const auto &component : components) {
      Graph *tmp = graph;
      // apply "GEM (Frick)" on the subgraph induced
      // by the current connected component
      graph = graph->inducedSubGraph(component);
      auto result = run();
      tmp->delSubGraph(graph);
      // restore current graph
      graph = tmp;
      // return if needed
      if (!result) {
        return result;
      }
    }

    // call connected component packing
    LayoutProperty tmpLayout(graph);
    DataSet ds;
    ds.set("coordinates", result);
    graph->applyPropertyAlgorithm("Connected Components Packing", &tmpLayout, err, &ds,
                                  pluginProgress);
    *result = tmpLayout;
    return true;
  }

  /* Handle parameters */
  metric = nullptr;
  LayoutProperty *layout = graph->getLayoutProperty("viewLayout");

  bool is3D = false;
  bool initLayout = false;
  _useLength = false;
  max_iter = 0;

  if (dataSet != nullptr) {
    dataSet->get("3D layout", is3D);
    _useLength = dataSet->get("edge length", metric) && metric != nullptr;
    dataSet->get("max iterations", max_iter);
    initLayout = !dataSet->get("initial layout", layout);

    if (initLayout) {
      dataSet->get("unmovable nodes", fixedNodes);
    }
  }

  if (is3D) {
    _dim = 3;
  } else {
    _dim = 2;
  }

  _nbNodes = graph->numberOfNodes();

  // no bends
  result->setAllEdgeValue(vector<Coord>(0));

  // initialize a random sequence according the given seed
  tlp::initRandomSequence();

  if (max_iter == 0) {
    max_iter = std::max(a_maxiter * _nbNodes * _nbNodes, MIN_ITER);
  }

  _particules.resize(_nbNodes);
  /* Max Edge to scale actual edges length to preferres length */
  uint i = 0;
  for (auto n : graph->nodes()) {
    _particules[i] = GEMparticule(float(graph->deg(n)));
    _particules[i].n = n;
    _particules[i].id = i;

    if (!initLayout && layout != nullptr) {
      _particules[i].pos = layout->getNodeValue(n);
    } else {
      _particules[i].pos.fill(0);
    }

    ++i;
  }

  if (initLayout && layout != nullptr) {
    if (i_finaltemp < i_starttemp) {
      this->insert();
    }
  }

  if ((pluginProgress->state() == ProgressState::TLP_CONTINUE) && (a_finaltemp < a_starttemp)) {
    this->arrange();
  }

  if (pluginProgress->state() != ProgressState::TLP_CANCEL) {
    updateLayout();
  }

  return pluginProgress->state() != ProgressState::TLP_CANCEL;
}
//=========================================================
