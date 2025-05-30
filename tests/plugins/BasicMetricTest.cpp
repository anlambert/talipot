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

#include "BasicMetricTest.h"
#include <talipot/DoubleProperty.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(BasicMetricTest);

template <typename PropType>
bool BasicMetricTest::computeProperty(const std::string &algorithm, const std::string &graphType,
                                      PropType *prop) {

  DataSet ds;
  tlp::Graph *g = tlp::importGraph(graphType, ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);

  bool deleteProp = prop == nullptr;

  if (prop == nullptr) {
    prop = new PropType(graph);
  }

  std::string errorMsg;
  bool result = graph->applyPropertyAlgorithm(algorithm, prop, errorMsg);

  if (deleteProp) {
    delete prop;
  }

  return result;
}

void BasicMetricTest::setUp() {
  graph = tlp::newGraph();
}

void BasicMetricTest::tearDown() {
  delete graph;
}

void BasicMetricTest::testArityMetric() {
  bool result = computeProperty<DoubleProperty>("Degree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testBetweennessCentrality() {
  bool result = computeProperty<DoubleProperty>("Betweenness Centrality");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testBiconnectedComponent() {
  bool result = computeProperty<DoubleProperty>("Biconnected Components");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testClusterMetric() {
  bool result = computeProperty<DoubleProperty>("Cluster");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testConnectedComponent() {
  bool result = computeProperty<DoubleProperty>("Connected Components");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testDagLevelMetric() {
  bool result = computeProperty<DoubleProperty>("Dag Level");
  CPPUNIT_ASSERT(result == false);
  graph->clear();
  result = computeProperty<DoubleProperty>("Dag Level", "Random General Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testDepthMetric() {
  bool result = computeProperty<DoubleProperty>("Depth");
  CPPUNIT_ASSERT(result == false);
  graph->clear();
  result = computeProperty<DoubleProperty>("Depth", "Random General Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testEccentricity() {
  bool result = computeProperty<DoubleProperty>("Eccentricity");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testIdMetric() {
  bool result = computeProperty<DoubleProperty>("Id");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testLeafMetric() {
  bool result = computeProperty<DoubleProperty>("Leaf");
  CPPUNIT_ASSERT(result == false);
  graph->clear();
  result = computeProperty<DoubleProperty>("Leaf", "Random General Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testNodeMetric() {
  bool result = computeProperty<DoubleProperty>("Node");
  CPPUNIT_ASSERT(result == false);
  graph->clear();
  result = computeProperty<DoubleProperty>("Node", "Random General Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testPathLengthMetric() {
  bool result = computeProperty<DoubleProperty>("Path Length");
  CPPUNIT_ASSERT(result == false);
  graph->clear();
  result = computeProperty<DoubleProperty>("Path Length", "Random General Tree");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testRandomMetric() {
  bool result = computeProperty<DoubleProperty>("Random metric");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testStrahlerMetric() {
  bool result = computeProperty<DoubleProperty>("Strahler");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testStrengthMetric() {
  bool result = computeProperty<DoubleProperty>("Strength");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicMetricTest::testStrongComponent() {
  bool result = computeProperty<DoubleProperty>("Strongly Connected Components");
  CPPUNIT_ASSERT(result);
}
//==========================================================
