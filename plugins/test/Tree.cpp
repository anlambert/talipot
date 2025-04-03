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

#include <talipot/TreeTest.h>
#include <talipot/GraphTest.h>
#include <talipot/GraphTools.h>
#include <talipot/BooleanProperty.h>

class DirectedTreeTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Directed Tree", "Tulip team", "18/04/2012",
                      "Tests whether a graph is a directed tree or not.", "1.0", "Topological Test")
    DirectedTreeTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::TreeTest::isTree(graph);
    }
};
PLUGIN(DirectedTreeTest)

class FreeTreeTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Free Tree", "Tulip team", "18/04/2012",
                      "Tests whether a graph is a free tree or not.", "1.0", "Topological Test")
    FreeTreeTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {}

    bool test() override {
        return tlp::TreeTest::isFreeTree(graph);
    }
};
PLUGIN(FreeTreeTest)

class MakeDirectedTree : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Directed Tree", "Tulip team", "18/04/2012",
                      "Makes a free tree a directed tree.", "1.0", "Topology Update")
    MakeDirectedTree(const tlp::PluginContext *context) : tlp::Algorithm(context) {}

    tlp::node root;

    bool check(std::string &errorMsg) override {
        if (!tlp::TreeTest::isFreeTree(graph)) {
            errorMsg = "The graph is not topologically a tree";
            return false;
        }

        for (auto n : graph->getBooleanProperty("viewSelection")->getNodesEqualTo(true)) {
            if (root.isValid()) {
                tlp::error() << "Only one root node must be selected." << std::endl;
                return false;
            }

            root = n;
        }

        if (!root.isValid()) {
            root = graphCenterHeuristic(graph);
        }

        return true;
    }

    bool run() override {
        tlp::TreeTest::makeRootedTree(graph, root);
        return true;
    }
};
PLUGIN(MakeDirectedTree)
