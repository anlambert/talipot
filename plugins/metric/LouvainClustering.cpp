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

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;

/** \file
 * \brief  An implementation of the Louvain clustering algorithm
 *
 * This plugin is an implementation of the Louvain clustering algorithm
 * first published as:
 *
 * Blondel, V.D. and Guillaume, J.L. and Lambiotte, R. and Lefebvre, E., \n
 * "Fast unfolding of communities in large networks", \n
 * "Journal of Statistical Mechanics: Theory and Experiment, P10008",\n
 * 2008. \n
 *
 * <b> HISTORY</b>
 *
 * - 25/02/2011 Version 1.0: Initial release (François Queyroi)
 * - 13/05/2011 Version 2.0 (Bruno Pinaud): Change plugin type from General Algorithm to
 *DoubleAlgorithm, code cleaning and fix some memory leaks.
 * - 09/06/2015 Version 2.1 (Patrick Mary) full rewrite according the updated version of the
 *original source code available at  https://sites.google.com/site/findcommunities/
 *
 * \note A threshold for modularity improvement is used here, its value is 0.000001
 *
 * \author Patrick Mary, Labri
 *
 *
 **/
class LouvainClustering : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION(
      "Louvain", "Patrick Mary", "09/06/15",
      "Nodes partitioning measure used for community detection."
      "This is an implementation of the Louvain clustering algorithm first published as:<br/>"
      "<b>Fast unfolding of communities in large networks</b>, Blondel, V.D. and Guillaume, J.L. "
      "and Lambiotte, R. and Lefebvre, E., Journal of Statistical Mechanics: Theory and "
      "Experiment, P10008 (2008).",
      "2.1", "Clustering")
  LouvainClustering(const tlp::PluginContext *);
  bool run() override;

private:
  // the number of nodes of the original graph
  uint nb_nodes;

  // a quotient graph of the original graph
  Graph *quotient;
  // number of nodes in the quotient graph and size of all vectors
  uint nb_qnodes;

  // the mapping between the nodes of the original graph
  // and the quotient nodes
  NodeVectorProperty<int> *clusters;

  // quotient graph edge weights
  EdgeVectorProperty<double> *weights;
  // total weight (sum of edge weights for the quotient graph)
  double total_weight;
  // 1./total_weight
  double ootw;

  std::vector<double> neigh_weight;
  std::vector<uint> neigh_pos;
  uint neigh_last;

  // community to which each node belongs
  std::vector<uint> n2c;
  // used to renumber the communities
  std::vector<int> renumber;
  // used to compute the modularity participation of each community
  std::vector<double> in, tot;

  // a new pass is computed if the last one has generated an increase
  // greater than min_modularity
  // if 0. even a minor increase is enough to go for one more pass
  double min_modularity;
  double new_mod;

  // return the weighted degree and selfloops of a node
  // of the current quotient graph
  void get_weighted_degree_and_selfloops(uint n, double &wdg, double &nsl) {
    wdg = nsl = 0;
    const std::vector<edge> &edges = quotient->incidence(node(n));

    for (uint i = 0; i < edges.size(); ++i) {
      edge e = edges[i];
      double weight = (*weights)[e];
      wdg += weight;
      // self loop must be counted only once
      const auto &[src, tgt] = quotient->ends(e);

      if (src == tgt) {
        nsl = weight;
        ++i;
      }
    }
  }

  // compute the gain of modularity if node where inserted in comm
  // given that node has dnodecomm links to comm.  The formula is:
  // [(In(comm)+2d(node,comm))/2m - ((tot(comm)+deg(node))/2m)^2]-
  // [In(comm)/2m - (tot(comm)/2m)^2 - (deg(node)/2m)^2]
  // where In(comm)    = number of half-links strictly inside comm
  //       Tot(comm)   = number of half-links inside or outside comm (sum(degrees))
  //       d(node,com) = number of links from node to comm
  //       deg(node)   = node degree
  //       m           = number of links
  inline double modularity_gain(uint /*node*/, uint comm, double dnode_comm, double w_degree) {
    return (dnode_comm - tot[comm] * w_degree * ootw);
  }

  // compute the modularity of the current partition
  double modularity() {
    double q = 0.;

    for (uint i = 0; i < nb_qnodes; i++) {
      if (tot[i] > 0) {
        q += ootw * (in[i] - tot[i] * tot[i] * ootw);
      }
    }

    return q;
  }

  // compute the set of neighboring communities of node
  // for each community, gives the number of links from node to comm
  void neigh_comm(uint n) {
    for (uint i = 0; i < neigh_last; i++) {
      neigh_weight[neigh_pos[i]] = -1;
    }

    neigh_last = 0;

    neigh_pos[0] = n2c[n];
    neigh_weight[neigh_pos[0]] = 0;
    neigh_last = 1;

    for (auto e : quotient->incidence(node(n))) {
      const auto &[src, tgt] = quotient->ends(e);
      uint neigh = (src == node(n)) ? tgt : src;
      uint neigh_comm = n2c[neigh];
      double neigh_w = (*weights)[e];

      if (neigh != n) {
        if (neigh_weight[neigh_comm] == -1) {
          neigh_weight[neigh_comm] = 0.;
          neigh_pos[neigh_last++] = neigh_comm;
        }

        neigh_weight[neigh_comm] += neigh_w;
      }
    }
  }

  // generates the quotient graph of communities as computed by one_level
  void partitionToQuotient(Graph *new_quotient, EdgeVectorProperty<double> *new_weights) {
    // Renumber communities
    vector<int> renumber(nb_qnodes, -1);

    for (uint n = 0; n < nb_qnodes; n++) {
      renumber[n2c[n]] = 0;
    }

    int final = 0;

    for (uint i = 0; i < nb_qnodes; i++) {
      if (renumber[i] != -1) {
        renumber[i] = final++;
      }
    }

    // update clustering
    TLP_PARALLEL_MAP_INDICES(nb_nodes,
                             [&](uint i) { (*clusters)[i] = renumber[n2c[(*clusters)[i]]]; });

    // Compute weighted graph
    new_quotient->addNodes(final);

    total_weight = 0;
    for (auto e : quotient->edges()) {
      auto [src, tgt] = quotient->ends(e);
      auto ends = quotient->ends(e);

      uint src_comm = renumber[n2c[src]];
      uint tgt_comm = renumber[n2c[tgt]];
      double weight = (*weights)[e];
      edge e_comm = new_quotient->existEdge(node(src_comm), node(tgt_comm), false);
      total_weight += weight;
      double *weight_comm = nullptr;

      if (!e_comm.isValid()) {
        ends = pair(node(src_comm), node(tgt_comm));
        e_comm = new_quotient->addEdge(ends.first, ends.second);
        weight_comm = &((*new_weights)[e_comm]);
        *weight_comm = weight;
      } else {
        ends = new_quotient->ends(e_comm);
        weight_comm = &((*new_weights)[e_comm]);

        if (ends.second == node(tgt_comm)) {
          *weight_comm += weight;
        }
      }

      // self loop are counted only once
      if (src != tgt) {
        total_weight += weight;

        if (ends.first == node(tgt_comm)) {
          *weight_comm += weight;
        }
      }
    }

    ootw = 1. / total_weight;
  }

  // compute communities of the graph for one level
  // return true if some nodes have been moved
  bool one_level() {
    bool improvement = false;
    new_mod = modularity();
    double cur_mod = new_mod;

    vector<uint> random_order(nb_qnodes);
    TLP_PARALLEL_MAP_INDICES(nb_qnodes, [&](uint i) { random_order[i] = i; });

    shuffle(random_order.begin(), random_order.end(), getRandomNumberGenerator());

    // repeat while
    // there is an improvement of modularity
    // or there is an improvement of modularity greater than a given epsilon
    do {
      cur_mod = new_mod;
      int nb_moves = 0;

      // for each node:
      // remove the node from its community
      // and insert it in the best community
      for (auto n : random_order) {
        uint n_comm = n2c[n];
        double n_wdg;
        double n_nsl;
        get_weighted_degree_and_selfloops(n, n_wdg, n_nsl);

        // computation of all neighboring communities of current node
        neigh_comm(n);

        // remove node from its current community
        tot[n_comm] -= n_wdg;
        in[n_comm] -= 2 * neigh_weight[n_comm] + n_nsl;

        // compute the nearest community for node
        // default choice for future insertion is the former community
        uint best_comm = n_comm;
        double best_nblinks = 0.;
        double best_increase = 0.;

        for (uint i = 0; i < neigh_last; i++) {
          double increase = modularity_gain(n, neigh_pos[i], neigh_weight[neigh_pos[i]], n_wdg);

          if (increase > best_increase ||
              // keep the best cluster with the minimum id
              (increase == best_increase && neigh_pos[i] > best_comm)) {
            best_nblinks = neigh_weight[neigh_pos[i]];
            best_increase = increase;
            best_comm = neigh_pos[i];
          }
        }

        // insert node in the nearest community
        tot[best_comm] += n_wdg;
        in[best_comm] += 2 * best_nblinks + n_nsl;
        n2c[n] = best_comm;

        if (best_comm != n_comm) {
          nb_moves++;
        }
      }

      new_mod = modularity();

      if (nb_moves > 0) {
        improvement = true;
      }

    } while (improvement && ((new_mod - cur_mod) > min_modularity));

    return improvement;
  }

  void init_level() {
    nb_qnodes = quotient->numberOfNodes();
    neigh_weight.resize(nb_qnodes, -1);
    neigh_pos.resize(nb_qnodes);
    neigh_last = 0;

    n2c.resize(nb_qnodes);
    in.resize(nb_qnodes);
    tot.resize(nb_qnodes);

    TLP_PARALLEL_MAP_INDICES(nb_qnodes, [&](uint i) {
      n2c[i] = i;
      double wdg, nsl;
      get_weighted_degree_and_selfloops(i, wdg, nsl);
      in[i] = nsl;
      tot[i] = wdg;
    });
  }
};

//========================================================================================
static constexpr std::string_view paramHelp[] = { // metric
    "An existing edge weight metric property. If it is not defined "
    "all edges have a weight of 1.0.",

    // precision
    "A given pass stops when the modularity is increased by less "
    "than precision. Default value is "
    "<b>0.000001</b>"};
//========================================================================================
// same precision as the original code
#define DEFAULT_PRECISION 0.000001

LouvainClustering::LouvainClustering(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), new_mod(0.) {
  addInParameter<NumericProperty *>("metric", paramHelp[0].data(), "", false);
  addInParameter<double>("precision", paramHelp[1].data(), "0.000001", false);
  addOutParameter<double>("modularity", "The computed modularity");
  addOutParameter<uint>("#communities", "The number of communities found");
}
//========================================================================================
bool LouvainClustering::run() {
  NumericProperty *metric = nullptr;
  min_modularity = DEFAULT_PRECISION;

  if (dataSet != nullptr) {
    dataSet->get("metric", metric);
    dataSet->get("precision", min_modularity);
  }

  // initialize a random sequence according the given seed
  tlp::initRandomSequence();

  nb_nodes = graph->numberOfNodes();

  quotient = tlp::newGraph();
  quotient->addNodes(nb_nodes);

  clusters = new NodeVectorProperty<int>(graph);

  TLP_PARALLEL_MAP_INDICES(nb_nodes, [&](uint i) { (*clusters)[i] = i; });

  weights = new EdgeVectorProperty<double>(quotient);
  // init total_weight, weights and quotient edges
  for (auto e : graph->edges()) {
    double weight = metric ? metric->getEdgeDoubleValue(e) : 1;
    const auto &[src, tgt] = graph->ends(e);
    node q_src((*clusters)[src.id]);
    node q_tgt((*clusters)[tgt.id]);
    // self loops are counted only once
    total_weight += q_src != q_tgt ? 2 * weight : weight;
    // create corresponding edge if needed
    edge qe = quotient->existEdge(q_src, q_tgt, false);

    if (!qe.isValid()) {
      qe = quotient->addEdge(q_src, q_tgt);
      (*weights)[qe] = weight;
    } else {
      // set current edge weight
      (*weights)[qe] += weight;
    }
  }
  ootw = 1. / total_weight;

  // init other vectors
  init_level();

  while (one_level()) {
    auto *new_quotient = tlp::newGraph();
    auto *new_weights = new EdgeVectorProperty<double>(new_quotient);

    partitionToQuotient(new_quotient, new_weights);
    delete quotient;
    delete weights;
    quotient = new_quotient;
    weights = new_weights;

    init_level();
  }

  // update measure
  // Renumber communities
  vector<int> renumber(nb_qnodes, -1);

  for (uint n = 0; n < nb_qnodes; n++) {
    renumber[n2c[n]] = 0;
  }

  int final = 0;

  for (uint i = 0; i < nb_qnodes; i++) {
    if (renumber[i] != -1) {
      renumber[i] = final++;
    }
  }

  // then set measure values
  int maxVal = -1;
  TLP_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
    int val = renumber[n2c[(*clusters)[i]]];
    result->setNodeValue(n, val);
    maxVal = std::max(val, maxVal);
  });

  delete quotient;
  delete weights;
  delete clusters;

  if (dataSet != nullptr) {
    dataSet->set("modularity", new_mod);
    dataSet->set("#communities", uint(maxVal + 1));
  }

  return true;
}
//========================================================================================
PLUGIN(LouvainClustering)
