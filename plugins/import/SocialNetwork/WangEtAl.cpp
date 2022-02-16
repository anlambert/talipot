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
 * L.Wang, F. Du, H. P. Dai, and Y. X. Sun.
 * Random pseudofractal scale-free networks with small-world effect.
 * The European Physical Journal B - Condensed Matter and Complex Systems, 53, 361-366, (2006).
 *
 */
struct WangEtAl : public ImportModule {
  PLUGININFORMATION("Wang et al. Model", "Arnaud Sallaberry", "21/02/2011",
                    "Randomly generates a small world graph using the model described "
                    "in<br/>L.Wang, F. Du, H. P. Dai, and Y. X. Sun.<br/><b>Random pseudofractal "
                    "scale-free networks with small-world effect.</b><br/>The European Physical "
                    "Journal B - Condensed Matter and Complex Systems, 53, 361-366, (2006).",
                    "1.0", "Social network")

  WangEtAl(PluginContext *context) : ImportModule(context) {
    addInParameter<uint>("nodes", paramHelp[0].data(), "300");
  }

  bool importGraph() override {

    uint n = 300;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    graph->addNodes(n);
    const vector<node> &nodes = graph->nodes();

    graph->reserveEdges(2 * n - 3);

    vector<edge> e(2 * n - 3);
    e[0] = graph->addEdge(nodes[0], nodes[1]);
    uint nbe = 1;

    for (unsigned i = 2; i < n; ++i) {
      if (i % 100 == 0) {
        if (pluginProgress->progress(i, n) != ProgressState::TLP_CONTINUE) {
          return pluginProgress->state() != ProgressState::TLP_CANCEL;
        }
      }

      int id = tlp::randomInteger(nbe - 1);
      auto [src, tgt] = graph->ends(e[id]);
      e[nbe] = graph->addEdge(src, nodes[i]);
      e[nbe + 1] = graph->addEdge(tgt, nodes[i]);
      nbe += 2;
    }

    return true;
  }
};

PLUGIN(WangEtAl)
