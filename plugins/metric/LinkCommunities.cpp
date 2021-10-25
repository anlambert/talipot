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

#include <talipot/DoubleProperty.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;

/** \file
 * @brief This plugin is an implementation of a fuzzy clustering procedure. First introduced in :
 *
 * Ahn, Y.Y. and Bagrow, J.P. and Lehmann, S., \n
 * "Link communities reveal multiscale complexity in networks", \n
 * in Nature vol:466, \n
 * pages 761--764, \n
 * 2010 \n
 *
 * The result of this procedure is saved as an edge metric : two edges share the same value
 * if they are part of the same group.
 * The result for a node shows the number of groups to which it belongs.
 *
 * @note To create subgraphs using the result of this algorithm use "Equal Value" with parameter
 *Type="edges".
 *
 * @todo Deal with directed graphs.
 *
 **/
class LinkCommunities : public tlp::DoubleAlgorithm {
public:
  PLUGININFORMATION(
      "Link Communities", "François Queyroi", "25/02/11",
      "Edges partitioning measure used for community detection.<br>"
      "It is an implementation of a fuzzy clustering procedure. First introduced in :<br>"
      " <b>Link communities reveal multiscale complexity in networks</b>, Ahn, Y.Y. and Bagrow, "
      "J.P. and Lehmann, S., Nature vol:466, 761--764 (2010)",
      "1.0", "Clustering")

  LinkCommunities(const tlp::PluginContext *);
  ~LinkCommunities() override;
  bool run() override;

private:
  /**
   * @brief Create the dual of the graph
   * in order to store Similarity value between two edges
   * Edges are represented by nodes linked according to edges' neighborhood.
   **/
  void createDualGraph(const std::vector<edge> &edges);
  /**
   * @brief Compute all similarities between all pairs of adjacent edges.
   **/
  void computeSimilarities(const std::vector<edge> &edges);
  /**
   * @brief Compute similarity (Jaccard) between graph->edges()[source(e).id] and
   *graph->edges()[target(e).id]
   **/
  double getSimilarity(tlp::edge e, const std::vector<edge> &edges);
  /**
   * @brief Compute weighted (Tanimoto) similarity between graph->edges()[source(e).id] and
   *graph->edges()[target(e).id]
   **/
  double getWeightedSimilarity(tlp::edge e, const std::vector<edge> &edges);
  /**
   * @brief Perform #(step) single linkage clustering in order to find the partition
   * which maximise the average density
   **/
  double findBestThreshold(uint, const std::vector<edge> &edges);
  /**
   * @brief Compute the partition of dual node for the given threshold value
   * and return average density of this edge partition
   **/
  double computeAverageDensity(double, const std::vector<edge> &edges);
  /**
   * @brief set edge values according the partition corresponding
   * to the best threshold
   **/
  void setEdgeValues(double, bool, const std::vector<edge> &edges);

  std::unique_ptr<tlp::Graph> dual; // Dual Node -> Graph Edges; Dual Edge -> indicates that the
                                    // linked Graph Edges have a same end.
  tlp::EdgeVectorProperty<tlp::node> mapKeystone;
  tlp::EdgeVectorProperty<double> similarity;

  tlp::NumericProperty *metric;
};

//==============================================================================================================
static constexpr std::string_view paramHelp[] = {
    // metric
    "An existing edge metric property.",

    // Group isthmus
    "This parameter indicates whether the single-link clusters should be merged or not.",

    // Number of steps
    "This parameter indicates the number of thresholds to be compared."};
//==============================================================================================================
LinkCommunities::LinkCommunities(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), dual(tlp::newGraph()), metric(nullptr) {
  addInParameter<NumericProperty *>("metric", paramHelp[0].data(), "", false);
  addInParameter<bool>("Group isthmus", paramHelp[1].data(), "true", true);
  addInParameter<uint>("Number of steps", paramHelp[2].data(), "200", true);
}
//==============================================================================================================
LinkCommunities::~LinkCommunities() = default;
//==============================================================================================================
bool LinkCommunities::run() {
  metric = nullptr;
  bool group_isthmus = true;
  uint nb_steps = 200;

  if (dataSet != nullptr) {
    dataSet->get("metric", metric);
    dataSet->get("Group isthmus", group_isthmus);
    dataSet->get("Number of steps", nb_steps);
  }

  const std::vector<edge> &edges = graph->edges();
  createDualGraph(edges);

  computeSimilarities(edges);

  result->setAllNodeValue(0);
  result->setAllEdgeValue(0);
  double th = findBestThreshold(nb_steps, edges);

  setEdgeValues(th, group_isthmus, edges);

  dual->clear();
  similarity.clear();

  for (auto n : graph->nodes()) {
    std::set<double> around;
    for (auto e : graph->incidence(n)) {
      double val = result->getEdgeValue(e);

      if (val) {
        around.insert(val);
      }
    }
    result->setNodeValue(n, around.size());
  }

  return true;
}
//==============================================================================================================
void LinkCommunities::createDualGraph(const std::vector<edge> &edges) {
  uint nbEdges = edges.size();
  dual->reserveNodes(nbEdges);
  similarity.alloc(dual.get(), nbEdges);
  mapKeystone.alloc(dual.get(), nbEdges);

  for (uint i = 0; i < nbEdges; ++i) {
    node dn = dual->addNode();
    const auto &[src, tgt] = graph->ends(edges[i]);

    for (auto ee : graph->incidence(src)) {
      uint eePos = graph->edgePos(ee);

      if (eePos < i) {
        if (!dual->existEdge(dn, node(eePos), false).isValid()) {
          edge de = dual->addEdge(dn, node(eePos));
          mapKeystone[de] = src;
        }
      }
    }
    for (auto ee : graph->incidence(tgt)) {
      uint eePos = graph->edgePos(ee);

      if (eePos < i) {
        if (!dual->existEdge(dn, node(eePos), false).isValid()) {
          edge de = dual->addEdge(dn, node(eePos));
          mapKeystone[de] = tgt;
        }
      }
    }
  }
}
//==============================================================================================================
void LinkCommunities::computeSimilarities(const std::vector<edge> &edges) {
  similarity.resize(dual->numberOfEdges());
  if (metric == nullptr) {
    TLP_PARALLEL_MAP_INDICES(dual->numberOfEdges(), [&](uint i) {
      edge e = edge(i);
      similarity[e] = getSimilarity(e, edges);
    });
  } else {
    TLP_PARALLEL_MAP_INDICES(dual->numberOfEdges(), [&](uint i) {
      edge e = edge(i);
      similarity[e] = getWeightedSimilarity(e, edges);
    });
  }
}
//==============================================================================================================
double LinkCommunities::getSimilarity(edge ee, const std::vector<edge> &edges) {
  node key = mapKeystone[ee];
  auto [eeSrc, eeTgt] = dual->ends(ee);
  edge e1 = edges[eeSrc.id];
  edge e2 = edges[eeTgt.id];
  const auto &[e1Src, e1Tgt] = graph->ends(e1);
  node n1 = (e1Src != key) ? e1Src : e1Tgt;
  const auto &[e2Src, e2Tgt] = graph->ends(e2);
  node n2 = (e2Src != key) ? e2Src : e2Tgt;
  uint wuv = 0, m = 0;
  for (auto n : graph->getInOutNodes(n1)) {
    if (graph->existEdge(n2, n, true).isValid()) {
      wuv += 1;
    }

    if (graph->existEdge(n, n2, true).isValid()) {
      wuv += 1;
    }

    m += 1.0;
  }

  for (auto n : graph->getInOutNodes(n2)) {
    if (!graph->existEdge(n1, n, false).isValid()) {
      m += 1;
    }
  }

  if (graph->existEdge(n1, n2, false).isValid()) {
    wuv += 2;
  }

  if (m > 0) {
    return wuv / double(m);
  } else {
    return 0.0;
  }
}
//==============================================================================================================
double LinkCommunities::getWeightedSimilarity(tlp::edge ee, const std::vector<edge> &edges) {
  node key = mapKeystone[ee];
  auto [eeSrc, eeTgt] = dual->ends(ee);
  edge e1 = edges[eeSrc.id];
  edge e2 = edges[eeTgt.id];
  const auto &[e1Src, e1Tgt] = graph->ends(e1);
  node n1 = (e1Src != key) ? e1Src : e1Tgt;
  const auto &[e2Src, e2Tgt] = graph->ends(e2);
  node n2 = (e2Src != key) ? e2Src : e2Tgt;

  if (graph->deg(n1) > graph->deg(n2)) {
    node tmp = n1;
    n1 = n2;
    n2 = tmp;
  }

  double a1a2 = 0.0;
  double a1 = 0.0, a2 = 0.0;
  double a11 = 0.0, a22 = 0.0;
  for (auto e : graph->getInEdges(n1)) {
    double val = metric->getEdgeDoubleValue(e);
    node n = graph->source(e);
    edge me = graph->existEdge(n2, n, true);

    if (me.isValid()) {
      a1a2 += val * metric->getEdgeDoubleValue(me);
    }

    me = graph->existEdge(n, n2, true);

    if (me.isValid()) {
      a1a2 += val * metric->getEdgeDoubleValue(me);
    }

    a1 += val;
    a11 += val * val;
  }

  for (auto e : graph->getOutEdges(n1)) {
    double val = metric->getEdgeDoubleValue(e);
    node n = graph->target(e);
    edge me = graph->existEdge(n2, n, true);

    if (me.isValid()) {
      a1a2 += val * metric->getEdgeDoubleValue(me);
    }

    me = graph->existEdge(n, n2, true);

    if (me.isValid()) {
      a1a2 += val * metric->getEdgeDoubleValue(me);
    }

    a1 += val;
    a11 += val * val;
  }

  for (auto e : graph->incidence(n2)) {
    double val = metric->getEdgeDoubleValue(e);
    a2 += val;
    a22 += val * val;
  }
  a1 /= graph->deg(n1);
  a11 += a1 * a1;
  a2 /= graph->deg(n2);
  a22 += a2 * a2;

  edge e = graph->existEdge(n1, n2, false);

  if (e.isValid()) {
    a1a2 += metric->getEdgeDoubleValue(e) * (a1 + a2);
  }

  double m = a11 + a22 - a1a2;

  if (m < 0.0) {
    return 0.0;
  } else {
    return a1a2 / m;
  }
}
//==============================================================================================================

double LinkCommunities::computeAverageDensity(double threshold, const std::vector<edge> &edges) {
  double d = 0.0;
  NodeVectorProperty<bool> dn_visited(dual.get());
  dn_visited.setAll(false);

  uint sz = dual->numberOfNodes();

  for (uint i = 0; i < sz; ++i) {
    node dn = node(i);

    if (!dn_visited[dn]) {
      uint nbDNodes = 1;
      dn_visited[dn] = true;
      edge re = edges[dn.id];
      MutableContainer<bool> visited;
      uint nbNodes = 1;
      const auto &[reSrc, reTgt] = graph->ends(re);
      visited.set(reSrc.id, true);

      if (reSrc != reTgt) {
        visited.set(reTgt.id, true);
        nbNodes = 2;
      }

      list<node> dnToVisit;
      dnToVisit.push_front(dn);

      while (!dnToVisit.empty()) {
        dn = dnToVisit.front();
        dnToVisit.pop_front();

        for (auto e : dual->incidence(dn)) {
          if (similarity[e] > threshold) {
            node neighbour = dual->opposite(e, dn);

            if (!dn_visited[neighbour]) {
              dn_visited[neighbour] = true;
              dnToVisit.push_back(neighbour);
              ++nbDNodes;
              edge re = edges[neighbour.id];
              const auto &[reSrc, reTgt] = graph->ends(re);

              if (!visited.get(reSrc.id)) {
                visited.set(reSrc.id, true);
                ++nbNodes;
              }

              if (!visited.get(reTgt.id)) {
                visited.set(reTgt.id, true);
                ++nbNodes;
              }
            }
          }
        }
      }

      if (nbNodes >= 3) {
        double mc = nbDNodes;
        double nc = nbNodes;
        double density = (mc - nc + 1) / (nc * (nc - 1) / 2.0 - nc + 1);
        d += nbDNodes * density;
      }
    }
  }

  return 2.0 * d / (graph->numberOfEdges());
}
//==============================================================================================================
void LinkCommunities::setEdgeValues(double threshold, bool group_isthmus,
                                    const std::vector<edge> &edges) {
  NodeVectorProperty<bool> dn_visited(dual.get());
  dn_visited.setAll(false);

  double val = 1;
  uint sz = dual->numberOfNodes();

  for (uint i = 0; i < sz; ++i) {
    node dn = node(i);

    if (!dn_visited[dn]) {
      dn_visited[dn] = true;
      vector<node> component;
      component.push_back(dn);

      list<node> dnToVisit;
      dnToVisit.push_front(dn);

      while (!dnToVisit.empty()) {
        dn = dnToVisit.front();
        dnToVisit.pop_front();

        for (auto e : dual->incidence(dn)) {

          if (similarity[e] > threshold) {
            node neighbour = dual->opposite(e, dn);

            if (!dn_visited[neighbour]) {
              dn_visited[neighbour] = true;
              dnToVisit.push_back(neighbour);
              component.push_back(neighbour);
            }
          }
        }
      }

      if (component.size() >= 2 || !group_isthmus) {
        for (auto n : component) {
          edge re = edges[n.id];
          result->setEdgeValue(re, val);
        }
      }

      val += 1;
    }
  }
}
//==============================================================================================================
double LinkCommunities::findBestThreshold(uint numberOfSteps, const std::vector<edge> &edges) {
  double maxD = -2;
  double threshold = 0.0;

  double min = 1.1;
  double max = -1.0;

  uint sz = dual->numberOfEdges();

  for (uint i = 0; i < sz; ++i) {
    double value = similarity[edge(i)];

    if (value < min) {
      min = value;
    } else if (value > max) {
      max = value;
    }
  }

  double deltaThreshold = (max - min) / double(numberOfSteps);

  TLP_PARALLEL_MAP_INDICES(numberOfSteps, [&](uint i) {
    double step = min + i * deltaThreshold;
    double d = computeAverageDensity(step, edges);
    TLP_LOCK_SECTION(findBestThreshold) {
      if (d > maxD) {
        threshold = step;
        maxD = d;
      }
    }
    TLP_UNLOCK_SECTION(findBestThreshold);
  });

  return threshold;
}
//==============================================================================================================
PLUGIN(LinkCommunities)
