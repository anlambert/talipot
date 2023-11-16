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
    // nodes
    "Number of nodes.",

    // m
    "Number of activated nodes.",

    // proba
    "Probability to connect a node to a random other node<br/>instead of an activated node."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * Konstantin Klemm and Victor M. Eguiluz.
 * Growing Scale-Free Networks with Small World Behavior.
 * Physical Review E, 65, 057102,(2002).
 *
 */
struct KlemmEguiluzModel : public ImportModule {
  PLUGININFORMATION("Klemm Eguiluz Model", "Sallaberry & Pennarun", "21/02/2011 & 08/04/2014",
                    "Randomly generates a small world graph using the model described "
                    "in<br/>Konstantin Klemm and Victor M. Eguiluz.<br/><b>Growing Scale-Free "
                    "Networks with Small World Behavior.</b><br/>Physical Review E, 65, "
                    "057102,(2002).",
                    "1.0", "Social network")

  KlemmEguiluzModel(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "200");
    addInParameter<uint>("m", paramHelp[1].data(), "10");
    addInParameter<double>("mu", paramHelp[2].data(), "0.5");
  }

  bool importGraph() override {

    uint n = 200;
    uint m = 10;
    double mu = 0.5;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
      dataSet->get("m", m);
      dataSet->get("mu", mu);
    }

    // check arguments
    if (m > n) {
      pluginProgress->setError("The m parameter cannot be greater than the number of nodes.");
      return false;
    }

    if (mu > 1 || mu < 0) {
      pluginProgress->setError("The mu parameter must belong to [0, 1].");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    vector<bool> activated(n, false);

    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    // fully connect and activate the m first nodes
    for (uint i = 0; i < m; ++i) {
      activated[i] = true;

      for (uint j = i + 1; j < m; ++j) {
        graph->addEdge(nodes[i], nodes[j]);
      }
    }

    for (unsigned i = m; i < n; ++i) {
      if (i % 100 == 0) {
        if (pluginProgress->progress(i, n) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }

      double a = 0, pr, pr_sum;
      for (uint j = 0; j < i; ++j) {
        a += 1 / double(graph->deg(nodes[j]));
      }

      // the new node is connected to m nodes
      for (uint j = 0; j < i; ++j) {
        if (activated[j]) {
          double proba = tlp::randomNumber();

          if (proba < mu) { // rewire the edge to a random node chosen with preferential attachment
            pr = tlp::randomNumber();
            pr_sum = 0;
            uint sn = 0;

            while (pr_sum < pr && sn <= i) {
              pr_sum += (1 / double(graph->deg(nodes[sn]))) * a;
              ++sn;
            }

            graph->addEdge(nodes[i], nodes[--sn]);
          } else { // keep the edge
            graph->addEdge(nodes[i], nodes[j]);
          }
        }
      }

      // the new node becomes active
      activated[i] = true;

      // deactivate one of the previously m activated nodes
      a = 0;

      for (uint j = 0; j < i; ++j) {
        if (activated[j]) {
          a += 1 / double(graph->deg(nodes[j]));
        }
      }

      pr = tlp::randomNumber();
      pr_sum = 0;
      uint sn = 0;

      while (pr_sum < pr && sn < i) {
        if (activated[sn]) {
          pr_sum += a * (1 / double(graph->deg(nodes[sn])));
        }

        ++sn;
      }

      activated[--sn] = false;
    }

    return true;
  }
};

PLUGIN(KlemmEguiluzModel)
