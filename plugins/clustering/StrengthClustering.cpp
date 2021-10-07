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

#include "StrengthClustering.h"

using namespace std;
using namespace tlp;

PLUGIN(StrengthClustering)

//================================================================================
StrengthClustering::~StrengthClustering() = default;
//==============================================================================
double StrengthClustering::computeMQValue(const vector<unordered_set<node>> &partition, Graph *sg) {

  vector<uint> nbIntraEdges(partition.size(), 0);

  map<pair<uint, uint>, uint> nbExtraEdges;

  MutableContainer<uint> clusterId;

  uint i = 0;
  for (const auto &p : partition) {
    for (auto n : p) {
      clusterId.set(n.id, i);
    }
    ++i;
  }

  for (auto e : sg->edges()) {
    auto [src, tgt] = sg->ends(e);

    if (src.id >= tgt.id) {
      std::swap(src, tgt);
    }

    uint srcClustId = clusterId.get(src.id);
    uint tgtClustId = clusterId.get(tgt.id);

    if (srcClustId == tgtClustId) {
      nbIntraEdges[srcClustId] += 1;
    } else {
      pair pp = {srcClustId, tgtClustId};

      if (nbExtraEdges.find(pp) != nbExtraEdges.end()) {
        nbExtraEdges[pp] += 1;
      } else {
        nbExtraEdges[pp] = 1;
      }
    }
  }

  double positive = 0;

  for (uint i = 0; i < partition.size(); ++i) {
    if (partition[i].size() > 1) {
      positive +=
          2.0 * double(nbIntraEdges[i]) / double(partition[i].size() * (partition[i].size() - 1));
    }
  }

  positive /= double(partition.size());

  double negative = 0;

  for (const auto &[pp, val] : nbExtraEdges) {
    if (!partition[pp.first].empty() && !partition[pp.second].empty()) {
      negative += double(val) / double(partition[pp.first].size() * partition[pp.second].size());
    }
  }

  if (partition.size() > 1) {
    negative /= double(partition.size() * (partition.size() - 1)) / 2.0;
  }

  double result = positive - negative;
  return result;
}

//==============================================================================
void StrengthClustering::computeNodePartition(double threshold,
                                              vector<unordered_set<node>> &result) {
  Graph *tmpGraph = graph->addCloneSubGraph();

  for (auto e : graph->edges()) {
    if (values->getEdgeValue(e) < threshold) {
      const auto &[src, tgt] = graph->ends(e);

      if (graph->deg(src) > 1 && graph->deg(tgt) > 1) {
        tmpGraph->delEdge(e);
      }
    }
  }

  // Select SubGraph singleton in graph
  unordered_set<node> singleton;

  for (auto n : tmpGraph->nodes()) {
    if (tmpGraph->deg(n) == 0) {
      singleton.insert(n);
    }
  }

  // restore edges to reconnect singleton by computing induced subgraph
  for (auto e : graph->edges()) {
    const auto &[src, tgt] = graph->ends(e);

    if (singleton.find(src) != singleton.end() && singleton.find(tgt) != singleton.end()) {
      tmpGraph->addEdge(e);
    }
  }

  // Extract connected component
  DoubleProperty connected(tmpGraph);
  string errMsg;
  tmpGraph->applyPropertyAlgorithm("Connected Components", &connected, errMsg);

  // Compute the node partition
  int index = 0;
  unordered_map<double, int> resultIndex;

  for (auto n : tmpGraph->nodes()) {
    double val = connected.getNodeValue(n);

    if (resultIndex.find(val) != resultIndex.end()) {
      result[resultIndex[val]].insert(n);
    } else {
      unordered_set<node> tmp;
      result.push_back(tmp);
      resultIndex[val] = index;
      result[index].insert(n);
      ++index;
    }
  }

  graph->delAllSubGraphs(tmpGraph);
}
//==============================================================================
double StrengthClustering::findBestThreshold(int numberOfSteps, bool &stopped) {
  double maxMQ = -2;
  double threshold = values->getEdgeMin(graph);
  double deltaThreshold =
      (values->getEdgeMax(graph) - values->getEdgeMin(graph)) / double(numberOfSteps);
  int steps = 0;

  for (double i = values->getEdgeMin(graph); i < values->getEdgeMax(graph); i += deltaThreshold) {
    vector<unordered_set<node>> tmp;
    computeNodePartition(i, tmp);

    if (pluginProgress && ((++steps % (numberOfSteps / 10)) == 0)) {
      pluginProgress->progress(steps, numberOfSteps);

      if ((stopped = (pluginProgress->state() != TLP_CONTINUE))) {
        return threshold;
      }
    }

    double mq = computeMQValue(tmp, graph);

    if (mq > maxMQ) {
      threshold = i;
      maxMQ = mq;
    }
  }

  return threshold;
}
//==============================================================================
static constexpr std::string_view paramHelp[] = {
    // metric
    "Metric used in order to multiply strength metric computed values."
    "If one is given, the complexity is O(n log(n)), O(n) neither."
    // do you mean "else it will be O(n)" instead of "O(n) neither"?
};

//================================================================================
StrengthClustering::StrengthClustering(PluginContext *context) : DoubleAlgorithm(context) {
  addInParameter<NumericProperty *>("metric", paramHelp[0].data(), "", false);
  addDependency("Strength", "1.0");
}

//==============================================================================
bool StrengthClustering::run() {
  string errMsg;
  values = new DoubleProperty(graph);

  if (!graph->applyPropertyAlgorithm("Strength", values, errMsg, nullptr, pluginProgress)) {
    return false;
  }

  NumericProperty *metric = nullptr;

  if (dataSet) {
    dataSet->get("metric", metric);
  }

  if (metric) {
    NumericProperty *mult = metric->copyProperty(graph);

    if (pluginProgress) {
      pluginProgress->setComment("Computing Strength metric X specified metric on edges ...");
    }

    mult->uniformQuantification(100);
    uint steps = 0, maxSteps = graph->numberOfEdges();

    if (maxSteps < 10) {
      maxSteps = 10;
    }

    for (auto e : graph->edges()) {
      values->setEdgeValue(e, values->getEdgeValue(e) * (mult->getEdgeDoubleValue(e) + 1));

      if (pluginProgress && ((++steps % (maxSteps / 10) == 0))) {
        pluginProgress->progress(steps, maxSteps);

        if (pluginProgress->state() != TLP_CONTINUE) {
          return pluginProgress->state() != TLP_CANCEL;
        }
      }
    }
    delete mult;
  }

  bool stopped = false;
  const uint NB_TEST = 100;

  if (pluginProgress) {
    pluginProgress->setComment("Partitioning nodes...");
    pluginProgress->progress(0, NB_TEST + 1);
  }

  double threshold = findBestThreshold(NB_TEST, stopped);

  if (stopped) {
    return pluginProgress->state() != TLP_CANCEL;
  }

  vector<unordered_set<node>> tmp;
  computeNodePartition(threshold, tmp);

  for (uint i = 0; i < tmp.size(); ++i) {
    for (auto n : tmp[i]) {
      result->setNodeValue(n, i);
    }
  }

  delete values;
  return true;
}
//================================================================================
bool StrengthClustering::check(string &erreurMsg) {
  if (!SimpleTest::isSimple(graph)) {
    erreurMsg = "The graph must be simple";
    return false;
  }
  return true;
}
