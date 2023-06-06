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

#ifndef SUPER_GRAPH_TEST_H
#define SUPER_GRAPH_TEST_H

#include <talipot/Graph.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

class SuperGraphTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SuperGraphTest);
  CPPUNIT_TEST(testAddDel);
  CPPUNIT_TEST(testClear);
  CPPUNIT_TEST(testOrderEdgeAndSwap);
  CPPUNIT_TEST(testSubgraph);
  CPPUNIT_TEST(testSubgraphId);
  CPPUNIT_TEST(testDeleteSubgraph);
  CPPUNIT_TEST(testInheritance);
  CPPUNIT_TEST(testIterators);
  CPPUNIT_TEST(testPropertiesIteration);
  CPPUNIT_TEST(testDegree);
  CPPUNIT_TEST(testAttributes);
  CPPUNIT_TEST(testGetNodesEqualTo);
  CPPUNIT_TEST(testInducedSubGraph);
  CPPUNIT_TEST_SUITE_END();

private:
  tlp::Graph *graph;

public:
  void setUp() override;
  void tearDown() override;

  void testAddDel();
  void testClear();
  void testOrderEdgeAndSwap();
  void testSubgraph();
  void testSubgraphId();
  void testDeleteSubgraph();
  void testInheritance();
  void testIterators();
  void testPropertiesIteration();
  void testDegree();
  void testAttributes();
  void testGetNodesEqualTo();
  void testInducedSubGraph();

private:
  void build(uint, uint);
};

#endif // SUPER_GRAPH_TEST_H
