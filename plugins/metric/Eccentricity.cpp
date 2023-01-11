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

#include <algorithm>
#include <atomic>

#include <talipot/GraphMeasure.h>
#include <talipot/PropertyAlgorithm.h>

#include "Eccentricity.h"

using namespace std;
using namespace tlp;

PLUGIN(EccentricityMetric)

static constexpr std::string_view paramHelp[] = {
    // closeness centrality
    "If true, the closeness centrality is computed (i.e. the average distance from a node to all "
    "others).",

    // norm
    "If true, the returned values are normalized. "
    "For the closeness centrality, the reciprocal of the sum of distances is returned. "
    "The eccentricity values are divided by the graph diameter. "
    "<b> Warning : </b> The normalized eccentricity values should be computed on a (strongly) "
    "connected graph.",

    // directed
    "If true, the graph is considered directed.",

    // weight
    "An existing edge weight metric property.",

    // graph diameter
    "The computed graph diameter (the length of the shortest path between the most distanced "
    "nodes)."};

EccentricityMetric::EccentricityMetric(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), allPaths(false), norm(true), directed(false) {
  addInParameter<bool>("closeness centrality", paramHelp[0].data(), "false");
  addInParameter<bool>("norm", paramHelp[1].data(), "true");
  addInParameter<bool>("directed", paramHelp[2].data(), "false");
  addInParameter<NumericProperty *>("weight", paramHelp[3].data(), "", false);
  addOutParameter<double>("graph diameter", paramHelp[4].data(), "-1");
}
//====================================================================
EccentricityMetric::~EccentricityMetric() = default;
//====================================================================
double EccentricityMetric::compute(node n, NodeVectorProperty<double> &maxDistance) {

  NodeVectorProperty<double> distance(graph);
  distance.setAll(0);
  double val = tlp::maxDistance(graph, n, distance, weight,
                                directed ? EdgeType::DIRECTED : EdgeType::UNDIRECTED);

  maxDistance[n] = val;
  if (!allPaths) {
    return val;
  }

  double nbAcc = 0.;
  val = 0.;
  uint nbNodes = graph->numberOfNodes();
  double maxDist = nbNodes;
  if (weight) {
    maxDist = nbNodes * weight->getEdgeDoubleMax();
  }

  for (auto nn : graph->nodes()) {
    double d = distance[nn];

    if (d < maxDist) {
      nbAcc += 1.;

      if (nn != n) {
        val += d;
      }
    }
  }

  if (nbAcc < 2.0) {
    return 0.0;
  }

  if (norm) {
    val = 1.0 / val;
  } else {
    val /= (nbAcc - 1.0);
  }

  return val;
}
//====================================================================
bool EccentricityMetric::run() {
  allPaths = false;
  norm = true;
  directed = false;
  weight = nullptr;

  if (dataSet != nullptr) {
    dataSet->get("closeness centrality", allPaths);
    dataSet->get("norm", norm);
    dataSet->get("directed", directed);
    dataSet->get("weight", weight);
  }

  // Edges weights should be positive
  if (weight && weight->getEdgeDoubleMin() <= 0) {
    if (pluginProgress) {
      pluginProgress->setError("Edges weights should be positive.");
    }
    return false;
  }

  NodeVectorProperty<double> maxDistance(graph);
  NodeVectorProperty<double> res(graph);
  atomic_int nbTreatedNodes = 0;
  atomic_bool stop = false;

  TLP_PARALLEL_MAP_NODES(graph, [&](node n) {
    if (stop.load()) {
      return;
    }
    res[n] = compute(n, maxDistance);
    ++nbTreatedNodes;
    if (ThreadManager::getThreadNumber() == 0) {
      if (pluginProgress &&
          pluginProgress->progress(nbTreatedNodes.load(), graph->numberOfNodes()) !=
              ProgressState::TLP_CONTINUE) {
        stop = true;
      }
    }
  });

  if (pluginProgress && pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  } else if (pluginProgress) {
    pluginProgress->progress(graph->numberOfNodes(), graph->numberOfNodes());
  }

  double diameter = *max_element(maxDistance.begin(), maxDistance.end());

  for (auto n : graph->nodes()) {
    if (!allPaths && norm) {
      result->setNodeValue(n, res[n] / diameter);
    } else {
      result->setNodeValue(n, res[n]);
    }
  }

  if (dataSet != nullptr) {
    dataSet->set("graph diameter", diameter);
  }

  return true;
}
