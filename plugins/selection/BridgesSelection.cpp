/**
 *
 * Copyright (C) 2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/BooleanProperty.h>
#include <talipot/PropertyAlgorithm.h>
#include <talipot/ConnectedTest.h>

using namespace std;
using namespace tlp;

class BridgesSelection : public BooleanAlgorithm {
public:
  PLUGININFORMATION("Bridges Selection", "Antoine Lambert", "09/2021",
                    "Selects bridges in a graph.<br/>A bridge is defined as an edge which, "
                    "when removed, makes the graph disconnected (or more precisely, "
                    "increases the number of connected components in the graph).",
                    " 1.0 ", "Selection")
  BridgesSelection(const PluginContext *context) : BooleanAlgorithm(context) {
    addOutParameter<uint>("#bridges", "The number of bridges selected");
  }

  bool run() override {
    vector<edge> bridges = ConnectedTest::computeBridges(graph);
    for (auto e : bridges) {
      result->setEdgeValue(e, true);
    }
    if (dataSet) {
      dataSet->set<uint>("#bridges", bridges.size());
    }
    return true;
  }
};

PLUGIN(BridgesSelection)
