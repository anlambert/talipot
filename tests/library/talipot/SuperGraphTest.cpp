/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "SuperGraphTest.h"
#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(SuperGraphTest);

//==========================================================
void SuperGraphTest::setUp() {
#ifndef NDEBUG
  // choose the seed to ease the debugging
  tlp::setSeedOfRandomSequence(1);
  tlp::initRandomSequence();
#endif
  graph = tlp::newGraph();
}
//==========================================================
void SuperGraphTest::tearDown() {
  delete graph;
}
//==========================================================
void SuperGraphTest::build(uint nbNodes, uint edgeRatio) {

  uint NB_ADD = nbNodes;
  uint EDGE_RATIO = edgeRatio;

  vector<node> nodes = graph->addNodes(nbNodes);

  uint NB_EDGES = EDGE_RATIO * NB_ADD;

  for (uint i = 0; i < NB_EDGES; ++i) {
    graph->addEdge(graph->getRandomNode(), graph->getRandomNode());
  }
}
//==========================================================
void SuperGraphTest::testIterators() {
  size_t NB_NODES = 100;
  uint EDGE_RATIO = 100;
  size_t NB_EDGES = EDGE_RATIO * NB_NODES;

  vector<node> nodes = graph->addNodes(NB_NODES);
  vector<edge> edges;

  for (uint i = 0; i < NB_EDGES; ++i) {
    edges.push_back(graph->addEdge(graph->getRandomNode(), graph->getRandomNode()));
  }

  CPPUNIT_ASSERT_EQUAL(nodes, graph->nodes());
  CPPUNIT_ASSERT_EQUAL(NB_NODES, graph->nodes().size());

  CPPUNIT_ASSERT_EQUAL(edges, graph->edges());
  CPPUNIT_ASSERT_EQUAL(NB_EDGES, graph->edges().size());

  graph->clear();
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  edge e1 = graph->addEdge(n1, n1); // loop
  edge e2 = graph->addEdge(n1, n2);
  edge e3 = graph->addEdge(n2, n1); // parallel edge
  edge e4 = graph->addEdge(n1, n3);

  CPPUNIT_ASSERT_EQUAL(vector({n1, n2, n3}), iteratorVector(graph->getOutNodes(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({n1, n2}), iteratorVector(graph->getInNodes(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({n1, n1, n2, n2, n3}), iteratorVector(graph->getInOutNodes(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({e1, e2, e4}), iteratorVector(graph->getOutEdges(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({e1, e3}), iteratorVector(graph->getInEdges(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({e1, e1, e2, e3, e4}), iteratorVector(graph->getInOutEdges(n1)));

  CPPUNIT_ASSERT_EQUAL(vector({e1, e1, e2, e3, e4}), graph->incidence(n1));
}
//==========================================================
void degreeCheck(Graph *graph) {

  for (auto n : graph->nodes()) {
    uint outdeg = iteratorCount(graph->getOutEdges(n));
    uint indeg = iteratorCount(graph->getInEdges(n));
    uint deg = iteratorCount(graph->getInOutEdges(n));

    CPPUNIT_ASSERT_EQUAL(indeg, graph->indeg(n));
    CPPUNIT_ASSERT_EQUAL(outdeg, graph->outdeg(n));
    CPPUNIT_ASSERT_EQUAL(deg, graph->deg(n));
    CPPUNIT_ASSERT_EQUAL(deg, indeg + outdeg);
  }
}
//==========================================================
void SuperGraphTest::testDegree() {
  build(100, 100);
  degreeCheck(graph);
  Graph *gr = graph->addCloneSubGraph();
  CPPUNIT_ASSERT_EQUAL(graph, gr->getSuperGraph());
  Graph *gr1 = gr->addCloneSubGraph("gr1", true);
  CPPUNIT_ASSERT_EQUAL(graph, gr1->getSuperGraph());
  Graph *gr2 = gr1->addCloneSubGraph();
  CPPUNIT_ASSERT_EQUAL(gr1, gr2->getSuperGraph());

  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);

  for (auto e : graph->edges()) {
    graph->reverse(e);
  }

  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);

  for (auto e : gr2->getEdges()) {
    gr2->reverse(e);
  }

  degreeCheck(graph);
  degreeCheck(gr);
  degreeCheck(gr1);
  degreeCheck(gr2);

  gr1->delSubGraph(gr2);
  graph->delSubGraph(gr1);
  graph->delSubGraph(gr);

  graph->clear();
  gr1 = graph->addCloneSubGraph();
  gr2 = gr1->addCloneSubGraph();
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  node n3 = graph->addNode();
  edge e = graph->addEdge(n1, n2);
  gr2->addNode(n1);
  gr2->addNode(n2);
  gr2->addNode(n3);
  gr2->addEdge(e);
  gr2->addEdge(graph->addEdge(n1, n3));
  gr2->delNode(n3);
  CPPUNIT_ASSERT_EQUAL(3u, graph->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2u, graph->numberOfEdges());
  CPPUNIT_ASSERT_EQUAL(3u, gr1->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2u, gr1->numberOfEdges());
  CPPUNIT_ASSERT_EQUAL(2u, gr2->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(1u, gr2->numberOfEdges());
  CPPUNIT_ASSERT_EQUAL(0u, graph->indeg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, graph->indeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, graph->indeg(n3));
  CPPUNIT_ASSERT_EQUAL(2u, graph->outdeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, graph->outdeg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, graph->deg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, graph->deg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, graph->deg(n3));
  CPPUNIT_ASSERT_EQUAL(0u, gr1->indeg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->indeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->indeg(n3));
  CPPUNIT_ASSERT_EQUAL(2u, gr1->outdeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, gr1->outdeg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, gr1->deg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->deg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->deg(n3));
  CPPUNIT_ASSERT_EQUAL(0u, gr2->indeg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, gr2->indeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, gr2->outdeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, gr2->outdeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, gr2->deg(n1));
  CPPUNIT_ASSERT_EQUAL(1u, gr2->deg(n2));
  graph->setEnds(e, n3, n1);
  CPPUNIT_ASSERT_EQUAL(3u, graph->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2u, graph->numberOfEdges());
  CPPUNIT_ASSERT_EQUAL(3u, gr1->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(2u, gr1->numberOfEdges());
  CPPUNIT_ASSERT_EQUAL(2u, gr2->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(0u, gr2->numberOfEdges());
  CPPUNIT_ASSERT(graph->isElement(e));
  CPPUNIT_ASSERT(gr1->isElement(e));
  CPPUNIT_ASSERT(!gr2->isElement(e));
  CPPUNIT_ASSERT_EQUAL(1u, graph->indeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, graph->indeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, graph->indeg(n3));
  CPPUNIT_ASSERT_EQUAL(1u, graph->outdeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, graph->outdeg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, graph->deg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, graph->deg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, graph->deg(n3));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->indeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, gr1->indeg(n2));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->indeg(n3));
  CPPUNIT_ASSERT_EQUAL(1u, gr1->outdeg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, gr1->outdeg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, gr1->deg(n1));
  CPPUNIT_ASSERT_EQUAL(0u, gr1->deg(n2));
  CPPUNIT_ASSERT_EQUAL(2u, gr1->deg(n3));
}
//==========================================================
void SuperGraphTest::testAddDel() {
  uint NB_ADD = 100;
  uint EDGE_RATIO = 100;
  uint NB_EDGES = EDGE_RATIO * NB_ADD;

  vector<node> nodes;
  vector<edge> edges;

  for (uint i = 0; i < NB_ADD; ++i) {
    nodes.push_back(graph->addNode());
    CPPUNIT_ASSERT(graph->isElement(nodes[i]));
  }

  CPPUNIT_ASSERT_EQUAL(NB_ADD, graph->numberOfNodes());

  graph->delNodes(nodes);

  CPPUNIT_ASSERT_EQUAL(0u, graph->numberOfNodes());

  nodes = graph->addNodes(NB_ADD);

  for (uint i = 0; i < NB_ADD; ++i) {
    CPPUNIT_ASSERT(graph->isElement(nodes[i]));
  }

  CPPUNIT_ASSERT_EQUAL(NB_ADD, graph->numberOfNodes());

  for (uint i = 0; i < NB_EDGES; ++i) {
    edges.push_back(graph->addEdge(graph->getRandomNode(), graph->getRandomNode()));
    CPPUNIT_ASSERT(graph->isElement(edges[i]));
  }

  CPPUNIT_ASSERT_EQUAL(NB_EDGES, graph->numberOfEdges());

  for (uint i = 0; i < NB_EDGES; ++i) {
    graph->delEdge(edges[i]);
    CPPUNIT_ASSERT(!graph->isElement(edges[i]));
  }

  CPPUNIT_ASSERT_EQUAL(0u, graph->numberOfEdges());

  vector<pair<node, node>> ends;

  for (uint i = 0; i < NB_EDGES; ++i) {
    ends.push_back({graph->getRandomNode(), graph->getRandomNode()});
  }

  edges = graph->addEdges(ends);
  CPPUNIT_ASSERT_EQUAL(NB_EDGES, graph->numberOfEdges());

  for (uint i = 0; i < NB_ADD; ++i) {

    for (auto e : graph->incidence(nodes[i])) {
      CPPUNIT_ASSERT(graph->isElement(e));
    }

    graph->delNode(nodes[i]);
    CPPUNIT_ASSERT(!graph->isElement(nodes[i]));
  }

  CPPUNIT_ASSERT_EQUAL(0u, graph->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(0u, graph->numberOfEdges());
}
//==========================================================
void SuperGraphTest::testClear() {
  build(100, 100);
  graph->clear();
  CPPUNIT_ASSERT(graph->numberOfNodes() == 0);
  CPPUNIT_ASSERT(graph->numberOfEdges() == 0);
}
//==========================================================
void SuperGraphTest::testOrderEdgeAndSwap() {
  size_t NB_NODES = 100;
  size_t NB_EDGES = NB_NODES - 1;

  vector<node> nodes = graph->addNodes(NB_NODES);
  vector<edge> edges;

  for (uint i = 0; i < NB_EDGES; ++i) {
    edges.push_back(graph->addEdge(nodes[0], nodes[i + 1]));
  }

  CPPUNIT_ASSERT_EQUAL(edges, iteratorVector(graph->getInOutEdges(nodes[0])));
  CPPUNIT_ASSERT_EQUAL(edges, graph->incidence(nodes[0]));

  // change edges order
  for (uint j = 0; j < NB_EDGES; ++j) {
    uint u = randomUnsignedInteger(NB_EDGES - 1);
    uint v = randomUnsignedInteger(NB_EDGES - 1);
    std::swap(edges[u], edges[v]);
  }

  graph->setEdgeOrder(nodes[0], edges);

  CPPUNIT_ASSERT_EQUAL(edges, iteratorVector(graph->getInOutEdges(nodes[0])));
  CPPUNIT_ASSERT_EQUAL(edges, graph->incidence(nodes[0]));

  // swap two edges
  for (uint j = 0; j < NB_EDGES; ++j) {
    uint u = randomUnsignedInteger(NB_EDGES - 1);
    uint v = randomUnsignedInteger(NB_EDGES - 1);
    graph->swapEdgeOrder(nodes[0], edges[u], edges[v]);
    std::swap(edges[u], edges[v]);
  }

  CPPUNIT_ASSERT_EQUAL(edges, iteratorVector(graph->getInOutEdges(nodes[0])));
  CPPUNIT_ASSERT_EQUAL(edges, graph->incidence(nodes[0]));
}
//==========================================================
void SuperGraphTest::testDeleteSubgraph() {
  Graph *g1, *g2, *g3, *g4;
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  CPPUNIT_ASSERT_EQUAL(graph, graph->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g1->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g2->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(g2, g3->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(g2, g4->getSuperGraph());

  CPPUNIT_ASSERT_EQUAL(graph, graph->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g1->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g2->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g3->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g4->getRoot());

  graph->delSubGraph(g2);
  CPPUNIT_ASSERT_EQUAL(graph, graph->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g1->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g3->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g4->getSuperGraph());

  graph->delSubGraph(g3);
  graph->delSubGraph(g4);
  g2 = g1->addSubGraph();
  g3 = g2->addSubGraph();
  g3->addSubGraph();
  // try to delete its father
  // fail because of an assertion in GraphAbstract::delSubGraph
  // g3->delSubGraph(g2);
  g1->delAllSubGraphs(g2);
  Iterator<Graph *> *itGr = g1->getSubGraphs();
  CPPUNIT_ASSERT(!itGr->hasNext());
  delete itGr;
}
//==========================================================
void SuperGraphTest::testSubgraphId() {
  BooleanProperty sel(graph);
  Graph *g1 = graph->addSubGraph(&sel);
  uint id = g1->getId();

  for (uint i = 1; i < 1000; ++i) {
    g1 = graph->addSubGraph(&sel);
    CPPUNIT_ASSERT_EQUAL(id + i, g1->getId());
    CPPUNIT_ASSERT_EQUAL(g1, graph->getSubGraph(id + i));
  }

  uint i = 0; // if the graph are not ordered that test can fail.
  for (Graph *g : graph->getSubGraphs()) {
    CPPUNIT_ASSERT_EQUAL(id + i, g->getId());
    CPPUNIT_ASSERT_EQUAL(g, graph->getSubGraph(id + i));
    ++i;
  }
}
//==========================================================
void SuperGraphTest::testSubgraph() {
  Graph *g1, *g2, *g3, *g4;
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  CPPUNIT_ASSERT_EQUAL(graph, graph->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g1->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(graph, g2->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(g2, g3->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(g2, g4->getSuperGraph());
  CPPUNIT_ASSERT_EQUAL(g1, graph->getSubGraph(g1->getId()));
  CPPUNIT_ASSERT_EQUAL(g2, graph->getSubGraph(g2->getId()));
  CPPUNIT_ASSERT(graph->getSubGraph(g3->getId()) == nullptr);
  CPPUNIT_ASSERT(graph->getSubGraph(g4->getId()) == nullptr);
  CPPUNIT_ASSERT_EQUAL(g3, g2->getSubGraph(g3->getId()));
  CPPUNIT_ASSERT_EQUAL(g4, g2->getSubGraph(g4->getId()));
  CPPUNIT_ASSERT(graph->isSubGraph(g1));
  CPPUNIT_ASSERT(graph->isSubGraph(g2));
  CPPUNIT_ASSERT(!graph->isSubGraph(g3));
  CPPUNIT_ASSERT(!graph->isSubGraph(g4));
  CPPUNIT_ASSERT(g2->isSubGraph(g3));
  CPPUNIT_ASSERT(g2->isSubGraph(g4));
  CPPUNIT_ASSERT_EQUAL(g1, graph->getDescendantGraph(g1->getId()));
  CPPUNIT_ASSERT_EQUAL(g2, graph->getDescendantGraph(g2->getId()));
  CPPUNIT_ASSERT_EQUAL(g3, graph->getDescendantGraph(g3->getId()));
  CPPUNIT_ASSERT_EQUAL(g4, graph->getDescendantGraph(g4->getId()));
  CPPUNIT_ASSERT_EQUAL(g3, g2->getDescendantGraph(g3->getId()));
  CPPUNIT_ASSERT_EQUAL(g4, g2->getDescendantGraph(g4->getId()));
  CPPUNIT_ASSERT(graph->isDescendantGraph(g1));
  CPPUNIT_ASSERT(graph->isDescendantGraph(g2));
  CPPUNIT_ASSERT(graph->isDescendantGraph(g3));
  CPPUNIT_ASSERT(graph->isDescendantGraph(g4));
  CPPUNIT_ASSERT(g2->isDescendantGraph(g3));
  CPPUNIT_ASSERT(g2->isDescendantGraph(g4));

  CPPUNIT_ASSERT_EQUAL(graph, graph->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g1->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g2->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g3->getRoot());
  CPPUNIT_ASSERT_EQUAL(graph, g4->getRoot());

  // graph's descendant graphs loop
  CPPUNIT_ASSERT(iteratorCountCheck(graph->getDescendantGraphs(), 1));
  set<Graph *> dg;
  dg.insert(g1);
  dg.insert(g2);
  dg.insert(g3);
  dg.insert(g4);
  for (Graph *g : graph->getDescendantGraphs()) {
    auto itdg = dg.find(g);
    CPPUNIT_ASSERT(itdg != dg.end());
    dg.erase(itdg);
  }
  CPPUNIT_ASSERT(dg.empty());

  Iterator<Graph *> *it = g2->getSubGraphs();
  Graph *a, *b;
  CPPUNIT_ASSERT(it->hasNext());
  a = it->next();
  CPPUNIT_ASSERT(it->hasNext());
  b = it->next();
  CPPUNIT_ASSERT((a == g3 && b == g4) || (a == g4 && b == g3));
  CPPUNIT_ASSERT(!it->hasNext());
  delete it;
  g2->clear();
  it = g2->getSubGraphs();
  CPPUNIT_ASSERT(!it->hasNext());
  delete it;

  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();
  node n1 = g3->addNode();
  node n2 = g3->addNode();
  CPPUNIT_ASSERT(g3->isElement(n1) && g3->isElement(n2));
  CPPUNIT_ASSERT(g2->isElement(n1) && graph->isElement(n1));
  CPPUNIT_ASSERT(g2->isElement(n2) && graph->isElement(n2));
  CPPUNIT_ASSERT(!g4->isElement(n1) && !g1->isElement(n1));
  CPPUNIT_ASSERT(!g4->isElement(n2) && !g1->isElement(n2));

  edge e = g2->addEdge(n1, n2);
  CPPUNIT_ASSERT(!g3->isElement(e) && !g4->isElement(e) && !g1->isElement(e));
  CPPUNIT_ASSERT(g2->isElement(e) && graph->isElement(e) && g2->isElement(n2));
  g2->delNode(n2);
  CPPUNIT_ASSERT(!g2->isElement(n2) && !g2->isElement(e));
  CPPUNIT_ASSERT(graph->isElement(n2) && graph->isElement(e));
  g2->addNode(n2);
  g2->addEdge(e);
  CPPUNIT_ASSERT(g2->isElement(n2) && g2->isElement(e));
  g2->delNode(n2, true);
  CPPUNIT_ASSERT(!g2->isElement(n2) && !g2->isElement(e));
  CPPUNIT_ASSERT(!graph->isElement(n2) && !graph->isElement(e));
  graph->clear();

  graph = graph->addSubGraph();
  testAddDel();
  graph->clear();
  testClear();
  graph->clear();
  testOrderEdgeAndSwap();
  graph->clear();
  testIterators();
  graph = graph->getSuperGraph();
}
//==========================================================
void SuperGraphTest::testInheritance() {
  Graph *g1, *g2, *g3, *g4;

  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();
  graph->getDoubleProperty("metric");
  CPPUNIT_ASSERT(graph->existProperty("metric"));
  CPPUNIT_ASSERT(g1->existProperty("metric"));
  CPPUNIT_ASSERT(g2->existProperty("metric"));
  CPPUNIT_ASSERT(g3->existProperty("metric"));
  CPPUNIT_ASSERT(g4->existProperty("metric"));

  graph->delLocalProperty("metric");
  CPPUNIT_ASSERT(!graph->existProperty("metric"));
  CPPUNIT_ASSERT(!g1->existProperty("metric"));
  CPPUNIT_ASSERT(!g2->existProperty("metric"));
  CPPUNIT_ASSERT(!g3->existProperty("metric"));
  CPPUNIT_ASSERT(!g4->existProperty("metric"));

  DoubleProperty *m2 = g2->getLocalDoubleProperty("metric");
  CPPUNIT_ASSERT(!graph->existProperty("metric"));
  CPPUNIT_ASSERT(!g1->existProperty("metric"));
  CPPUNIT_ASSERT(g2->existProperty("metric"));
  CPPUNIT_ASSERT(g3->existProperty("metric"));
  CPPUNIT_ASSERT(g4->existProperty("metric"));

  DoubleProperty *m = graph->getDoubleProperty("metric");
  CPPUNIT_ASSERT_EQUAL(m, graph->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m, g1->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m2, g2->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m2, g3->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m2, g4->getDoubleProperty("metric"));

  m2->rename("metric1");
  CPPUNIT_ASSERT_EQUAL(m, graph->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m, g1->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m2, g2->getDoubleProperty("metric1"));
  CPPUNIT_ASSERT_EQUAL(m, g3->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m, g4->getDoubleProperty("metric"));
  CPPUNIT_ASSERT_EQUAL(m2, g3->getDoubleProperty("metric1"));
  CPPUNIT_ASSERT_EQUAL(m2, g4->getDoubleProperty("metric1"));

  g2->delLocalProperty("metric1");
  CPPUNIT_ASSERT_EQUAL(g2->existLocalProperty("metric1"), false);
  CPPUNIT_ASSERT_EQUAL(g3->existLocalProperty("metric1"), false);
  CPPUNIT_ASSERT_EQUAL(g4->existLocalProperty("metric1"), false);
}
//==========================================================
void SuperGraphTest::testPropertiesIteration() {
  Graph *g1, *g2, *g3, *g4;
  set<string> propList1;
  set<string> propList2;
  propList1.insert("m1");
  propList1.insert("m2");
  propList1.insert("m3");
  propList2.insert("m4");
  propList2.insert("m5");
  propList2.insert("m6");
  g1 = graph->addSubGraph();
  g2 = graph->addSubGraph();
  g3 = g2->addSubGraph();
  g4 = g2->addSubGraph();

  for (const string &str : propList1) {
    graph->getIntegerProperty(str);
    CPPUNIT_ASSERT(g4->existProperty(str));
  }

  for (const string &str : graph->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end());
    }
  }

  for (const string &str : g2->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end());
    }
  }

  for (const string &str : g1->getLocalProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end());
    }
  }

  for (const string &str : g4->getInheritedProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end());
    }
  }

  for (const string &str : propList2) {
    g2->getIntegerProperty(str);
    CPPUNIT_ASSERT(g4->existProperty(str));
  }

  for (const string &str : graph->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end() &&
                     propList2.find(str) == propList2.end());
    }
  }

  for (const string &str : g1->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end() &&
                     propList2.find(str) == propList2.end());
    }
  }

  for (const string &str : g2->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end() ||
                     propList2.find(str) != propList2.end());
    }
  }

  for (const string &str : g3->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end() ||
                     propList2.find(str) != propList2.end());
    }
  }

  for (const string &str : g4->getProperties()) {
    if (str.size() == 2) {
      CPPUNIT_ASSERT(propList1.find(str) != propList1.end() ||
                     propList2.find(str) != propList2.end());
    }
  }
}

//==========================================================
void SuperGraphTest::testAttributes() {
  float f = 12.12f;
  CPPUNIT_ASSERT(!graph->getAttribute("f", f));
  graph->setAttribute("name", string("The Graph"));
  string name;
  graph->getAttribute<string>("name", name);
  CPPUNIT_ASSERT_EQUAL(string("The Graph"), name);
  CPPUNIT_ASSERT(graph->getAttribute("name", name));
  CPPUNIT_ASSERT_EQUAL(string("The Graph"), name);
  DataType *dt = graph->getAttribute("name");
  CPPUNIT_ASSERT(dt != nullptr);
  delete dt;
  TypedData<string> dtc(new string("test"));
  graph->setAttribute("name", static_cast<const DataType *>(&dtc));
  CPPUNIT_ASSERT(graph->existAttribute("name"));
  dt = graph->getAttribute("name");
  CPPUNIT_ASSERT(dt->getTypeName() == typeid(string).name());
  delete dt;
}
//==========================================================
void SuperGraphTest::testGetNodesEqualTo() {
  for (int i = 0; i != 10; ++i) {
    graph->addNode();
  }

  BooleanProperty property(graph);
  property.setAllNodeValue(false);
  Graph *subGraph = graph->addSubGraph();

  for (auto n : property.getNodesEqualTo(false, subGraph)) {
    CPPUNIT_ASSERT(subGraph->isElement(n));
  }
}
