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

#ifndef EXTENDED_CLUSTER_OPERATION_TEST_H
#define EXTENDED_CLUSTER_OPERATION_TEST_H

#include <string>
#include <talipot/Graph.h>

#include "CppUnitIncludes.h"

class ExtendedClusterOperationTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ExtendedClusterOperationTest);
    CPPUNIT_TEST(testCreateMetaNode);
    CPPUNIT_TEST(testBugCreateOpenMetaNode);
    CPPUNIT_TEST(testBugOpenInSubgraph);
    CPPUNIT_TEST(testOpenMetaNode);
    CPPUNIT_TEST(testOpenMetaNodes);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph, *quotient;
    tlp::node meta;
    std::vector<tlp::node> nodes;
    std::vector<tlp::edge> edges;
    std::vector<tlp::node> group;

  public:
    void setUp() override;
    void tearDown() override;
    void testCreateMetaNode();
    void testBugCreateOpenMetaNode();
    void testBugOpenInSubgraph();
    void testOpenMetaNode();
    void testOpenMetaNodes();
};

#endif // EXTENDED_CLUSTER_OPERATION_TEST_H
