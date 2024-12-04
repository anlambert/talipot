/**
 *
 * Copyright (C) 2021-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/Graph.h>
#include <talipot/Iterator.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

using namespace std;

class GraphTraversalTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(GraphTraversalTest);
  CPPUNIT_TEST(testBFS);
  CPPUNIT_TEST(testBFSEdges);
  CPPUNIT_TEST(testDFS);
  CPPUNIT_TEST(testDFSWithCallbacks);
  CPPUNIT_TEST(testDFSEdges);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
    graph = tlp::newGraph();
    nodes = graph->addNodes(13);
    graph->addEdges({{nodes[0], nodes[1]},
                     {nodes[0], nodes[7]},
                     {nodes[0], nodes[8]},
                     {nodes[0], nodes[9]},
                     {nodes[1], nodes[2]},
                     {nodes[1], nodes[3]},
                     {nodes[1], nodes[6]},
                     {nodes[3], nodes[4]},
                     {nodes[4], nodes[5]},
                     {nodes[9], nodes[10]},
                     {nodes[9], nodes[12]},
                     {nodes[10], nodes[11]}});
  }

  void tearDown() {
    delete graph;
  }

  void testDFS() {
    /*      O________
     *     /   \  \  \
     *    1___  7  8  9
     *   / \  \      / \
     *  2   3  6    10 12
     *      |        |
     *      4       11
     *      |
     *      5
     */
    vector<tlp::node> dfs = {nodes[0], nodes[1], nodes[2], nodes[3],  nodes[4],  nodes[5], nodes[6],
                             nodes[7], nodes[8], nodes[9], nodes[10], nodes[11], nodes[12]};
    CPPUNIT_ASSERT_EQUAL(dfs, graph->dfs());

    vector<tlp::node> dfsRootedDirected = {nodes[1], nodes[2], nodes[3],
                                           nodes[4], nodes[5], nodes[6]};
    CPPUNIT_ASSERT_EQUAL(dfsRootedDirected, graph->dfs(nodes[1], true));
  }

  void testDFSWithCallbacks() {
    vector<tlp::node> dfs = {nodes[0], nodes[1], nodes[2], nodes[3],  nodes[4],  nodes[5], nodes[6],
                             nodes[7], nodes[8], nodes[9], nodes[10], nodes[11], nodes[12]};

    vector<tlp::node> dfsOutVisit = {nodes[2],  nodes[5], nodes[4], nodes[3],  nodes[6],
                                     nodes[1],  nodes[7], nodes[8], nodes[11], nodes[10],
                                     nodes[12], nodes[9], nodes[0]};

    vector<tlp::node> inVisitOrder;
    vector<tlp::node> outVisitOrder;

    graph->dfs(
        [&](const tlp::Graph *, tlp::node n) {
          inVisitOrder.push_back(n);
          return true;
        },
        [&](const tlp::Graph *, tlp::node n) {
          outVisitOrder.push_back(n);
          return true;
        });

    CPPUNIT_ASSERT_EQUAL(dfs, inVisitOrder);
    CPPUNIT_ASSERT_EQUAL(dfsOutVisit, outVisitOrder);

    vector<tlp::node> dfsRootedDirected = {nodes[1], nodes[2], nodes[3],
                                           nodes[4], nodes[5], nodes[6]};
    vector<tlp::node> dfsRootedDirectedOutVisit = {nodes[2], nodes[5], nodes[4],
                                                   nodes[3], nodes[6], nodes[1]};

    inVisitOrder.clear();
    outVisitOrder.clear();

    graph->dfs(
        nodes[1],
        [&](const tlp::Graph *, tlp::node n) {
          inVisitOrder.push_back(n);
          return true;
        },
        [&](const tlp::Graph *, tlp::node n) {
          outVisitOrder.push_back(n);
          return true;
        },
        true);
    CPPUNIT_ASSERT_EQUAL(dfsRootedDirected, inVisitOrder);
    CPPUNIT_ASSERT_EQUAL(dfsRootedDirectedOutVisit, outVisitOrder);
  }

  void testDFSEdges() {
    vector<tlp::edge> dfsEdges = {
        graph->existEdge(nodes[0], nodes[1]),   graph->existEdge(nodes[1], nodes[2]),
        graph->existEdge(nodes[1], nodes[3]),   graph->existEdge(nodes[3], nodes[4]),
        graph->existEdge(nodes[4], nodes[5]),   graph->existEdge(nodes[1], nodes[6]),
        graph->existEdge(nodes[0], nodes[7]),   graph->existEdge(nodes[0], nodes[8]),
        graph->existEdge(nodes[0], nodes[9]),   graph->existEdge(nodes[9], nodes[10]),
        graph->existEdge(nodes[10], nodes[11]), graph->existEdge(nodes[9], nodes[12]),
    };
    CPPUNIT_ASSERT_EQUAL(dfsEdges, graph->dfsEdges());

    vector<tlp::edge> dfsEdgesRootedDirected = {
        graph->existEdge(nodes[1], nodes[2]), graph->existEdge(nodes[1], nodes[3]),
        graph->existEdge(nodes[3], nodes[4]), graph->existEdge(nodes[4], nodes[5]),
        graph->existEdge(nodes[1], nodes[6]),
    };
    CPPUNIT_ASSERT_EQUAL(dfsEdgesRootedDirected, graph->dfsEdges(nodes[1], true));
  }

  void testBFS() {
    /*      O________
     *     /   \  \  \
     *    1___  2  3  4
     *   / \  \      / \
     *  5   6  7    8   9
     *      |       |
     *     10      11
     *      |
     *     12
     *
     * 0 -> 0
     * 1 -> 1
     * 2 -> 7
     * 3 -> 8
     * 4 -> 9
     * 5 -> 2
     * 6 -> 3
     * 7 -> 6
     * 8 -> 10
     * 9 -> 12
     * 10 -> 4
     * 11 -> 11
     * 12 -> 5
     */
    vector<tlp::node> bfs = {nodes[0], nodes[1],  nodes[7], nodes[8],  nodes[9],
                             nodes[2], nodes[3],  nodes[6], nodes[10], nodes[12],
                             nodes[4], nodes[11], nodes[5]};
    CPPUNIT_ASSERT_EQUAL(bfs, graph->bfs());

    vector<tlp::node> bfsRootedDirected = {nodes[1], nodes[2], nodes[3],
                                           nodes[6], nodes[4], nodes[5]};
    CPPUNIT_ASSERT_EQUAL(bfsRootedDirected, graph->bfs(nodes[1], true));
  }

  void testBFSEdges() {
    vector<tlp::edge> bfsEdges = {
        graph->existEdge(nodes[0], nodes[1]),   graph->existEdge(nodes[0], nodes[7]),
        graph->existEdge(nodes[0], nodes[8]),   graph->existEdge(nodes[0], nodes[9]),
        graph->existEdge(nodes[1], nodes[2]),   graph->existEdge(nodes[1], nodes[3]),
        graph->existEdge(nodes[1], nodes[6]),   graph->existEdge(nodes[9], nodes[10]),
        graph->existEdge(nodes[9], nodes[12]),  graph->existEdge(nodes[3], nodes[4]),
        graph->existEdge(nodes[10], nodes[11]), graph->existEdge(nodes[4], nodes[5]),
    };
    CPPUNIT_ASSERT_EQUAL(bfsEdges, graph->bfsEdges());

    vector<tlp::edge> bfsEdgesRootedDirected = {
        graph->existEdge(nodes[1], nodes[2]), graph->existEdge(nodes[1], nodes[3]),
        graph->existEdge(nodes[1], nodes[6]), graph->existEdge(nodes[3], nodes[4]),
        graph->existEdge(nodes[4], nodes[5]),
    };
    CPPUNIT_ASSERT_EQUAL(bfsEdgesRootedDirected, graph->bfsEdges(nodes[1], true));
  }

private:
  tlp::Graph *graph;
  std::vector<tlp::node> nodes;
};

CPPUNIT_TEST_SUITE_REGISTRATION(GraphTraversalTest);
