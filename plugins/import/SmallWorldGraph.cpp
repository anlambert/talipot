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
static const int WIDTH = 1024;
static const int HEIGHT = 1024;

static constexpr std::string_view paramHelp[] = {
    // nodes
    "Number of nodes in the final graph.",

    // degree
    "Average degree of the nodes in the final graph.",

    // degree
    "If true, long distance edges are added in the grid approximation."};

/*
 * TODO :
 * Use a quadtree in order to change the complexity from n*n to n*log(n)
 */

/// Grid Approximation - Import of a grid approximation
/** This plugin enables to create a grid approximation
 *
 *  User can specify the number of nodes and their average degree.
 */
class SmallWorldGraph : public ImportModule {
public:
  PLUGININFORMATION("Grid Approximation", "Auber", "25/06/2002",
                    "Imports a new grid approximation graph.", "1.0", "Graph")
  SmallWorldGraph(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "200");
    addInParameter<uint>("degree", paramHelp[1].data(), "10");
    addInParameter<bool>("long edge", paramHelp[2].data(), "false");
  }
  ~SmallWorldGraph() override = default;

  bool importGraph() override {
    uint nbNodes = 200;
    uint avgDegree = 10;
    bool enableLongEdge = false;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("degree", avgDegree);
      dataSet->get("long edge", enableLongEdge);
    }

    if (nbNodes == 0) {
      if (pluginProgress) {
        pluginProgress->setError(string("Error: the number of nodes cannot be null"));
      }

      return false;
    }

    if (avgDegree == 0) {
      if (pluginProgress) {
        pluginProgress->setError(string("Error: the average degree cannot be null"));
      }

      return false;
    }

    double maxDistance =
        sqrt(double(avgDegree) * double(WIDTH) * double(HEIGHT) / (double(nbNodes) * M_PI));
    // initialize a random sequence according the given seed
    tlp::initRandomSequence();

    LayoutProperty *newLayout = graph->getLocalLayoutProperty("viewLayout");

    pluginProgress->showPreview(false);

    graph->addNodes(nbNodes);
    graph->reserveEdges(nbNodes * avgDegree);

    const vector<node> &nodes = graph->nodes();

    for (auto n : nodes) {
      newLayout->setNodeValue(n, Coord(float(randomNumber(WIDTH)), float(randomNumber(HEIGHT)), 0));
    }

    // double minSize = DBL_MAX;

    for (uint i = 0; i < nbNodes - 1; ++i) {
      bool longEdge = false;

      for (uint j = i + 1; j < nbNodes; ++j) {
        if (i != j) {
          double distance =
              newLayout->getNodeValue(nodes[i]).dist(newLayout->getNodeValue(nodes[j]));
          // minSize = std::min(distance, minSize);

          // newSize->setAllNodeValue(Size(minSize/2.0, minSize/2.0, 1));
          if (distance < maxDistance) {
            graph->addEdge(nodes[i], nodes[j]);
          } else if (!longEdge && enableLongEdge) {
            double distrand = randomNumber();

            if (distrand < 1.0 / (2.0 + double(nbNodes - i - 1))) {
              longEdge = true;
              graph->addEdge(nodes[i], nodes[j]);
            }
          }
        }
      }

      if (((i % 100) == 0) &&
          (pluginProgress->progress(i, nbNodes - 1) != ProgressState::TLP_CONTINUE)) {
        break;
      }
    }

    return pluginProgress->state() != ProgressState::TLP_CANCEL;
  }
};

PLUGIN(SmallWorldGraph)
