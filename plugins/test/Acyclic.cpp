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

#include <talipot/AcyclicTest.h>
#include <talipot/GraphTest.h>

class AcyclicTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Acyclic", "Tulip team", "18/04/2012",
                      "Tests whether a graph is acyclic or not.", "1.0", "Topological Test")
    AcyclicTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::AcyclicTest::isAcyclic(graph);
    }
};
PLUGIN(AcyclicTest)

class MakeAcyclic : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Acyclic", "Tulip team", "18/04/2012", "Makes a graph acyclic.", "1.0",
                      "Topology Update")
    MakeAcyclic(const tlp::PluginContext *context) : tlp::Algorithm(context) {}

    bool run() override {
        std::vector<tlp::edge> edges;
        std::vector<tlp::SelfLoops> selfLoops;
        tlp::AcyclicTest::makeAcyclic(graph, edges, selfLoops);
        return true;
    }
};
PLUGIN(MakeAcyclic)
