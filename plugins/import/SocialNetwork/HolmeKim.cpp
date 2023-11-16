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
    "Number of edges added at each time step.",

    // proba
    "Probability of adding a triangle after adding a random edge."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * Petter Holme and Beom Jun Kim.
 * Growing scale-free networks with tunable clustering.
 * Physical Review E, 65, 026107, (2002).
 *
 */
struct HolmeKim : public ImportModule {
  PLUGININFORMATION("Holme and Kim Model", "Sallaberry & Pennarun", "21/02/2011 & 08/04/2014",
                    "Randomly generates a scale-free graph using the model described in<br/>Petter "
                    "Holme and Beom Jun Kim.<br/><b>Growing scale-free networks with tunable "
                    "clustering.</b><br/>Physical Review E, 65, 026107, (2002).",
                    "1.0", "Social network")

  HolmeKim(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "300");
    addInParameter<uint>("m", paramHelp[1].data(), "5");
    addInParameter<double>("p", paramHelp[2].data(), "0.5");
  }

  bool importGraph() override {
    uint n = 300;
    uint m = 5;
    double mu = 0.5;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
      dataSet->get("m", m);
      dataSet->get("p", mu);
    }

    // check arguments
    if (m > n) {
      pluginProgress->setError("The m parameter cannot be greater than the number of nodes.");
      return false;
    }

    if (mu > 1 || mu < 0) {
      pluginProgress->setError("The p parameter must belong to [0, 1].");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    /*
     * Initial ring construction
     */
    uint m0 = 3;
    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    for (uint i = 1; i < m0; ++i) {
      graph->addEdge(nodes[i - 1], nodes[i]);
    }

    graph->addEdge(nodes[m0 - 1], nodes[0]);

    /*
     * Main loop
     */
    for (uint i = m0; i < n; ++i) {
      double k_sum = 0; // degree of present nodes

      for (uint j = 0; j < i; ++j) {
        k_sum += graph->deg(nodes[j]);
      }

      double proba = tlp::randomNumber();

      for (uint j = 0; j < m; ++j) {
        // Preferential attachment
        double pr = tlp::randomNumber();
        double pr_sum = 0;
        double firstNeighbour = 0;

        while (pr_sum < pr && firstNeighbour <= i) {
          pr_sum += graph->deg(nodes[firstNeighbour]) / k_sum;
          ++firstNeighbour;
        }

        graph->addEdge(nodes[i], nodes[--firstNeighbour]);

        if (proba < mu) { // Triad formation
          // collect all neighbours of firstNeighbour
          // which are not already connected to nodes[i]
          vector<node> freeNeighbours;

          for (auto neighbour : graph->getInOutNodes(nodes[firstNeighbour])) {
            if (!graph->hasEdge(nodes[i], neighbour)) {
              freeNeighbours.push_back(neighbour);
            }
          }

          if (!freeNeighbours.empty()) {
            // randomly choose one of the free neighbours to connect with
            uint randomNeighbour = tlp::randomNumber(freeNeighbours.size() - 1);
            graph->addEdge(nodes[i], freeNeighbours[randomNeighbour]);
            continue;
          }
        }

        // Preferential attachment
        pr = tlp::randomNumber();
        pr_sum = 0;
        uint rn = 0;

        while (pr_sum < pr && rn < (i - 1)) {
          pr_sum += graph->deg(nodes[rn]) / k_sum;
          ++rn;
        }

        graph->addEdge(nodes[i], nodes[--rn]);
      }
    }

    return true;
  }
};

PLUGIN(HolmeKim)
