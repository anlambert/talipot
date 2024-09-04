/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <queue>
#include <talipot/PluginHeaders.h>
#include <talipot/SortIterator.h>

using namespace tlp;
using namespace std;

/** \file
 * \brief  An implementation of the MCL clustering algorithm
 *
 * This plugin is an implementation of the MCL algorithm
 * first published as:
 *
 * Stijn van Dongen \n
 * PhD Thesis "Graph Clustering by Flow Simulation", \n
 * University of Utrecht,\n
 * 2000. \n
 *
 * <b> HISTORY</b>
 *
 * - 16/09/2011 Version 1.0: Initial release
 *
 * \author David Auber, Labri, Email : auber@labri.fr
 *
 *
 **/
class MCLClustering : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION(
      "MCL Clustering", "D. Auber & R. Bourqui", "10/10/2005",
      "Nodes partitioning measure of Markov Cluster algorithm<br/>used for community detection."
      "This is an implementation of the MCL algorithm first published as:<br/>"
      "<b>Graph Clustering by Flow Simulation</b>, Stijn van Dongen PhD Thesis, University of "
      "Utrecht (2000).",
      "1.0", "Clustering")

  MCLClustering(const tlp::PluginContext *);
  ~MCLClustering() override;
  bool run() override;
  bool inflate(double r, uint k, node n, bool equal /*, bool noprune*/);
  void prune(node n);
  // void pruneT(node n);

  bool equal();
  void init();
  void power(node n);

  unique_ptr<Graph> g;
  EdgeVectorProperty<double> inW, outW;
  NumericProperty *weights;
  double _r;
  uint _k;
};

const double epsilon = 1E-9;

//=================================================
void MCLClustering::power(node n) {
  std::unordered_map<node, double> newTargets;

  for (auto e1 : g->getOutEdges(n)) {
    double v1 = inW[e1];

    if (v1 > epsilon) {
      for (auto e2 : g->getOutEdges(g->target(e1))) {
        double v2 = inW[e2] * v1;

        if (v2 > epsilon) {
          node tgt = g->target(e2);
          edge ne = g->existEdge(n, tgt, true);

          if (ne.isValid()) {
            outW[ne] += v2;
          } else {
            auto it = newTargets.find(tgt);

            if (it != newTargets.end()) {
              // newTargets[tgt] += v2;
              it->second += v2;
            } else {
              newTargets[tgt] = v2;
            }
          }
        }
      }
    }
  }

  for (const auto &it : newTargets) {
    edge ne;
    ne = g->addEdge(n, it.first);
    inW[ne] = 0.;
    outW[ne] = it.second;
  }
}
//==================================================
struct pvectCmp {
  bool operator()(const std::pair<double, edge> &p1, const std::pair<double, edge> &p2) {
    return p1.first < p2.first;
  }
};

void MCLClustering::prune(node n) {
  uint outdeg = g->outdeg(n);

  if (outdeg == 0) {
    return;
  }

  // we use a specific vector to hold out edges needed info
  // in order to
  // - improve the locality of reference
  // - ease the sort of the out edges according to their outW value
  // - avoid a costly stable iteration when deleting edges
  std::vector<pair<double, edge>> pvect;
  pvect.reserve(outdeg);
  for (auto e : g->getOutEdges(n)) {
    pvect.push_back(pair<double, edge>(outW[e], e));
  }

  std::sort(pvect.begin(), pvect.end(), pvectCmp());
  double t = pvect[outdeg - 1].first;

  for (uint i = 0; i < outdeg; ++i) {
    pair<double, edge> p = pvect[i];

    if (p.first < t || inW[p.second] < epsilon) {
      g->delEdge(p.second);
    }
  }
}
//=================================================
bool MCLClustering::inflate(double r, uint k, node n, bool equal
                            /*, bool noprune */) {
  uint sz = g->outdeg(n);
  // we use a specific vector to hold out edges needed info
  // in order to
  // - improve the locality of reference
  // - ease the sort of the out edges according to their outW value
  // - avoid a costly stable iteration when deleting edges
  std::vector<pair<double, edge>> pvect;
  pvect.reserve(sz);

  double sum = 0.;
  for (auto e : g->getOutEdges(n)) {
    double outVal = outW[e];
    sum += pow(outVal, r);
    pvect.push_back(pair<double, edge>(outVal, e));
  }

  if (sum > 0.) {
    double oos = 1. / sum;

    for (uint i = 0; i < sz; ++i) {
      pair<double, edge> &p = pvect[i];
      p.first = outW[p.second] = pow(p.first, r) * oos;
    }
  }

  /*if (noprune)
    return;*/

  // pruneK step
  std::sort(pvect.begin(), pvect.end(), pvectCmp());
  double t = pvect[sz - 1].first;
  --k;
  uint outdeg = sz;

  for (int i = sz - 2; i > 0; --i) {
    pair<double, edge> &p = pvect[i];

    if (k) {
      if (p.first < t) {
        --k;
        t = p.first;
      }
    } else if (p.first < t) {
      edge e = p.second;
      inW[e] = 0.;
      outW[e] = 0.;
      g->delEdge(e);
      // put an invalid edge
      // to avoid any further computation
      // for this pvect elt
      p.second = edge();
      --outdeg;
    }
  }

  // makeStoc step
  sum = 0.;

  for (uint i = 0; i < sz; ++i) {
    pair<double, edge> &p = pvect[i];

    if (p.second.isValid()) {
      sum += p.first;
    }
  }

  if (sum > 0.) {
    double oos = 1. / sum;

    for (uint i = 0; i < sz; ++i) {
      pair<double, edge> &p = pvect[i];
      edge e = p.second;

      if (e.isValid()) {
        double outVal = outW[e] = p.first * oos;

        if (equal && (fabs(outVal - inW[e]) > epsilon)) {
          // more iteration needed
          equal = false;
        }
      }
    }
  } else {
    double ood = 1. / outdeg;

    for (uint i = 0; i < sz; ++i) {
      edge e = pvect[i].second;

      if (e.isValid()) {
        double outVal = outW[e] = ood;

        if (equal && (fabs(outVal - inW[e]) > epsilon)) {
          // more iteration needed
          equal = false;
        }
      }
    }
  }

  return equal;
}
//=================================================
static constexpr std::string_view paramHelp[] = {
    // inflate
    "Determines the random walk length at each step.",

    // weights
    "Edge weights to use.",

    // pruning
    "Determines, for each node, the number of strongest link kept at each iteration."};
//=================================================
MCLClustering::MCLClustering(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), g(tlp::newGraph()), weights(nullptr), _r(2.0), _k(5) {
  addInParameter<double>("inflate", paramHelp[0].data(), "2.", false);
  addInParameter<NumericProperty *>("weights", paramHelp[1].data(), "", false);
  addInParameter<uint>("pruning", paramHelp[2].data(), "5", false);
}
//===================================================================================
MCLClustering::~MCLClustering() = default;
//================================================================================
struct DegreeSort {
  DegreeSort(Graph *g) : g(g) {}
  bool operator()(node a, node b) const {
    uint da = g->deg(a), db = g->deg(b);

    if (da == db) {
      return a.id > b.id;
    }

    return da > db;
  }
  Graph *g;
};
//==============================================================================
bool MCLClustering::run() {

  weights = nullptr;
  _r = 2.;
  _k = 5;

  if (dataSet != nullptr) {
    dataSet->get("weights", weights);
    dataSet->get("inflate", _r);
    dataSet->get("pruning", _k);
  }

  NodeVectorProperty<node> nodeMapping(graph);
  g->clear();
  g->reserveNodes(graph->numberOfNodes());

  // add nodes to g
  uint nbEdges = 0;
  for (auto n : graph->nodes()) {
    nodeMapping[n] = g->addNode();
    nbEdges += 2 * graph->deg(n) + 1;
  }

  NodeVectorProperty<node> inverseNodeMapping(g.get());
  TLP_PARALLEL_MAP_NODES(graph, [&](const node n) { inverseNodeMapping[nodeMapping[n]] = n; });

  inW.alloc(g.get(), nbEdges);
  outW.alloc(g.get(), nbEdges);

  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);
    edge tmp = g->addEdge(nodeMapping[src], nodeMapping[tgt]);

    double weight = (weights != nullptr) ? weights->getEdgeDoubleValue(e) : 1.0;
    inW[tmp] = weight;
    outW[tmp] = 0.;
    // add reverse edge
    tmp = g->addEdge(nodeMapping[tgt], nodeMapping[src]);
    inW[tmp] = weight;
    outW[tmp] = 0.;
  }

  // add loops (Set the maximum of out-edges weights to self-loops weight)
  for (auto n : g->nodes()) {
    edge tmp = g->addEdge(n, n);
    double sum = 0.;
    outW[tmp] = 0.;

    if (weights != nullptr) {
      double tmpVal = inW[tmp] = 0.;
      for (auto e : g->getOutEdges(n)) {
        double eVal = inW[e];
        sum += eVal;

        if (eVal > tmpVal) {
          tmpVal = eVal;
        }
      }
      sum += (inW[tmp] = tmpVal);
    } else {
      inW[tmp] = 1.;
      sum = double(g->outdeg(n));
    }

    double oos = 1. / sum;
    for (auto e : g->getOutEdges(n)) {
      inW[e] *= oos;
    }
  }

  int iteration = 15. * log1p(g->numberOfNodes());

  while (iteration-- > 0) {
    bool equal = true;

    for (auto n : g->nodes()) {
      power(n);

      // comment the next line to have exact MCL
      if (inflate(_r, _k, n, equal /*, false*/) == false) {
        equal = false;
      }
    }

    /* exact MCL should inflate after because we share the same graphs structure,
     * or we should only remove edges created during the power and delay the
     * deletion of edge that does exist in the previous graph
     * however that impletenation doesn't change the result too much.
     */
    // uncomment that block to have correct MCL

    //        for(const node &n : g->nodes()) {
    //            inflate(_r, _k,  n, false);
    //        }

    inW.swap(outW);

    if (equal) {
      break;
    }

    outW.setAll(0.);
  }

  outW = inW;

  for (auto n : g->nodes()) {
    prune(n);
  }

  // sort nodes in decreasing order of their degree
  DegreeSort sortFunc(g.get());

  NodeVectorProperty<bool> visited(g.get());
  visited.setAll(false);

  double curVal = 0.;

  // connected component loop
  // set the same value to all connected nodes
  for (auto n : sortIterator(g->nodes(), sortFunc)) {
    if (!visited[n]) {
      queue<node> fifo;
      fifo.push(n);
      visited[n] = true;

      while (!fifo.empty()) {
        node nq = fifo.front();
        result->setNodeValue(inverseNodeMapping[nq], curVal);
        fifo.pop();
        for (auto ni : g->getInOutNodes(nq)) {
          if (!visited[ni]) {
            fifo.push(ni);
            visited[ni] = true;
          }
        }
      }

      curVal += 1.;
    }
  }

  return true;
}
//==============================================================================
PLUGIN(MCLClustering)
