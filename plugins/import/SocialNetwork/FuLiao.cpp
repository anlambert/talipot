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

    // m
    "Number of nodes added at each time step.",

    // m
    "Delta coefficient must belong to [0, 1]"};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * Peihua Fu and Kun Liao.
 * An evolving scale-free network with large clustering coefficient.
 * In ICARCV, pp. 1-4. IEEE, (2006).
 *
 */
struct FuLiao : public ImportModule {
  PLUGININFORMATION("Fu and Liao Model", "Arnaud Sallaberry", "21/02/2011",
                    "Randomly generates a scale-free graph using<br/>Peihua Fu and Kun "
                    "Liao.<br/><b>An evolving scale-free network with large clustering "
                    "coefficient.</b><br/>In ICARCV, pp. 1-4. IEEE, (2006).",
                    "1.0", "Social network")

  FuLiao(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "300");
    addInParameter<uint>("m", paramHelp[1].data(), "5");
    addInParameter<double>("delta", paramHelp[2].data(), "0.5");
  }

  bool importGraph() override {
    uint n = 300;
    uint m = 5;
    double d = 0.5;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
      dataSet->get("m", m);
      dataSet->get("delta", d);
    }

    // check arguments
    if (d < 0 || d > 1) {
      pluginProgress->setError("delta is not a probability,\nit is not between [0, 1].");
      return false;
    }

    tlp::initRandomSequence();
    uint i, j;

    /*
     * Initial ring construction
     */
    uint m0 = 3;
    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    for (i = 1; i < m0; ++i) {
      graph->addEdge(nodes[i - 1], nodes[i]);
    }

    graph->addEdge(nodes[m0 - 1], nodes[0]);

    /*
     * Main loop
     */
    for (i = m0; i < n; ++i) {
      double k_sum = 0;

      for (j = 0; j < i; ++j) {
        k_sum += graph->deg(nodes[j]);
      }

      // add first edge
      double pr_sum = 0;
      uint rn = 0;
      double pr = tlp::randomNumber();

      while (pr_sum < pr && rn < (i - 1)) {
        if (!graph->hasEdge(nodes[i], nodes[rn])) {
          pr_sum += double(graph->deg(nodes[rn])) / k_sum;
        }

        ++rn;
      }

      graph->addEdge(nodes[i], nodes[rn]);

      // add other edges
      for (j = 1; j < m; ++j) {
        rn = 0;
        double h_sum = 0;

        while (rn < (i - 1)) {
          if (!graph->hasEdge(nodes[i], nodes[rn])) {
            for (auto v : graph->getInOutNodes(nodes[rn])) {
              if (graph->hasEdge(nodes[i], v)) {
                h_sum++;
              }
            }
          }

          ++rn;
        }

        pr_sum = 0;
        rn = 0;
        pr = tlp::randomNumber();

        while (pr_sum < pr && rn < (i - 1)) {
          if (!graph->hasEdge(nodes[i], nodes[rn])) {
            double h = 0;
            for (auto v : graph->getInOutNodes(nodes[rn])) {
              if (graph->hasEdge(nodes[i], v)) {
                h++;
              }
            }
            pr_sum = pr_sum + (1.0 - d) * graph->deg(nodes[rn]) / (k_sum + j) + d * (h / h_sum);
          }

          ++rn;
        }

        --rn;
        graph->addEdge(nodes[i], nodes[rn]);
      }
    }

    return true;
  }
};

PLUGIN(FuLiao)
