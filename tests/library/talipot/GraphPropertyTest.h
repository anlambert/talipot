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

#ifndef GRAPH_PROPERTY_TEST_H
#define GRAPH_PROPERTY_TEST_H

#include <string>
#include <talipot/GraphProperty.h>
#include <talipot/Graph.h>

#include "CppUnitIncludes.h"

class GraphPropertyTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(GraphPropertyTest);
    CPPUNIT_TEST(testDestroyGraph);
    CPPUNIT_TEST(testSetGet);
    CPPUNIT_TEST(testSetAll);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;

  public:
    void setUp() override;
    void tearDown() override;
    void testDestroyGraph();
    void testSetGet();
    void testSetAll();
};

#endif // GRAPH_PROPERTY_TEST_H
