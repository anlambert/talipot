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
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // n
    "Number of nodes.",

    // m0
    "Number of nodes in the initial ring.",

    // m
    "Number of nodes added at each time step."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * Jianwei Wang and Lili Rong.
 * Evolving small-world networks based on the modified BA model.
 * International Conference on Computer Science and Information Technology,
 * 0, 143-146, (2008).
 *
 */
struct WangRong : public ImportModule {
  PLUGININFORMATION("Wang and Rong Model", "Arnaud Sallaberry", "21/02/2011",
                    "Randomly generates a small-world graph using the model described "
                    "in<br/>Jianwei Wang and Lili Rong.<br/><b>Evolving small-world networks based "
                    "on the modified BA model.</b><br/>International Conference on Computer "
                    "Science and Information Technology, 0, 143-146, (2008).",
                    "1.0", "Social network")

  WangRong(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "300");
    addInParameter<uint>("m0", paramHelp[1].data(), "5");
    addInParameter<uint>("m", paramHelp[2].data(), "5");
  }

  bool importGraph() override {
    uint n = 300;
    uint m0 = 5;
    uint m = 5;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
      dataSet->get("m0", m0);
      dataSet->get("m", m);
    }

    // check arguments
    if (m > n) {
      pluginProgress->setError("The m parameter cannot be greater than the number of nodes.");
      return false;
    } else if (m0 > n) {
      pluginProgress->setError("The m0 parameter cannot be greater than the number of nodes.");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    uint i, j;

    /*
     * Initial ring construction
     */
    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    for (i = 1; i < m0; ++i) {
      graph->addEdge(nodes[i - 1], nodes[i]);
    }

    graph->addEdge(nodes[m0 - 1], nodes[0]);

    /*
     * Main loop
     */
    uint nbNodes = m0;

    while (nbNodes < n) {
      if (nbNodes % 100 == 0) {
        if (pluginProgress->progress(nbNodes, n) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }

      /*
       * Add clique
       */
      for (i = nbNodes; i < (nbNodes + m); ++i) {
        for (j = nbNodes; j < i; ++j) {
          graph->addEdge(nodes[j], nodes[i]);
        }
      }

      /*
       * Preferencial attachment
       */
      double k_sum = 2 * graph->numberOfEdges();

      for (i = nbNodes; i < (nbNodes + m); ++i) {
        double pr = tlp::randomNumber();
        double pr_sum = 0;
        uint rn = 0;

        while (pr_sum < pr && rn < (nbNodes - 1)) {
          pr_sum += graph->deg(nodes[rn]) / k_sum;
          ++rn;
        }

        graph->addEdge(nodes[i], nodes[rn]);
      }

      nbNodes += m;
    }

    return true;
  }
};

PLUGIN(WangRong)
