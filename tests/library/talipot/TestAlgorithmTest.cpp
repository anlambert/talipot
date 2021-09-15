/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
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

#include <talipot/SimpleTest.h>
#include <talipot/TreeTest.h>
#include <talipot/AcyclicTest.h>
#include <talipot/ConnectedTest.h>
#include <talipot/BiconnectedTest.h>

#include "TestAlgorithmTest.h"

using namespace std;
using namespace tlp;

static Graph *tlp_loadGraph(const std::string &filename) {
  DataSet dataSet;
  dataSet.set("file::filename", filename);
  Graph *sg = tlp::importGraph("TLP Import", dataSet);
  return sg;
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestAlgorithmTest);

//==========================================================
void TestAlgorithmTest::setUp() {
  graph = tlp::newGraph();
}
//==========================================================
void TestAlgorithmTest::tearDown() {
  delete graph;
}
//==========================================================
void TestAlgorithmTest::testSimple() {

  // build a simple graph
  node n1, n2, n3, n4;
  edge e1, e2, e3;
  n1 = graph->addNode();
  n2 = graph->addNode();
  n3 = graph->addNode();

  e1 = graph->addEdge(n1, n2);
  e2 = graph->addEdge(n2, n3);
  e2 = graph->addEdge(n3, n1);

  // not directed test
  CPPUNIT_ASSERT(SimpleTest::isSimple(graph));
  // directed test
  CPPUNIT_ASSERT(SimpleTest::isSimple(graph, true));

  // add new edge inverted from e3
  auto e = graph->addEdge(n1, n3);

  // not directed tests
  CPPUNIT_ASSERT(!SimpleTest::isSimple(graph));
  auto [loops, parallelEdges] = SimpleTest::getLoopsAndParallelEdges(graph);
  CPPUNIT_ASSERT(parallelEdges.size() == 1);
  CPPUNIT_ASSERT(loops.empty());
  CPPUNIT_ASSERT(parallelEdges[0] == e || parallelEdges[0] == e3);

  // directed tests
  CPPUNIT_ASSERT(SimpleTest::isSimple(graph, true));
  std::tie(loops, parallelEdges) = SimpleTest::getLoopsAndParallelEdges(graph, true);
  CPPUNIT_ASSERT(parallelEdges.empty());
  CPPUNIT_ASSERT(loops.empty());

  // add loop
  auto loop1 = graph->addEdge(n1, n1);

  // not directed tests
  CPPUNIT_ASSERT(!SimpleTest::isSimple(graph));
  std::tie(loops, parallelEdges) = SimpleTest::getLoopsAndParallelEdges(graph);
  CPPUNIT_ASSERT(parallelEdges.size() == 1);
  CPPUNIT_ASSERT(parallelEdges[0] == e || parallelEdges[0] == e3);
  CPPUNIT_ASSERT(loops.size() == 1);
  CPPUNIT_ASSERT(loops[0] == loop1);

  // directed tests
  CPPUNIT_ASSERT(!SimpleTest::isSimple(graph, true));
  std::tie(loops, parallelEdges) = SimpleTest::getLoopsAndParallelEdges(graph, true);
  CPPUNIT_ASSERT(parallelEdges.empty());
  CPPUNIT_ASSERT(loops.size() == 1);
  CPPUNIT_ASSERT(loops[0] == loop1);

  // add new loop and parallel edge
  auto loop2 = graph->addEdge(n1, n1);

  // not directed tests
  CPPUNIT_ASSERT(!SimpleTest::isSimple(graph));
  std::tie(loops, parallelEdges) = SimpleTest::getLoopsAndParallelEdges(graph);
  CPPUNIT_ASSERT(parallelEdges.size() == 2);
  CPPUNIT_ASSERT(parallelEdges[0] == e || parallelEdges[0] == e3);
  CPPUNIT_ASSERT(parallelEdges[1] == loop1 || parallelEdges[1] == loop2);
  CPPUNIT_ASSERT(loops.size() == 2);
  CPPUNIT_ASSERT(loops[0] == loop1 && loops[1] == loop2);

  // directed tests
  CPPUNIT_ASSERT(!SimpleTest::isSimple(graph, true));
  std::tie(loops, parallelEdges) = SimpleTest::getLoopsAndParallelEdges(graph, true);
  CPPUNIT_ASSERT(parallelEdges.size() == 1);
  CPPUNIT_ASSERT(parallelEdges[0] == loop1 || parallelEdges[0] == loop2);
  CPPUNIT_ASSERT(loops.size() == 2);
  CPPUNIT_ASSERT(loops[0] == loop1 && loops[1] == loop2);
}
//==========================================================
void TestAlgorithmTest::testFreeTree() {
  node n1 = graph->addNode();
  edge e = graph->addEdge(n1, n1);
  CPPUNIT_ASSERT(!TreeTest::isFreeTree(graph));
  graph->delEdge(e);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  edge e0 = graph->addEdge(n1, n2);
  edge e1 = graph->addEdge(n3, n1);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  node n4 = graph->addNode();
  CPPUNIT_ASSERT(!TreeTest::isFreeTree(graph));
  graph->addEdge(n4, n1);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  Graph *clone = graph->addCloneSubGraph();
  CPPUNIT_ASSERT(TreeTest::isFreeTree(clone));
  clone->reverse(e1);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  CPPUNIT_ASSERT(TreeTest::isFreeTree(clone));
  clone->reverse(e0);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(clone));
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  clone->delEdge(e1);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  CPPUNIT_ASSERT(!TreeTest::isFreeTree(clone));
  clone->delNode(n3);
  CPPUNIT_ASSERT(TreeTest::isFreeTree(graph));
  CPPUNIT_ASSERT(TreeTest::isFreeTree(clone));
}
//==========================================================
void TestAlgorithmTest::testTree() {
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  edge e0 = graph->addEdge(n1, n2);
  edge e1 = graph->addEdge(n1, n3);
  CPPUNIT_ASSERT(TreeTest::isTree(graph));
  node n4 = graph->addNode();
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  edge e2 = graph->addEdge(n4, n1);
  CPPUNIT_ASSERT(TreeTest::isTree(graph));
  Graph *clone = graph->addCloneSubGraph();
  CPPUNIT_ASSERT(TreeTest::isTree(clone));
  graph->reverse(e1);
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  CPPUNIT_ASSERT(!TreeTest::isTree(clone));
  clone->reverse(e0);
  CPPUNIT_ASSERT(!TreeTest::isTree(clone));
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  graph->reverse(e2);
  clone->delNode(n3);
  CPPUNIT_ASSERT(!TreeTest::isTree(graph));
  CPPUNIT_ASSERT(TreeTest::isTree(clone));
  // known bug test
  {
    graph->clear();
    node n1 = graph->addNode();
    node n2 = graph->addNode();
    node n3 = graph->addNode();
    graph->addEdge(n1, n2);
    graph->addEdge(n1, n3);
    graph->delNode(n3);
    CPPUNIT_ASSERT(TreeTest::isTree(graph));
  }
}
//==========================================================
void TestAlgorithmTest::testAcyclic() {
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  graph->addEdge(n1, n2);
  graph->addEdge(n1, n3);
  Graph *clone = graph->addCloneSubGraph();
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(graph));
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(clone));
  clone->addEdge(n2, n3);
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(graph));
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(clone));
  edge e2 = clone->addEdge(n3, n1);
  CPPUNIT_ASSERT(!AcyclicTest::isAcyclic(graph));
  CPPUNIT_ASSERT(!AcyclicTest::isAcyclic(clone));
  clone->reverse(e2);
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(graph));
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(clone));
  clone->delEdge(e2);
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(graph));
  CPPUNIT_ASSERT(AcyclicTest::isAcyclic(clone));
}
//==========================================================
void TestAlgorithmTest::testConnected() {
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  CPPUNIT_ASSERT(!ConnectedTest::isConnected(graph));
  edge e = graph->addEdge(n1, n2);
  CPPUNIT_ASSERT(!ConnectedTest::isConnected(graph));
  graph->addEdge(n3, n2);
  CPPUNIT_ASSERT(ConnectedTest::isConnected(graph));
  graph->delEdge(e);
  CPPUNIT_ASSERT(!ConnectedTest::isConnected(graph));
  vector<edge> addedEdge = ConnectedTest::makeConnected(graph);
  CPPUNIT_ASSERT(ConnectedTest::isConnected(graph));
  CPPUNIT_ASSERT_EQUAL(size_t(1), addedEdge.size());
  graph->delEdge(addedEdge[0]);
  CPPUNIT_ASSERT_EQUAL(2u, ConnectedTest::numberOfConnectedComponents(graph));

  graph->delEdges(graph->edges());
  node n4 = graph->addNode();
  graph->addEdge(n1, n1);
  graph->addEdge(n1, n2);
  graph->addEdge(n3, n4);
  CPPUNIT_ASSERT(!ConnectedTest::isConnected(graph));
  CPPUNIT_ASSERT_EQUAL(2u, ConnectedTest::numberOfConnectedComponents(graph));
}
//==========================================================
const std::string GRAPHPATH = "./DATA/graphs/";

void TestAlgorithmTest::testBiconnected() {
  node n[10];
  edge e[10];

  for (int i = 0; i < 4; ++i) {
    n[i] = graph->addNode();
  }

  for (int i = 0; i < 4; ++i) {
    e[i] = graph->addEdge(n[i], n[(i + 1) % 4]);
  }

  CPPUNIT_ASSERT(BiconnectedTest::isBiconnected(graph));
  graph->delEdge(e[0]);
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  e[0] = graph->addEdge(n[0], n[2]);
  n[4] = graph->addNode();
  e[4] = graph->addEdge(n[4], n[1]);
  e[5] = graph->addEdge(n[4], n[2]);
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  e[6] = graph->addEdge(n[4], n[0]);
  CPPUNIT_ASSERT(BiconnectedTest::isBiconnected(graph));
  n[5] = graph->addNode();
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  e[7] = graph->addEdge(n[1], n[5]);
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  // Root separator
  graph->clear();

  for (int i = 0; i < 5; ++i) {
    n[i] = graph->addNode();
  }

  e[0] = graph->addEdge(n[0], n[1]);
  e[1] = graph->addEdge(n[0], n[2]);
  e[2] = graph->addEdge(n[1], n[2]);

  e[3] = graph->addEdge(n[0], n[3]);
  e[4] = graph->addEdge(n[0], n[4]);
  e[5] = graph->addEdge(n[3], n[4]);

  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  e[6] = graph->addEdge(n[2], n[4]);
  CPPUNIT_ASSERT(BiconnectedTest::isBiconnected(graph));
  // Test makeBiconnected
  graph->delEdge(e[6]);
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));
  vector<edge> addedEdges = BiconnectedTest::makeBiconnected(graph);
  CPPUNIT_ASSERT(BiconnectedTest::isBiconnected(graph));
  CPPUNIT_ASSERT(addedEdges.size() == 1);
  graph->delEdge(addedEdges[0]);
  CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(graph));

  Graph *tmpGraph;

  for (uint i = 0; i < 5; ++i) {
    tmpGraph = tlp_loadGraph(GRAPHPATH + "planar/unbiconnected.tlp");
    CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(tmpGraph));
    vector<edge> vEdges = BiconnectedTest::makeBiconnected(tmpGraph);
    CPPUNIT_ASSERT(BiconnectedTest::isBiconnected(tmpGraph));

    for (auto e : vEdges) {
      tmpGraph->delEdge(e, true);
    }

    CPPUNIT_ASSERT(!BiconnectedTest::isBiconnected(tmpGraph));
    delete tmpGraph;
  }
}
//==========================================================
void TestAlgorithmTest::testBridges() {
  // graph examples are taken from https://www.geeksforgeeks.org/bridge-in-a-graph/
  vector<node> nodes = graph->addNodes(5);
  vector<edge> edges = graph->addEdges({{nodes[0], nodes[1]},
                                        {nodes[1], nodes[2]},
                                        {nodes[2], nodes[0]},
                                        {nodes[0], nodes[3]},
                                        {nodes[3], nodes[4]}});
  vector<edge> bridges = ConnectedTest::computeBridges(graph);
  sort(bridges.begin(), bridges.end());
  CPPUNIT_ASSERT_EQUAL(vector({edges[3], edges.back()}), bridges);

  graph->clear();
  nodes = graph->addNodes(7);
  edges = graph->addEdges({{nodes[0], nodes[1]},
                           {nodes[1], nodes[2]},
                           {nodes[2], nodes[0]},
                           {nodes[1], nodes[3]},
                           {nodes[3], nodes[5]},
                           {nodes[5], nodes[4]},
                           {nodes[4], nodes[1]},
                           {nodes[1], nodes[6]}});
  bridges = ConnectedTest::computeBridges(graph);
  CPPUNIT_ASSERT_EQUAL(vector({edges.back()}), bridges);

  graph->clear();
  nodes = graph->addNodes(4);
  edges = graph->addEdges({{nodes[0], nodes[1]}, {nodes[1], nodes[2]}, {nodes[2], nodes[3]}});
  bridges = ConnectedTest::computeBridges(graph);
  sort(bridges.begin(), bridges.end());
  CPPUNIT_ASSERT_EQUAL(edges, bridges);
}
