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

#include "GraphPropertyTest.h"

using namespace std;
using namespace tlp;

tlp::Graph *nullGraph = nullptr;

CPPUNIT_TEST_SUITE_REGISTRATION(GraphPropertyTest);

//==========================================================
void GraphPropertyTest::setUp() {
  graph = tlp::newGraph();
  // add three nodes
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  // add three edges
  graph->addEdge(n2, n3);
  graph->addEdge(n1, n2);
  graph->addEdge(n3, n1);
}
//==========================================================
void GraphPropertyTest::tearDown() {
  delete graph;
}
//==========================================================
void GraphPropertyTest::testDestroyGraph() {
  // build the hierarchy
  Graph *g1 = graph->addCloneSubGraph("G1");
  Graph *g2 = graph->addCloneSubGraph("G2");
  Graph *meta1 = graph->addSubGraph("META1");
  GraphProperty *proxy1 = meta1->getLocalGraphProperty("viewMetaGraph");
  node mnode1 = meta1->addNode();
  node mnode2 = meta1->addNode();
  proxy1->setNodeValue(mnode1, g1);
  proxy1->setNodeValue(mnode2, g2);
  graph->delSubGraph(g2);
  CPPUNIT_ASSERT_EQUAL(nullGraph, proxy1->getNodeValue(mnode2));
  CPPUNIT_ASSERT_EQUAL(g1, proxy1->getNodeValue(mnode1));
  graph->delSubGraph(g1);
  CPPUNIT_ASSERT_EQUAL(nullGraph, proxy1->getNodeValue(mnode2));
  CPPUNIT_ASSERT_EQUAL(nullGraph, proxy1->getNodeValue(mnode1));
}
//==========================================================
void GraphPropertyTest::testSetGet() {
  // build the hierarchy
  Graph *g1 = graph->addCloneSubGraph("G1");
  Graph *g2 = graph->addCloneSubGraph("G2");
  Graph *g3 = graph->addCloneSubGraph("G3");
  Graph *meta1 = graph->addSubGraph("META1");
  GraphProperty *proxy1 = meta1->getLocalGraphProperty("viewMetaGraph");
  node mnode1 = meta1->addNode();
  node mnode2 = meta1->addNode();
  proxy1->setNodeValue(mnode1, g1);
  proxy1->setNodeValue(mnode2, g2);
  proxy1->setNodeValue(mnode2, g3);
  graph->delSubGraph(g2);
  CPPUNIT_ASSERT_EQUAL(g3, proxy1->getNodeValue(mnode2));
  CPPUNIT_ASSERT_EQUAL(g1, proxy1->getNodeValue(mnode1));
}
//==========================================================
void GraphPropertyTest::testSetAll() {
  // build the hierarchy
  Graph *g1 = graph->addCloneSubGraph("G1");
  Graph *g2 = graph->addCloneSubGraph("G2");
  Graph *g3 = graph->addCloneSubGraph("G3");
  Graph *meta1 = graph->addSubGraph("META1");
  GraphProperty proxy(meta1);
  node mnode1 = meta1->addNode();
  node mnode2 = meta1->addNode();
  node mnode3 = meta1->addNode();
  proxy.setAllNodeValue(g3);
  proxy.setNodeValue(mnode1, g1);
  proxy.setNodeValue(mnode2, g2);
  CPPUNIT_ASSERT_EQUAL(g1, proxy.getNodeValue(mnode1));
  CPPUNIT_ASSERT_EQUAL(g2, proxy.getNodeValue(mnode2));
  CPPUNIT_ASSERT_EQUAL(g3, proxy.getNodeValue(mnode3));
  graph->delSubGraph(g3);
  CPPUNIT_ASSERT_EQUAL(g1, proxy.getNodeValue(mnode1));
  CPPUNIT_ASSERT_EQUAL(g2, proxy.getNodeValue(mnode2));
  CPPUNIT_ASSERT_EQUAL(nullGraph, proxy.getNodeValue(mnode3));
  proxy.setAllNodeValue(nullptr);
  graph->delSubGraph(g1);
  graph->delSubGraph(g2);
}
