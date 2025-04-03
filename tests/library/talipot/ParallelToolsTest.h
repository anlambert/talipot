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

#ifndef PARALLEL_TOOLS_TEST_H
#define PARALLEL_TOOLS_TEST_H

#include <string>

#include "CppUnitIncludes.h"

#include <talipot/Graph.h>

class ParallelToolsTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ParallelToolsTest);
    CPPUNIT_TEST(testParallelMapIndices);
    CPPUNIT_TEST(testParallelMapNodes);
    CPPUNIT_TEST(testParallelMapEdges);
    CPPUNIT_TEST(testParallelMapNodesAndIndices);
    CPPUNIT_TEST(testParallelMapEdgesAndIndices);
    CPPUNIT_TEST(testCriticalSection);
    CPPUNIT_TEST(testNumberOfThreads);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *_graph;

  public:
    void setUp() override;
    void tearDown() override;
    void testParallelMapIndices();
    void testParallelMapNodes();
    void testParallelMapEdges();
    void testParallelMapNodesAndIndices();
    void testParallelMapEdgesAndIndices();
    void testCriticalSection();
    void testNumberOfThreads();
};

#endif // PARALLEL_TOOLS_TEST_H
