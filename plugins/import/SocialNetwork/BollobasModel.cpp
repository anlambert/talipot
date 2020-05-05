/**
 *
 * Copyright (C) 2019-2020  The Talipot developers
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
#include <talipot/TlpTools.h>
#include <talipot/PluginProgress.h>
#include <talipot/Graph.h>

using namespace std;
using namespace tlp;

static const char *paramHelp[] = {
    // nodes
    "This parameter defines the amount of nodes used to build the scale-free graph.",

    // d
    "Minimum degree."};

/**
 *
 * This plugin is an implementation of the model
 * described in
 * B. Bollobas, O.M Riordan, J. Spencer and G. Tusnady.
 * The Degree Sequence of a Scale-Free Random Graph Process.
 * RSA: Random Structures & Algorithms, 18, 279 (2001)
 *
 * first implemented by :
 * Vladimir Batagelj and Ulrik Brandes. Efficient Generation of Large Random Networks, 2005.
 *
 */
struct BollobasModel : public ImportModule {
  PLUGININFORMATION("Bollobas et al. Model", "Arnaud Sallaberry", "21/02/2011",
                    "Randomly generates a scale-free graph using the model described in<br/>B. "
                    "Bollobas, O.M Riordan, J. Spencer and G. Tusnady.<br/><b>The Degree Sequence "
                    "of a Scale-Free Random Graph Process.</b><br/>RSA: Random Structures & "
                    "Algorithms, 18, 279 (2001)",
                    "1.1", "Social network")

  BollobasModel(PluginContext *context) : ImportModule(context) {
    addInParameter<unsigned int>("nodes", paramHelp[0], "2000");
    addInParameter<unsigned int>("minimum degree", paramHelp[1], "4");
  }

  bool importGraph() override {
    unsigned int n = 2000;
    unsigned int d = 4;

    if (dataSet != nullptr) {
      dataSet->get("nodes", n);
      dataSet->get("minimum degree", d);
    }

    // check arguments
    if (d > n) {
      pluginProgress->setError("The minimum degree cannot be greater than the number of nodes.");
      return false;
    }

    pluginProgress->showPreview(false);
    tlp::initRandomSequence();

    vector<unsigned int> M(2 * n * d);
    graph->addNodes(n);

    for (unsigned int v = 0; v < n; ++v) {
      for (unsigned int i = 0; i < d; ++i) {
        M[2 * (v * d + i)] = v;
        int r = tlp::randomInteger(2 * (v * d + i) + 1);
        M[2 * (v * d + i) + 1] = M[r];
      }

      if (v % 100 == 0) {
        if (pluginProgress->progress(v, n * (d + 1)) != TLP_CONTINUE) {
          return pluginProgress->state() != TLP_CANCEL;
        }
      }
    }

    graph->reserveEdges(n * d);

    const vector<node> &nodes = graph->nodes();

    for (unsigned int i = 0; i < (n * d); ++i) {
      graph->addEdge(nodes[M[2 * i]], nodes[M[2 * i + 1]]);

      if (i % 100 == 0) {
        if (pluginProgress->progress(i, n * (d + 1)) != TLP_CONTINUE) {
          return pluginProgress->state() != TLP_CANCEL;
        }
      }
    }

    return true;
  }
};

PLUGIN(BollobasModel)
