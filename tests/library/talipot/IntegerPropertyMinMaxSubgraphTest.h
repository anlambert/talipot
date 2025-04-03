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

#ifndef INTEGER_PROPERTY_MIN_MAX_SUBGRAPH_TEST_H
#define INTEGER_PROPERTY_MIN_MAX_SUBGRAPH_TEST_H

#include <talipot/Graph.h>
#include <talipot/IntegerProperty.h>

#include "CppUnitIncludes.h"

class IntegerPropertyMinMaxSubgraphTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IntegerPropertyMinMaxSubgraphTest);
    CPPUNIT_TEST(testIntegerPropertyRootGraphMin);
    CPPUNIT_TEST(testIntegerPropertyRootGraphMax);
    CPPUNIT_TEST(testIntegerPropertySubgraphMin);
    CPPUNIT_TEST(testIntegerPropertySubgraphMax);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testIntegerPropertyRootGraphMin();
    void testIntegerPropertyRootGraphMax();
    void testIntegerPropertySubgraphMin();
    void testIntegerPropertySubgraphMax();

  private:
    tlp::Graph *graph, *subGraph;
};

#endif // INTEGER_PROPERTY_MIN_MAX_SUBGRAPH_TEST_H
