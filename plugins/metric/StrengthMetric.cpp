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

#include "StrengthMetric.h"

PLUGIN(StrengthMetric)

using namespace std;
using namespace tlp;

StrengthMetric::StrengthMetric(const tlp::PluginContext *context) : DoubleAlgorithm(context) {}

StrengthMetric::~StrengthMetric() = default;
//=============================================================
double StrengthMetric::e(std::unordered_set<tlp::node> &U, std::unordered_set<tlp::node> &V) {
  double result = 0;
  std::unordered_set<node> *A, *B;

  if (U.size() < V.size()) {
    A = &U;
    B = &V;
  } else {
    A = &V;
    B = &U;
  }

  for (auto n : *A) {
    for (auto n2 : graph->getInOutNodes(n)) {
      if (B->contains(n2)) {
        result += 1.0;
      }
    }
  }

  return result;
}
//=============================================================
double StrengthMetric::e(const std::unordered_set<tlp::node> &U) {
  double result = 0.0;

  for (auto u : U) {
    for (auto n : graph->getInOutNodes(u)) {
      if (U.contains(n)) {
        result += 1.0;
      }
    }
  }

  return result / 2.0;
}
//=============================================================
double StrengthMetric::s(std::unordered_set<tlp::node> &U, std::unordered_set<tlp::node> &V) {
  if ((U.empty()) || (V.empty())) {
    return 0;
  }

  return (e(U, V) / double(U.size() * V.size()));
}
//=============================================================
double StrengthMetric::s(const std::unordered_set<tlp::node> &U) {
  if (U.size() < 2) {
    return 0.0;
  }

  return (e(U)) * 2.0 / double(U.size() * (U.size() - 1));
}
//=============================================================
double StrengthMetric::getEdgeValue(const tlp::edge ee) {
  const auto &[u, v] = graph->ends(ee);
  std::unordered_set<node> Nu, Nv, Wuv;

  // Compute Nu
  for (auto n : graph->getInOutNodes(u)) {
    if (n != v) {
      Nu.insert(n);
    }
  }

  if (Nu.empty()) {
    return 0;
  }

  // Compute Nv
  for (auto n : graph->getInOutNodes(v)) {
    if (n != u) {
      Nv.insert(n);
    }
  }

  if (Nv.empty()) {
    return 0;
  }

  // Compute Wuv, choose the minimum set to minimize operation
  std::unordered_set<node> *A, *B;

  if (Nu.size() < Nv.size()) {
    A = &Nu;
    B = &Nv;
  } else {
    A = &Nv;
    B = &Nu;
  }

  for (auto na : *A) {
    if (B->contains(na)) {
      Wuv.insert(na);
    }
  }

  std::unordered_set<node> &Mu = Nu;
  std::unordered_set<node> &Mv = Nv;

  /* Compute Mu and Mv, we do not need Nu and Nv anymore,
     thus we modify them to speed up computation
  */
  for (auto uv : Wuv) {
    Mu.erase(uv);
    Mv.erase(uv);
  }

  // compute strength metric
  auto gamma3 = double(Wuv.size());
  auto norm3 = double((Wuv.size() + Mv.size() + Mu.size()));

  double gamma4 = (e(Mu, Wuv) + e(Mv, Wuv) + e(Mu, Mv) + e(Wuv));
  double norm4 = (double(Mu.size() * Wuv.size() + Mv.size() * Wuv.size() + Mu.size() * Mv.size()) +
                  double(Wuv.size() * (Wuv.size() - 1)) / 2.0);

  double norm = norm3 + norm4;
  double gamma = gamma3 + gamma4;

  if (norm > 1E-5) {
    gamma /= norm;
  } else {
    gamma = 0;
  }

  return gamma;
}
//=============================================================
double StrengthMetric::getNodeValue(const tlp::node n) {
  //  tlp::warning() << __PRETTY_FUNCTION__ << endl;
  if (graph->deg(n) == 0) {
    return 0;
  }

  double res = 0;

  for (auto ite : graph->incidence(n)) {
    res += result->getEdgeValue(ite);
  }

  return res / double(graph->deg(n));
}
//=============================================================
bool StrengthMetric::run() {
  uint steps = 0, maxSteps = graph->numberOfEdges();

  if (maxSteps < 10) {
    maxSteps = 10;
  }

  pluginProgress->showPreview(false);
  pluginProgress->setComment("Computing Strength metric on edges...");

  for (auto e : graph->edges()) {
    result->setEdgeValue(e, getEdgeValue(e));

    if ((++steps % (maxSteps / 10)) == 0) {
      if (pluginProgress->progress(steps, maxSteps) != ProgressState::TLP_CONTINUE) {
        return pluginProgress->state() != ProgressState::TLP_CANCEL;
      }
    }
  }

  steps = 0;
  maxSteps = graph->numberOfNodes();

  if (maxSteps < 10) {
    maxSteps = 10;
  }

  pluginProgress->setComment("Computing Strength metric on nodes...");
  for (auto n : graph->nodes()) {
    result->setNodeValue(n, getNodeValue(n));

    if ((++steps % (maxSteps / 10)) == 0) {
      if (pluginProgress->progress(steps, maxSteps) != ProgressState::TLP_CONTINUE) {
        return pluginProgress->state() != ProgressState::TLP_CANCEL;
      }
    }
  }

  return true;
}
//=============================================================
