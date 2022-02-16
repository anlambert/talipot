/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

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
    "An existing edge weight metric property."};

EccentricityMetric::EccentricityMetric(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), allPaths(false), norm(true), directed(false) {
  addInParameter<bool>("closeness centrality", paramHelp[0].data(), "false");
  addInParameter<bool>("norm", paramHelp[1].data(), "true");
  addInParameter<bool>("directed", paramHelp[2].data(), "false");
  addInParameter<NumericProperty *>("weight", paramHelp[3].data(), "", false);
  addOutParameter<double>("graph diameter", "The computed diameter (-1 if not computed)", "-1");
}
//====================================================================
EccentricityMetric::~EccentricityMetric() = default;
//====================================================================
double EccentricityMetric::compute(uint nPos) {

  NodeVectorProperty<double> distance(graph);
  distance.setAll(0);
  double val = tlp::maxDistance(graph, nPos, distance, weight,
                                directed ? EdgeType::DIRECTED : EdgeType::UNDIRECTED);

  if (!allPaths) {
    return val;
  }

  double nbAcc = 0.;
  val = 0.;
  uint nbNodes = graph->numberOfNodes();
  double max_d_acc = nbNodes + 0.;
  if (weight) {
    max_d_acc = nbNodes * weight->getEdgeDoubleMax();
  }

  for (uint i = 0; i < nbNodes; ++i) {
    double d = distance[i];

    if (d < max_d_acc) {
      nbAcc += 1.;

      if (i != nPos) {
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
    pluginProgress->setError("Edges weights should be positive.");
    return false;
  }

  NodeVectorProperty<double> res(graph);
  uint nbNodes = graph->numberOfNodes();

  double diameter = 1.0;
  std::atomic<bool> stopfor(false);
  TLP_PARALLEL_MAP_INDICES(nbNodes, [&](uint i) {
    if (stopfor.load()) {
      return;
    }

    if (ThreadManager::getThreadNumber() == 0) {
      if (pluginProgress->progress(i, nbNodes / ThreadManager::getNumberOfThreads()) !=
          ProgressState::TLP_CONTINUE) {
        stopfor = true;
      }
    }

    res[i] = compute(i);

    if (!allPaths && norm) {
      TLP_LOCK_SECTION(DIAMETER) {
        if (diameter < res[i]) {
          diameter = res[i];
        }
      }
      TLP_UNLOCK_SECTION(DIAMETER);
    }
  });

  if (pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }

  TLP_MAP_NODES_AND_INDICES(graph, [&](const node n, uint i) {
    if (!allPaths && norm) {
      result->setNodeValue(n, res[i] / diameter);
    } else {
      result->setNodeValue(n, res[i]);
    }
  });

  if (dataSet != nullptr) {
    dataSet->set("graph diameter", (!allPaths && norm) ? diameter : double(-1));
  }

  return pluginProgress->state() != ProgressState::TLP_CANCEL;
}
