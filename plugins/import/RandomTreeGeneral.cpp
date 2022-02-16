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

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // minsize
    "Minimal number of nodes in the tree.",

    // maxsize
    "Maximal number of nodes in the tree.",

    // maxdegree
    "Maximal degree of the nodes.",

    // tree layout
    "If true, the generated tree is drawn with the 'Tree Leaf' layout algorithm."};

/// Random General Tree - Import of a random general tree
/** This plugin enables to create a random general tree
 *
 *  User can specify the minimal/maximal number of nodes and the maximal degree.
 */
class RandomTreeGeneral : public ImportModule {

  bool buildNode(node n, uint sizeM, int arityMax) {
    if (graph->numberOfNodes() >= sizeM) {
      return true;
    }

    bool result = true;
    int randNumber = randomInteger(RAND_MAX);
    int i = 0;

    while (randNumber < RAND_MAX / pow(2.0, 1.0 + i)) {
      ++i;
    }

    i = i % arityMax;
    graph->reserveNodes(i);
    graph->reserveEdges(i);

    for (; i > 0; --i) {
      node n1;
      n1 = graph->addNode();
      graph->addEdge(n, n1);
      result = result && buildNode(n1, sizeM, arityMax);
    }

    return result;
  }

public:
  PLUGININFORMATION("Random General Tree", "Auber", "16/02/2001",
                    "Imports a new randomly generated tree.", "1.1", "Graph")
  RandomTreeGeneral(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<unsigned>("Minimum size", paramHelp[0].data(), "10");
    addInParameter<unsigned>("Maximum size", paramHelp[1].data(), "100");
    addInParameter<unsigned>("Maximal node's degree", paramHelp[2].data(), "5");
    addInParameter<bool>("tree layout", paramHelp[3].data(), "false");
    addDependency("Tree Leaf", "1.0");
  }

  bool importGraph() override {
    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    uint sizeMin = 10;
    uint sizeMax = 100;
    uint arityMax = 5;
    bool needLayout = false;

    if (dataSet != nullptr) {
      if (!dataSet->get("Minimum size", sizeMin)) {
        dataSet->get("minsize", sizeMin); // keep old parameter name for backward compatibility
      }

      if (!dataSet->get("Maximum size", sizeMax)) {
        dataSet->get("maxsize", sizeMax); // keep old parameter name for backward compatibility
      }

      if (!dataSet->get("Maximal node's degree", arityMax)) {
        dataSet->get("maxdegree", arityMax); // keep old parameter name for backward compatibility
      }

      dataSet->get("tree layout", needLayout);
    }

    if (arityMax < 1) {
      if (pluginProgress) {
        pluginProgress->setError(
            "Error: maximum node's degree must be a strictly positive integer");
      }

      return false;
    }

    if (sizeMax < 1) {
      if (pluginProgress) {
        pluginProgress->setError("Error: maximum size must be a strictly positive integer");
      }

      return false;
    }

    if (sizeMax < sizeMin) {
      if (pluginProgress) {
        pluginProgress->setError("Error: maximum size must be greater than minimum size");
      }

      return false;
    }

    bool ok = true;
    int i = 0;
    uint nbTest = 0;

    while (ok) {
      ++nbTest;

      if (nbTest % 100 == 0) {
        if (pluginProgress->progress((i / 100) % 100, 100) != ProgressState::TLP_CONTINUE) {
          break;
        }
      }

      ++i;
      graph->clear();
      node n = graph->addNode();
      ok = !buildNode(n, sizeMax, arityMax);

      if (graph->numberOfNodes() < sizeMin) {
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

PLUGIN(RandomTreeGeneral)
