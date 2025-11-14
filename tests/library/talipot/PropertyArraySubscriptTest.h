/**
 *
 * Copyright (C) 2025-2026  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef PROPERTYARRAYSUBSCRIPTTEST_H_
#define PROPERTYARRAYSUBSCRIPTTEST_H_

#include <talipot/Graph.h>

#include "CppUnitIncludes.h"

class PropertyArraySubscriptTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PropertyArraySubscriptTest);
  CPPUNIT_TEST(testBooleanProperty);
  CPPUNIT_TEST(testIntegerProperty);
  CPPUNIT_TEST(testDoubleProperty);
  CPPUNIT_TEST(testLayoutProperty);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override;
  void tearDown() override;
  void testBooleanProperty();
  void testDoubleProperty();
  void testIntegerProperty();
  void testLayoutProperty();

private:
  tlp::Graph *graph;
  tlp::node n1, n2, n3;
  tlp::edge e1, e2, e3;
};

#endif