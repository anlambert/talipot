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

#include <talipot/BiconnectedTest.h>
#include <talipot/GraphTest.h>
#include <talipot/Edge.h>

class BiconnectedTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Biconnected", "Tulip team", "18/04/2012",
                      "Tests whether a graph is biconnected or not.", "1.0", "Topological Test")
    BiconnectedTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::BiconnectedTest::isBiconnected(graph);
    }
};
PLUGIN(BiconnectedTest)

class MakeBiconnected : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Biconnected", "Tulip team", "18/04/2012", "Makes a graph biconnected.",
                      "1.0", "Topology Update")
    MakeBiconnected(const tlp::PluginContext *context) : tlp::Algorithm(context) {}

    bool run() override {
        tlp::BiconnectedTest::makeBiconnected(graph);
        return true;
    }
};
PLUGIN(MakeBiconnected)
