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

#include <talipot/SimpleTest.h>
#include <talipot/GraphTest.h>
#include <talipot/Edge.h>

class SimpleTest : public tlp::GraphTest {
  public:
    PLUGININFORMATION("Simple", "Tulip team", "18/04/2012",
                      "Tests whether a graph is simple or not.<br/>An undirected graph "
                      "is simple if it has no loops and no more than one "
                      "edge between any unordered pair of vertices. "
                      "A directed graph is simple if has no loops and no more than one "
                      "edge between any ordered pair of vertices.",
                      "1.1", "Topological Test")
    SimpleTest(const tlp::PluginContext *context) : tlp::GraphTest(context) {
        addInParameter<bool>(
            "directed", "Indicates if the graph should be considered as directed or not.", "false");
        addOutParameter<uint>("loops count", "The number of found loops.");
        addOutParameter<uint>("parallel edges count", "The number of found parallel edges.");
    }

    bool test() override {
        bool directed = false;
        if (dataSet) {
            dataSet->get("directed", directed);
        }
        auto [loops, parallelEdges] = tlp::SimpleTest::getLoopsAndParallelEdges(graph, directed);
        if (dataSet) {
            dataSet->set("loops count", uint(loops.size()));
            dataSet->set("parallel edges count", uint(parallelEdges.size()));
        }
        return loops.empty() && parallelEdges.empty();
    }
};
PLUGIN(SimpleTest)

class MakeSimple : public tlp::Algorithm {
  public:
    PLUGININFORMATION("Make Simple", "Tulip team", "18/04/2012",
                      "Makes a graph simple.<br/>An undirected graph "
                      "is simple if it has no loops and no more than one "
                      "edge between any unordered pair of vertices. "
                      "A directed graph is simple if has no loops and no more than one "
                      "edge between any ordered pair of vertices.",
                      "1.1", "Topology Update")
    MakeSimple(const tlp::PluginContext *context) : tlp::Algorithm(context) {
        addInParameter<bool>(
            "directed", "Indicates if the graph should be considered as directed or not.", "false");
    }

    bool run() override {
        bool directed = false;
        if (dataSet) {
            dataSet->get("directed", directed);
        }
        tlp::SimpleTest::makeSimple(graph, directed);
        return true;
    }
};
PLUGIN(MakeSimple)
