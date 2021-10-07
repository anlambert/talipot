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

#include "Grip.h"
#include "Distances.h"

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // 3D
    "If true the layout is in 3D else it is computed in 2D"};

//======================================================
Grip::Grip(const tlp::PluginContext *context)
    : LayoutAlgorithm(context), misf(nullptr), edgeLength(0), level(0), currentGraph(nullptr),
      _dim(0) {
  addInParameter<bool>("3D layout", paramHelp[0].data(), "false");
  addDependency("Connected Components Packing", "1.0");
}
Grip::~Grip() = default;

void Grip::computeCurrentGraphLayout() {
  if (currentGraph->numberOfNodes() <= 3) {
    uint nb_nodes = currentGraph->numberOfNodes();
    auto nodes = currentGraph->nodes();

    if (nb_nodes == 1) {
      node n = currentGraph->getOneNode();
      result->setNodeValue(n, Coord(0, 0, 0));
    }

    if (nb_nodes == 2) {
      node n1 = nodes[0];
      node n2 = nodes[1];
      result->setNodeValue(n1, Coord(0, 0, 0));
      result->setNodeValue(n2, Coord(1, 0, 0));
    } else if (nb_nodes == 3) {
      if (currentGraph->numberOfEdges() == 3) {
        node n1 = nodes[0];
        node n2 = nodes[1];
        node n3 = nodes[2];
        result->setNodeValue(n1, Coord(0, 0, 0));
        result->setNodeValue(n2, Coord(1, 0, 0));
        result->setNodeValue(n3, Coord(0.5, sqrt(0.5), 0));
      } else {
        auto edges = currentGraph->edges();
        edge e1 = edges[0];
        edge e2 = edges[1];

        auto [src1, tgt1] = currentGraph->ends(e1);
        const auto &[src2, tgt2] = currentGraph->ends(e2);

        node n;

        if (tgt2 == src1) {
          src1 = src2;
          n = tgt1;
          tgt1 = tgt2;
        } else if (src2 == src1) {
          src1 = tgt2;
          n = tgt1;
          tgt1 = src2;
        } else {
          n = (src2 == tgt1) ? tgt2 : src2;
        }

        result->setNodeValue(src1, Coord(0, 0, 0));
        result->setNodeValue(tgt1, Coord(1, 0, 0));
        result->setNodeValue(n, Coord(2, 0, 0));
      }
    }
  } else {
    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    MISFiltering filtering(currentGraph);
    misf = &filtering;
    computeOrdering();
    init();
    firstNodesPlacement();
    placement();
  }
}
//======================================================
bool Grip::run() {
  bool is3D = false;

  if (dataSet != nullptr) {
    if (!dataSet->get("3D layout", is3D)) {
      is3D = false;
    }
  }

  if (is3D) {
    _dim = 3;
  } else {
    _dim = 2;
  }

  if (pluginProgress) {
    // user cannot interact while computing
    pluginProgress->showPreview(false);
    pluginProgress->showStops(false);
  }

  auto components = ConnectedTest::computeConnectedComponents(graph);

  if (components.size() > 1) {
    for (const auto &component : components) {
      currentGraph = graph->inducedSubGraph(component);
      computeCurrentGraphLayout();
      graph->delSubGraph(currentGraph);
    }

    string err;
    DataSet tmp;
    tmp.set("coordinates", result);
    LayoutProperty layout(graph);
    graph->applyPropertyAlgorithm("Connected Components Packing", &layout, err, &tmp);

    for (auto n : graph->nodes()) {
      result->setNodeValue(n, layout.getNodeValue(n));
    }

  } else {
    currentGraph = graph;
    computeCurrentGraphLayout();
  }

  return true;
}
//======================================================
void Grip::computeOrdering() {
  misf->computeFiltering();
}

//======================================================
void Grip::firstNodesPlacement() {
  node n1 = misf->ordering[0];
  node n2 = misf->ordering[1];
  node n3 = misf->ordering[2];

  float d12 = getDist(currentGraph, n1, n2);
  float d13 = getDist(currentGraph, n1, n3);
  float d23 = getDist(currentGraph, n2, n3);

  result->setNodeValue(n1, Coord(0, 0, 0));
  result->setNodeValue(n2, Coord(d12, 0, 0));

  float x3 = (d13 * d13 - d23 * d23 + d12 * d12) / (d12 * 2.);
  float y3 = sqrt(d13 * d13 - x3 * x3);
  result->setNodeValue(n3, Coord(x3, y3, 0));

  if (_dim == 2) {
    oldDisp[n1] = Coord(1., 0, 0);
    oldDisp[n2] = Coord(d12 + 1., 0, 0);
    oldDisp[n3] = Coord(x3 + 1., y3, 0);
  } else {
    Graph *g = currentGraph->addSubGraph();
    g->addNode(n1);
    g->addNode(n2);
    g->addNode(n3);
    result->rotateX(3.14159 / 2. - (3.14159 * randomInteger(1)), g->getNodes(), g->getEdges());
    currentGraph->delSubGraph(g);

    const Coord &c1 = result->getNodeValue(n1);
    const Coord &c2 = result->getNodeValue(n2);
    const Coord &c3 = result->getNodeValue(n3);
    oldDisp[n1] = c1;
    oldDisp[n2] = c2;
    oldDisp[n3] = c3;
  }

  neighbors[n1].push_back(n2);
  neighbors[n1].push_back(n3);
  neighbors_dist[n1].push_back(uint(d12));
  neighbors_dist[n1].push_back(uint(d13));
  neighbors[n2].push_back(n1);
  neighbors[n2].push_back(n3);
  neighbors_dist[n2].push_back(uint(d12));
  neighbors_dist[n2].push_back(uint(d23));
  neighbors[n3].push_back(n1);
  neighbors[n3].push_back(n2);
  neighbors_dist[n3].push_back(uint(d13));
  neighbors_dist[n3].push_back(uint(d23));
}
//======================================================
void Grip::placement() {
  uint misf_size = misf->index.size();

  if (misf_size == 1) {
    initialPlacement(misf->index[0], misf->ordering.size() - 1);
    fr_reffinement(0, misf->ordering.size() - 1);
    return;
  }

  for (uint i = 0; i < misf_size - 1; ++i) {
    initialPlacement(misf->index[i], misf->index[i + 1] - 1);
    kk_reffinement(0, misf->index[i + 1] - 1);
    init_heat(misf->index[i + 1] - 1);
    ++level;
  }

  initialPlacement(misf->index[misf->index.size() - 1], misf->ordering.size() - 1);
  fr_reffinement(0, misf->ordering.size() - 1);
}
//======================================================
void Grip::seeLayout(uint end) {
  cerr << "profondeur " << level << endl;

  for (uint i = 0; i <= end; ++i) {
    node n = misf->ordering[i];

    for (uint j = 0; j < neighbors[n].size(); ++j) {
      cerr << "distance euclidienne "
           << (result->getNodeValue(n) - result->getNodeValue(neighbors[n][j])).norm() / edgeLength
           << " et distance dans le graphe " << neighbors_dist[n][j] << endl;
    }
  }
}
//======================================================
void Grip::initialPlacement(uint start, uint end) {
  for (uint i = start; i <= end; ++i) {
    node currNode = misf->ordering[i];
    misf->getNearest(currNode, neighbors[currNode], neighbors_dist[currNode], level,
                     levelToNbNeighbors[level + 1]);
  }

  for (uint i = start; i <= end; ++i) {
    node currNode = misf->ordering[i];
    Coord c_tmp;
    float nbConsidered = 0.;

    for (uint j = 0; j < neighbors[currNode].size(); ++j) {
      c_tmp += result->getNodeValue(neighbors[currNode][j]);
      oldDisp[currNode] += oldDisp[neighbors[currNode][j]];
      nbConsidered += 1.;
    }

    double alpha = edgeLength / 6.0 * randomDouble();
    Coord alea =
        Coord(alpha - (2. * alpha * randomInteger(1)), alpha - (2. * alpha * randomInteger(1)),
              (alpha - (2. * alpha * randomInteger(1))));

    if (_dim == 2) {
      alea[2] = 0.;
    }

    c_tmp /= nbConsidered;
    oldDisp[currNode] /= nbConsidered;
    oldDisp[currNode] += alea;
    c_tmp += alea;
    result->setNodeValue(currNode, c_tmp);
    heat[currNode] = edgeLength / 6.0;
    kk_local_reffinement(currNode);
  }
}

//======================================================
void Grip::kk_local_reffinement(node currNode) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  uint cpt = 6;

  while (cpt > 1) {
    disp[currNode] = Coord(0, 0, 0);
    const Coord &c = result->getNodeValue(currNode);

    for (uint j = 0; j < neighbors[currNode].size() /*&& j < 3*/; ++j) {
      node n = neighbors[currNode][j];
      const Coord &c_n = result->getNodeValue(n);
      Coord c_tmp = c_n - c;
      float euclidian_dist_sqr = c_tmp[0] * c_tmp[0] + c_tmp[1] * c_tmp[1];

      if (_dim == 3) {
        euclidian_dist_sqr += c_tmp[2] * c_tmp[2];
      }

      float th_dist = neighbors_dist[currNode][j];
      c_tmp *= (euclidian_dist_sqr / (th_dist * th_dist * edgeLength * edgeLength)) - 1.;
      disp[currNode] += c_tmp;
    }

    displace(currNode);
    --cpt;
  }
}
//======================================================
void Grip::displace(node n) {
  updateLocalTemp(n);
  float disp_norm = disp[n].norm();

  if (disp_norm > 1E-4) {
    disp[n] /= disp_norm;
    oldDisp[n] = disp[n];
    disp[n] *= float(heat[n]);
    result->setNodeValue(n, result->getNodeValue(n) + disp[n]);
  }
}
//======================================================
void Grip::kk_reffinement(uint start, uint end) {
  // cerr << __PRETTY_FUNCTION__ << endl;
  uint cpt = rounds(end, 0, 20, currentGraph->numberOfNodes(), 30) + 2;

  while (cpt >= 1) {
    for (uint i = start; i <= end; ++i) {
      node currNode = misf->ordering[i];
      disp[currNode] = Coord(0, 0, 0);
      const Coord &c = result->getNodeValue(currNode);

      for (uint j = 0; j < neighbors[currNode].size(); ++j) {
        node n = neighbors[currNode][j];
        const Coord &c_n = result->getNodeValue(n);
        Coord c_tmp = c_n - c;
        float euclidian_dist_sqr = c_tmp[0] * c_tmp[0] + c_tmp[1] * c_tmp[1];

        if (_dim == 3) {
          euclidian_dist_sqr += c_tmp[2] * c_tmp[2];
        }

        float th_dist = neighbors_dist[currNode][j];
        c_tmp *= (euclidian_dist_sqr / (th_dist * th_dist * edgeLength * edgeLength)) - 1.;
        disp[currNode] += c_tmp;
      }
    }

    // update node position
    for (uint i = 0; i <= end; ++i) {
      displace(misf->ordering[i]);
    }

    --cpt;
  }
}
//======================================================
void Grip::fr_reffinement(uint start, uint end) {
  // cerr << __PRETTY_FUNCTION__ << endl;

  uint cpt = rounds(end, 0, 20, currentGraph->numberOfNodes(), 30) + 2;

  while (cpt >= 1) {
    for (uint i = start; i <= end; ++i) {
      node currNode = misf->ordering[i];
      const Coord &curCoord = result->getNodeValue(currNode);
      disp[currNode] = Coord(0, 0, 0);

      // attractive force calculation
      for (auto n : currentGraph->getInOutNodes(currNode)) {
        const Coord &c_n = result->getNodeValue(n);
        Coord c_tmp = c_n - curCoord;
        float euclidian_dist_sqr = c_tmp[0] * c_tmp[0] + c_tmp[1] * c_tmp[1];

        if (_dim == 3) {
          euclidian_dist_sqr += c_tmp[2] * c_tmp[2];
        }

        c_tmp *= euclidian_dist_sqr / (edgeLength * edgeLength);
        disp[currNode] += c_tmp;
      }

      // repulsive force calculation
      for (uint j = 0; j < neighbors[currNode].size(); ++j) {
        node n = neighbors[currNode][j];
        const Coord &c_n = result->getNodeValue(n);
        Coord c_tmp = curCoord - c_n;
        double euclidian_dist_sqr =
            double(c_tmp[0]) * double(c_tmp[0]) + double(c_tmp[1]) * double(c_tmp[1]);

        if (_dim == 3) {
          euclidian_dist_sqr += c_tmp[2] * c_tmp[2];
        }

        if (!(euclidian_dist_sqr > 1E-4)) {
          double alpha = randomDouble(2.0);
          c_tmp = Coord(alpha - (2. * alpha * randomInteger(1)),
                        alpha - (2. * alpha * randomInteger(1)),
                        alpha - (2. * alpha * randomInteger(1)));

          if (_dim == 2) {
            c_tmp[2] = 0.;
          }

          euclidian_dist_sqr = 0.01;
        }

        c_tmp *= (0.05f * edgeLength * edgeLength) / float(euclidian_dist_sqr);
        disp[currNode] += c_tmp;
      }
    }

    // update node position
    for (uint i = 0; i <= end; ++i) {
      displace(misf->ordering[i]);
    }

    --cpt;
  }
}

//======================================================
void Grip::updateLocalTemp(node v) {
  // cerr << __PRETTY_FUNCTION__ << endl;
  float oldDisp_norm = oldDisp[v].norm();
  float curDisp_norm = disp[v].norm();

  if (curDisp_norm * oldDisp_norm > 1E-4) {

    double scalar = disp[v].dotProduct(oldDisp[v]);
    double cos = scalar / (curDisp_norm * oldDisp_norm);

    Coord tmp1 = oldDisp[v] / oldDisp_norm;
    Coord tmp2 = disp[v] / curDisp_norm;

    double sin = (tmp2 ^ tmp1).norm();

    double r = 6.;
    double o = 6.0;

    heat[v] += cos * r * heat[v];
    /*
    heat[v] = std::max(heat[v], 0.001);
    heat[v] = std::min(heat[v], 2. * edgeLength/6.0);
    */
    heat[v] += sin * o * heat[v];

    heat[v] = std::max(heat[v], edgeLength / 300.);
    heat[v] = std::min(heat[v], edgeLength / 4.0);
  }
}
//======================================================
uint Grip::rounds(uint x, uint max, uint maxVal, uint min, uint minVal) {
  if (x <= max) {
    return maxVal;
  } else if (max <= x && x <= min) {
    double k = -log(minVal / double(maxVal)) / min;
    return uint(ceil(maxVal * exp(-k * x)));
  } else {
    return minVal;
  }
}
//======================================================
void Grip::init() {
  // cerr << __PRETTY_FUNCTION__ << endl;
  set_nbr_size();
  edgeLength = 32.;
  level = 0;

  double diam = sqrt(currentGraph->numberOfNodes());
  for (auto n : currentGraph->nodes()) {
    Coord alea = Coord(diam - (2. * diam * randomInteger(1)), diam - (2. * diam * randomInteger(1)),
                       diam - (2. * diam * randomInteger(1)));

    if (_dim == 2) {
      alea[2] = 0.;
    }

    result->setNodeValue(n, alea);
    disp[n] = Coord(0, 0, 0);
    oldDisp[n] = Coord(0, 0, 0);
    heat[n] = edgeLength / 6.;
  }
}
//======================================================
void Grip::init_heat(uint end) {

  for (uint i = 0; i <= end; ++i) {
    heat[misf->ordering[i]] = edgeLength / 6.;
  }
}
//======================================================
void Grip::set_nbr_size() {
  uint maxCxty = 0;
  int initCxty = 10000;
  uint maxLevel = 0;

  for (auto n : currentGraph->nodes()) {
    maxCxty += currentGraph->deg(n);
  }

  if (maxCxty < uint(initCxty)) {
    maxCxty = initCxty;
  }

  for (uint i = 1; i < misf->index.size(); ++i) {
    if (int(misf->index[i] * misf->index[i]) - initCxty >= 0) {
      maxLevel = i;
      break;
    }
  }

  if (maxLevel == 0 &&
      int(currentGraph->numberOfNodes() * currentGraph->numberOfNodes()) - initCxty >= 0) {
    maxLevel = misf->index.size();
  }

  for (uint i = 1; i < misf->index.size(); ++i) {
    if (i >= maxLevel) {
      levelToNbNeighbors[i] =
          min(uint(sched(misf->index.size() - i, 0, 2, 10000, 1) * maxCxty / (misf->index[i])),
              uint(misf->index[i] - 1));
    } else {
      levelToNbNeighbors[i] = max(misf->index[i] - 1, 3u);
    }
  }

  if (misf->index.size() >= maxLevel) {
    levelToNbNeighbors[misf->index.size()] =
        min(uint(sched(currentGraph->numberOfNodes(), 0, 2, 10000, 1) * maxCxty /
                 (float(currentGraph->numberOfNodes()))),
            uint(currentGraph->numberOfNodes() - 1));
  } else {
    levelToNbNeighbors[misf->index.size()] = max(currentGraph->numberOfNodes() - 1, 3u);
  }

  levelToNbNeighbors[misf->index.size()] =
      min(2 * levelToNbNeighbors[misf->index.size()], currentGraph->numberOfNodes() - 1);
}
//======================================================
float Grip::sched(int x, int max, int maxVal, int min, int minVal) {
  if (x <= max) {
    return maxVal;
  } else if (max <= x && x <= min) {
    return ((minVal - maxVal) / float(min - max)) * (x - max) + maxVal;
  } else {
    return minVal;
  }
}
//======================================================
PLUGIN(Grip)
