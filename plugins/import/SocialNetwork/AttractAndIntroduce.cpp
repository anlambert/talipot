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

#include <talipot/ImportModule.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // nodes
    "This parameter defines the amount of nodes used to build the graph.",

    // edges
    "This parameter defines the amount of edges used to build the graph.",

    // alpha
    "This parameter defines the alpha parameter between [0,1]. This one is a percentage and "
    "describes the distribution of attractiveness; the model suggests about 1 - alpha of the "
    "individuals have very low attractiveness whereas the remaining alpha are approximately evenly "
    "distributed between low, medium, and high attractiveness",

    // beta
    "This parameter defines the beta parameter between [0,1]. This parameter indicates the "
    "probability a person will have the desire to introduce someone."};

//=================================================================================

/**
 * This plugin is an implementation of the "Attract and Introduce Model"
 * described in
 * J. H. Fowlera, C. T. Dawesa, N. A. Christakisb.
 * Model of genetic variation in human social networks.
 * PNAS 106 (6): 1720-1724, 2009.
 *
 */
class AttractAndIntroduce : public ImportModule {
public:
  PLUGININFORMATION("Attract And Introduce Model", "Arnaud Sallabery & Patrick Mary", "25/03/2014",
                    "Randomly generates a graph using the Attract and Introduce Model described "
                    "in<br/>J. H. Fowlera, C. T. Dawesa, N. A. Christakisb.<br/><b>Model of "
                    "genetic variation in human social networks.</b><br/>PNAS 106 (6): 1720-1724, "
                    "2009.",
                    "1.0", "Social network")

  AttractAndIntroduce(tlp::PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "750");
    addInParameter<uint>("edges", paramHelp[1].data(), "3150");
    addInParameter<double>("alpha", paramHelp[2].data(), "0.9");
    addInParameter<double>("beta", paramHelp[3].data(), "0.3");
  }

  bool importGraph() override {
    uint nbNodes = 750;
    uint nbEdges = 3150;

    double alpha = 0.9;
    double beta = 0.3;

    if (dataSet != nullptr) {
      dataSet->get("nodes", nbNodes);
      dataSet->get("edges", nbEdges);
      dataSet->get("alpha", alpha);
      dataSet->get("beta", beta);
    }

    // check arguments
    if (alpha < 0 || alpha > 1) {
      pluginProgress->setError("alpha is not a percentage,\nit is not between [0, 1]");
      return false;
    } else if (beta < 0 || beta > 1) {
      pluginProgress->setError("beta is not a probability,\nit is is not between [0, 1]");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    uint iterations = nbNodes + nbEdges;

    graph->addNodes(nbNodes);
    graph->reserveEdges(nbEdges);

    NodeVectorProperty<double> pAttractProperty(graph);
    NodeVectorProperty<double> pIntroduceProperty(graph);

    for (uint i = 0; i < nbNodes; ++i) {
      pAttractProperty[i] = ((1 - alpha) > randomNumber(1.0)) ? 0 : randomNumber(1.0);
      pIntroduceProperty[i] = (beta > randomNumber(1.0)) ? 1 : 0;

      if (i++ % 1000 == 0) {
        if (pluginProgress->progress(i, iterations) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }
    }

    uint tmpE = 0;
    const vector<node> &nodes = graph->nodes();

    while (tmpE < nbEdges) {
      uint i = randomNumber(nbNodes - 1);
      uint j;

      do {
        j = randomNumber(nbNodes - 1);
      } while (i == j);

      node nj = nodes[j];

      if (pAttractProperty[j] > randomNumber(1.0)) {
        node ni = nodes[i];

        if (pIntroduceProperty[i] > randomNumber(1.0)) {
          for (auto fd : graph->getInOutNodes(ni)) {
            if (fd == nj || graph->hasEdge(fd, nj, false)) {
              continue;
            }

            if (pAttractProperty[j] > randomNumber(1.0)) {
              graph->addEdge(fd, nj);
              ++tmpE;
              continue;
            }

            if (pAttractProperty[fd] > randomNumber(1.0)) {
              graph->addEdge(nj, fd);
              ++tmpE;
            }
          }
        }

        if (!graph->hasEdge(ni, nj, false)) {
          graph->addEdge(ni, nj);
          ++tmpE;
        }

        if (tmpE % 1000 == 0) {
          if (pluginProgress->progress(tmpE, iterations) != ProgressState::TLP_CONTINUE) {
            return pluginProgress->state() != ProgressState::TLP_CANCEL;
          }
        }
      }
    }

    return true;
  }
};

PLUGIN(AttractAndIntroduce)
