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

#ifndef BASIC_METRIC_TEST_H
#define BASIC_METRIC_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
}

class BasicMetricTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicMetricTest);
    CPPUNIT_TEST(testArityMetric);
    CPPUNIT_TEST(testBetweennessCentrality);
    CPPUNIT_TEST(testBiconnectedComponent);
    CPPUNIT_TEST(testClusterMetric);
    CPPUNIT_TEST(testConnectedComponent);
    CPPUNIT_TEST(testDagLevelMetric);
    CPPUNIT_TEST(testDepthMetric);
    CPPUNIT_TEST(testEccentricity);
    CPPUNIT_TEST(testIdMetric);
    CPPUNIT_TEST(testLeafMetric);
    CPPUNIT_TEST(testNodeMetric);
    CPPUNIT_TEST(testPathLengthMetric);
    CPPUNIT_TEST(testRandomMetric);
    CPPUNIT_TEST(testStrahlerMetric);
    CPPUNIT_TEST(testStrengthMetric);
    CPPUNIT_TEST(testStrongComponent);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    template <typename PropType>
    bool computeProperty(const std::string &algorithm,
                         const std::string &graphType = "Planar Graph", PropType *prop = nullptr);

  public:
    void setUp() override;
    void tearDown() override;

    void testArityMetric();
    void testBetweennessCentrality();
    void testBiconnectedComponent();
    void testClusterMetric();
    void testConnectedComponent();
    void testDagLevelMetric();
    void testDepthMetric();
    void testEccentricity();
    void testIdMetric();
    void testLeafMetric();
    void testNodeMetric();
    void testPathLengthMetric();
    void testRandomMetric();
    void testStrahlerMetric();
    void testStrengthMetric();
    void testStrongComponent();
};

#endif // BASIC_METRIC_TEST_H
