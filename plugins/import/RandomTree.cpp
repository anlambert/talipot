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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // minsize
    "Minimal number of nodes in the tree.",

    // maxsize
    "Maximal number of nodes in the tree.",

    // tree layout
    "If true, the generated tree is drawn with the 'Tree Leaf' layout algorithm."};

/// Random Tree - Import of a random uniform binary tree
/** This plugin enables to create a random tree
 *
 *  User can specify the minimal/maximal numbers of nodes used to build of the tree.
 */
class RandomTree : public ImportModule {

  bool buildNode(node n, uint sizeM) {
    if (graph->numberOfNodes() >= sizeM - 1) {
      return false;
    }

    bool result = true;
    int randNumber = randomNumber(RAND_MAX);

    if (randNumber > RAND_MAX / 2) {
      node n1, n2;
      n1 = graph->addNode();
      graph->addEdge(n, n1);
      result = buildNode(n1, sizeM);

      if (result) {
        n2 = graph->addNode();
        graph->addEdge(n, n2);
        result = buildNode(n2, sizeM);
      }
    }

    return result;
  }

public:
  PLUGININFORMATION("Uniform Random Binary Tree", "Auber", "16/02/2001",
                    "Imports a new randomly generated uniform binary tree.", "1.1", "Graph")
  RandomTree(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("Minimum size", paramHelp[0].data(), "50");
    addInParameter<uint>("Maximum size", paramHelp[1].data(), "60");
    addInParameter<bool>("tree layout", paramHelp[2].data(), "false");
    addDependency("Tree Leaf", "1.0");
  }

  bool importGraph() override {
    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    uint minSize = 100;
    uint maxSize = 1000;
    bool needLayout = false;

    if (dataSet != nullptr) {
      if (!dataSet->get("Minimum size", minSize)) {
        dataSet->get("minsize", minSize); // keep old name for backward compatibility
      }

      if (!dataSet->get("Maximum size", maxSize)) {
        dataSet->get("maxsize", maxSize); // keep old name for backward compatibility
      }

      dataSet->get("tree layout", needLayout);
    }

    if (maxSize < 1) {
      if (pluginProgress) {
        pluginProgress->setError("Error: maximum size must be a strictly positive integer");
      }

      return false;
    }

    if (maxSize < minSize) {
      if (pluginProgress) {
        pluginProgress->setError("Error: maximum size must be greater than minimum size");
      }

      return false;
    }

    bool ok = true;
    int i = 0;

    while (ok) {
      if (pluginProgress->progress(i % 100, 100) != ProgressState::TLP_CONTINUE) {
        break;
      }

      ++i;
      graph->clear();
      node n = graph->addNode();
      ok = !buildNode(n, maxSize);

      if (graph->numberOfNodes() < minSize) {
        ok = true;
      }
    }

    if (pluginProgress->progress(100, 100) == ProgressState::TLP_CANCEL) {
      return false;
    }

    if (needLayout) {
      // apply Tree Leaf
      string errMsg;
      LayoutProperty *layout = graph->getLayoutProperty("viewLayout");
      return graph->applyPropertyAlgorithm("Tree Leaf", layout, errMsg, nullptr, pluginProgress);
    }

    return true;
  }
};

PLUGIN(RandomTree)
