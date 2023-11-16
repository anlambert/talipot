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
    "Number of nodes."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * J.-G. Liu, Y.-Z. Dang, and Z. tuo Wang.
 * Multistage random growing small-world networks with power-law degree distribution.
 * Chinese Phys. Lett., 23(3):746, Oct. 31 2005.
 *
 */
struct LiuEtAl : public ImportModule {
  PLUGININFORMATION("Liu et al. model", "Arnaud Sallaberry", "20/06/2011",
                    "Randomly generates a small world graph using the model described in<br/>J.-G. "
                    "Liu, Y.-Z. Dang, and Z. tuo Wang.<br/><b>Multistage random growing "
                    "small-world networks with power-law degree distribution.</b><br/>Chinese "
                    "Phys. Lett., 23(3):746, Oct. 31 2005.",
                    "1.0", "Social network")

  LiuEtAl(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "300");
  }

  ~LiuEtAl() override = default;

  bool importGraph() override {
    uint n = 300;
    uint m = 5;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    uint i, j;

    /*
     * Initial ring construction
     */
    uint m0 = 3;
    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    graph->reserveEdges(m0 + (2 * (n - m0) * m / 2));

    for (i = 1; i < m0; ++i) {
      graph->addEdge(nodes[i - 1], nodes[i]);
    }

    graph->addEdge(nodes[m0 - 1], nodes[0]);

    /*
     * Main loop
     */
    for (i = m0; i < n; ++i) {
      if (i % 100 == 0) {
        if (pluginProgress->progress(i, n) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }

      double k_sum = 0;

      for (j = 0; j < i; ++j) {
        k_sum += graph->deg(nodes[j]);
      }

      /*
       * Preferential attachment
       */
      for (j = 0; j < m / 2; ++j) {
        double pr = tlp::randomNumber();
        double pr_sum = 0;
        uint rn = 0;

        while (pr_sum < pr && rn < (i - 1)) {
          pr_sum += graph->deg(nodes[rn]) / (k_sum + j);
          ++rn;
        }

        --rn;

        /*
         * Triad formation
         * Preferential attachment with a neighbour of the first node selected
         */
        double k2_sum = 0;
        for (auto n : graph->getInOutNodes(nodes[rn])) {
          k2_sum += graph->deg(n);
        }
        pr = tlp::randomNumber();
        pr_sum = 0;
        node v;

        for (auto n : graph->getInOutNodes(nodes[rn])) {
          if (pr_sum >= pr) {
            break;
          }
          v = n;
          pr_sum += graph->deg(n) / k2_sum;
        }

        graph->addEdge(nodes[i], nodes[rn]);
        graph->addEdge(nodes[i], v);
      }
    }

    return true;
  }
};

PLUGIN(LiuEtAl)
