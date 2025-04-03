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

#ifndef BASIC_LAYOUT_TEST_H
#define BASIC_LAYOUT_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
}

class BasicLayoutTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicLayoutTest);
    CPPUNIT_TEST(testBubbleTree);
    CPPUNIT_TEST(testCircular);
    CPPUNIT_TEST(testConeTreeExtended);
    CPPUNIT_TEST(testConnectedComponentPacking);
    CPPUNIT_TEST(testDendrogram);
    CPPUNIT_TEST(testGEMLayout);
    CPPUNIT_TEST(testHierarchicalGraph);
    CPPUNIT_TEST(testImprovedWalker);
    CPPUNIT_TEST(testMixedModel);
    CPPUNIT_TEST(testRandomLayout);
    CPPUNIT_TEST(testSquarifiedTreeMap);
    CPPUNIT_TEST(testTreeLeaf);
    CPPUNIT_TEST(testTreeMap);
    CPPUNIT_TEST(testTreeRadial);
    CPPUNIT_TEST(testTreeReingoldAndTilfordExtended);
    CPPUNIT_TEST(testTutte);
    CPPUNIT_TEST(testKruskal);
    CPPUNIT_TEST(testFastOverlapRemoval);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    void initializeGraph(const std::string &type);
    template <typename PropType>
    bool computeProperty(const std::string &algorithm,
                         const std::string &graphType = "Planar Graph", PropType *prop = nullptr);

  public:
    void setUp() override;
    void tearDown() override;

    void testBubbleTree();
    void testCircular();
    void testConeTreeExtended();
    void testConnectedComponentPacking();
    void testDendrogram();
    void testGEMLayout();
    void testHierarchicalGraph();
    void testImprovedWalker();
    void testMixedModel();
    void testRandomLayout();
    void testSquarifiedTreeMap();
    void testTreeLeaf();
    void testTreeMap();
    void testTreeRadial();
    void testTreeReingoldAndTilfordExtended();
    void testTutte();
    void testKruskal();
    void testFastOverlapRemoval();
};

#endif // BASIC_LAYOUT_TEST_H
