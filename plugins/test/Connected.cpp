/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/ConnectedTest.h>
#include <talipot/GraphTest.h>
#include <talipot/Edge.h>

class ConnectedTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Connected", "Tulip team", "18/04/2012",
                      "Tests whether a graph is connected or not.", "1.0", "Topological Test")
    ConnectedTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::ConnectedTest::isConnected(graph);
    }
};
PLUGIN(ConnectedTest)

class MakeConnected : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Connected", "Tulip team", "18/04/2012", "Makes a graph connected.",
                      "1.0", "Topology Update")
    MakeConnected(const tlp::PluginContext *context) : tlp::Algorithm(context) {}

    bool run() override {
        tlp::ConnectedTest::makeConnected(graph);
        return true;
    }
};
PLUGIN(MakeConnected)
