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

#include <talipot/PlanarityTest.h>
#include <talipot/GraphTest.h>

class PlanarityTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Planar", "Tulip team", "18/04/2012",
                      "Tests whether a graph is planar or not.", "1.0", "Topological Test")
    PlanarityTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::PlanarityTest::isPlanar(graph);
    }
};
PLUGIN(PlanarityTest)

class PlanarEmbeddingTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Planar Embedding", "Tulip team", "18/04/2012",
                      "Tests whether a graph is a planar embedding or not.", "1.0",
                      "Topological Test")
    PlanarEmbeddingTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::PlanarityTest::isPlanarEmbedding(graph);
    }
};
PLUGIN(PlanarEmbeddingTest)

class MakePlanarEmbedding : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Planar Embedding", "Tulip team", "18/04/2012",
                      "Makes the graph a planar embedding if it is planar.", "1.0",
                      "Topology Update")
    MakePlanarEmbedding(const tlp::PluginContext *context) : tlp::Algorithm(context) {}

    bool run() override {
        return tlp::PlanarityTest::planarEmbedding(graph);
    }

    bool check(std::string &errorMsg) override {
        bool result = tlp::PlanarityTest::isPlanar(graph);
        if (!result) {
            errorMsg = "The graph must be planar";
        }
        return result;
    }
};
PLUGIN(MakePlanarEmbedding)
