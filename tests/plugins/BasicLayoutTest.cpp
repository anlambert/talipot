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

#include "BasicLayoutTest.h"

#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/BooleanProperty.h>
#include <talipot/DoubleProperty.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(BasicLayoutTest);

void BasicLayoutTest::initializeGraph(const string &type) {
  DataSet ds;
  Graph *g = importGraph(type, ds, nullptr, graph);
  CPPUNIT_ASSERT_EQUAL(g, graph);
}
//==========================================================
template <typename PropType>
bool BasicLayoutTest::computeProperty(const std::string &algorithm, const std::string &graphType,
                                      PropType *prop) {
  initializeGraph(graphType);
  bool deleteProp = prop == nullptr;

  if (prop == nullptr) {
    prop = new PropType(graph);
  }

  string errorMsg;
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm(algorithm, prop, errorMsg);

  if (deleteProp) {
    delete prop;
  }

  return result;
}
//==========================================================

void BasicLayoutTest::setUp() {
  graph = tlp::newGraph();
}

void BasicLayoutTest::tearDown() {
  delete graph;
}

void BasicLayoutTest::testBubbleTree() {
  bool result = computeProperty<LayoutProperty>("Bubble Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testCircular() {
  bool result = computeProperty<LayoutProperty>("Circular");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testConeTreeExtended() {
  bool result = computeProperty<LayoutProperty>("Cone Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testConnectedComponentPacking() {
  bool result = computeProperty<LayoutProperty>("Connected Components Packing");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testDendrogram() {
  initializeGraph("Planar Graph");
  DataSet ds;
  SizeProperty size(graph);
  ds.set("node size", &size);
  LayoutProperty layout(graph);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Dendrogram", &layout, errorMsg, &ds);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testGEMLayout() {
  DataSet ds;
  ds.set("file::filename", string("data/unconnected.tlp"));
  Graph *g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  LayoutProperty prop(graph);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("GEM (Frick)", &prop, errorMsg);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testHierarchicalGraph() {
  bool result = computeProperty<LayoutProperty>("Hierarchical Graph");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testImprovedWalker() {
  bool result = computeProperty<LayoutProperty>("Improved Walker");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testMixedModel() {
  initializeGraph("Planar Graph");
  DataSet ds;
  SizeProperty size(graph);
  ds.set("node size", &size);
  LayoutProperty layout(graph);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Mixed Model", &layout, errorMsg, &ds);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testRandomLayout() {
  bool result = computeProperty<LayoutProperty>("Random layout");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testSquarifiedTreeMap() {
  initializeGraph("Random General Tree");
  DoubleProperty metric(graph);
  string errorMsg;
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm("Degree", &metric, errorMsg);
  CPPUNIT_ASSERT(result);

  LayoutProperty layout(graph);
  ds.set("metric", &metric);
  result = graph->applyPropertyAlgorithm("Squarified Tree Map", &layout, errorMsg, &ds);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testTreeLeaf() {
  initializeGraph("Planar Graph");
  DataSet ds;
  SizeProperty size(graph);
  ds.set("node size", &size);
  LayoutProperty layout(graph);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Tree Leaf", &layout, errorMsg, &ds);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testTreeMap() {}
//==========================================================
void BasicLayoutTest::testTreeRadial() {
  bool result = computeProperty<LayoutProperty>("Tree Radial");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testTreeReingoldAndTilfordExtended() {
  bool result = computeProperty<LayoutProperty>("Hierarchical Tree (R-T Extended)");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testTutte() {
  bool result = computeProperty<LayoutProperty>("3-Connected (Tutte)", "Complete General Graph");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicLayoutTest::testKruskal() {
  BooleanProperty selection(graph);
  bool result = computeProperty<BooleanProperty>("Kruskal", "Planar Graph", &selection);
  CPPUNIT_ASSERT(result);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(selection.getNodeValue(n));
  }
}
//==========================================================
void BasicLayoutTest::testFastOverlapRemoval() {
  initializeGraph("Planar Graph");
  DataSet ds;
  LayoutProperty layout(graph);
  ds.set("layout", &layout);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Fast Overlap Removal", &layout, errorMsg, &ds);
  CPPUNIT_ASSERT(result);
}
