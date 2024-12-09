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

#include "StrahlerMetric.h"

PLUGIN(StrahlerMetric)

using namespace std;
using namespace tlp;

struct StackEval {
  StackEval(int f, int u) : freeS(f), usedS(u) {}
  int freeS, usedS;
};

struct GreaterStackEval {
  bool operator()(const StackEval e1, const StackEval e2) {
    return (e1.freeS > e2.freeS);
  }
};

Strahler StrahlerMetric::topSortStrahler(tlp::node n, int &curPref,
                                         flat_hash_map<node, int> &tofree,
                                         flat_hash_map<node, int> &prefix,
                                         flat_hash_map<node, bool> &visited,
                                         flat_hash_map<node, bool> &finished,
                                         flat_hash_map<node, Strahler> &cachedValues) {
  visited[n] = true;
  Strahler result;
  prefix[n] = curPref;
  curPref++;

  if (graph->outdeg(n) == 0) {
    finished[n] = true;
    return (result);
  }

  list<int> strahlerResult;
  list<StackEval> tmpEval;
  // Construction des ensembles pour evaluer le strahler

  for (auto tmpN : graph->getOutNodes(n)) {

    if (!visited[tmpN]) {
      // Arc Normal
      tofree[n] = 0;
      Strahler tmpValue =
          topSortStrahler(tmpN, curPref, tofree, prefix, visited, finished, cachedValues);
      // Data for strahler evaluation on the spanning Dag.
      strahlerResult.push_front(tmpValue.strahler);
      // Counting current used stacks.
      tmpEval.push_back(StackEval(tmpValue.stacks - tmpValue.usedStack + tofree[n],
                                  tmpValue.usedStack - tofree[n]));
      // Looking if we need more stacks to evaluate this node.
      // old freeStacks=max(freeStacks,tmpValue.stacks-tmpValue.usedStack+tofree[n]);
    } else {
      if (finished[tmpN]) {
        if (prefix[tmpN] < prefix[n]) {
          // Cross Edge
          Strahler tmpValue = cachedValues[tmpN];
          // Data for strahler evaluation on the spanning Dag.
          strahlerResult.push_front(tmpValue.strahler);
          // Looking if we need more stacks to evaluate this node.
          tmpEval.push_back(StackEval(tmpValue.stacks, 0));
        } else {
          // Arc descent
          Strahler tmpValue = cachedValues[tmpN];
          // Data for strahler evaluation on the spanning Dag.
          strahlerResult.push_front(tmpValue.strahler);
        }
      } else {
        if (tmpN == n) {
          tmpEval.push_back(StackEval(1, 0));
        } else {
          // New nested cycle.
          tofree[tmpN]++;
          tmpEval.push_back(StackEval(0, 1));
          // result.usedStack++;
        }

        // Return edge
        // Register needed tp store the result of the recursive call
        strahlerResult.push_front(1);
      }
    }
  }

  // compute the minimal nested cycles
  GreaterStackEval gSE;
  tmpEval.sort(gSE);
  result.stacks = 0;
  result.usedStack = 0;

  for (auto se : tmpEval) {
    result.usedStack += se.usedS;
    result.stacks = std::max(result.stacks, se.freeS + se.usedS);
    result.stacks -= se.usedS;
  }

  result.stacks = result.stacks + result.usedStack;
  // evaluation of tree strahler
  int additional = 0;
  int available = 0;
  strahlerResult.sort();

  while (!strahlerResult.empty()) {
    int tmpDbl = strahlerResult.back();
    strahlerResult.pop_back();

    if (tmpDbl > available) {
      additional += tmpDbl - available;
      available = tmpDbl - 1;
    } else {
      available -= 1;
    }
  }

  result.strahler = additional;
  strahlerResult.clear();
  finished[n] = true;
  cachedValues[n] = result;
  return result;
}

static constexpr std::string_view paramHelp[] = {
    // All nodes
    "If true, for each node the Strahler number is computed from a spanning tree having that node "
    "as root: complexity o(n^2). If false the Strahler number is computed from a spanning tree "
    "having the heuristicly estimated graph center as root.",

    // Type
    "Sets the type of computation."};

#define COMPUTATION_TYPE "Type"
#define COMPUTATION_TYPES "all;ramification;nested cycles;"
#define ALL 0
#define REGISTERS 1
#define STACKS 2
//==============================================================================
StrahlerMetric::StrahlerMetric(const tlp::PluginContext *context)
    : DoubleAlgorithm(context), allNodes(false) {
  addInParameter<bool>("All nodes", paramHelp[0].data(), "false");
  addInParameter<StringCollection>(COMPUTATION_TYPE, paramHelp[1].data(), COMPUTATION_TYPES, true,
                                   "<b>all</b> <br> <b>ramification</b> <br> <b>nested cycles</b>");
}
//==============================================================================
bool StrahlerMetric::run() {
  allNodes = false;
  StringCollection computationTypes(COMPUTATION_TYPES);
  computationTypes.setCurrent(0);

  if (dataSet != nullptr) {
    dataSet->get("All nodes", allNodes);
    dataSet->get(COMPUTATION_TYPE, computationTypes);
  }

  flat_hash_map<node, bool> visited;
  flat_hash_map<node, bool> finished;
  flat_hash_map<node, int> prefix;
  flat_hash_map<node, int> tofree;
  flat_hash_map<node, Strahler> cachedValues;
  int curPref = 0;

  uint i = 0;

  if (pluginProgress) {
    pluginProgress->showPreview(false);
  }

  for (auto n : graph->nodes()) {
    tofree[n] = 0;

    if (!finished[n]) {
      topSortStrahler(n, curPref, tofree, prefix, visited, finished, cachedValues);
    }

    if (allNodes) {
      if (pluginProgress && ((++i % 100) == 0) &&
          (pluginProgress->progress(i, graph->numberOfNodes()) != ProgressState::TLP_CONTINUE)) {
        break;
      }

      switch (computationTypes.getCurrent()) {
      case ALL:
        result->setNodeValue(
            n, sqrt(double(cachedValues[n].strahler) * double(cachedValues[n].strahler) +
                    double(cachedValues[n].stacks) * double(cachedValues[n].stacks)));
        break;

      case REGISTERS:
        result->setNodeValue(n, cachedValues[n].strahler);
        break;

      case STACKS:
        result->setNodeValue(n, cachedValues[n].stacks);
      }

      visited.clear();
      finished.clear();
      prefix.clear();
      tofree.clear();
      cachedValues.clear();
      curPref = 0;
    }
  }

  if (pluginProgress->state() != ProgressState::TLP_CONTINUE) {
    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }

  if (!allNodes) {

    for (auto n : graph->nodes()) {

      switch (computationTypes.getCurrent()) {
      case ALL:
        result->setNodeValue(
            n, sqrt(double(cachedValues[n].strahler) * double(cachedValues[n].strahler) +
                    double(cachedValues[n].stacks) * double(cachedValues[n].stacks)));
        break;

      case REGISTERS:
        result->setNodeValue(n, cachedValues[n].strahler);
        break;

      case STACKS:
        result->setNodeValue(n, cachedValues[n].stacks);
      }
    }
  }

  return pluginProgress->state() != ProgressState::TLP_CANCEL;
}
//==============================================================================
