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

#ifndef TEST_ALGORITHM_TEST_H
#define TEST_ALGORITHM_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
}

class TestAlgorithmTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestAlgorithmTest);
  CPPUNIT_TEST(testSimple);
  CPPUNIT_TEST(testFreeTree);
  CPPUNIT_TEST(testTree);
  CPPUNIT_TEST(testAcyclic);
  CPPUNIT_TEST(testConnected);
  CPPUNIT_TEST(testBiconnected);
  CPPUNIT_TEST(testBridges);
  CPPUNIT_TEST_SUITE_END();

private:
  tlp::Graph *graph;

public:
  void setUp() override;
  void tearDown() override;
  void testSimple();
  void testFreeTree();
  void testTree();
  void testAcyclic();
  void testConnected();
  void testBiconnected();
  void testBridges();
};

#endif // TEST_ALGORITHM_TEST_H
