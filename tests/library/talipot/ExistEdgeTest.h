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

#ifndef EXIST_EDGE_TEST_H
#define EXIST_EDGE_TEST_H

#include <talipot/Graph.h>

#include "CppUnitIncludes.h"

class ExistEdgeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ExistEdgeTest);
    CPPUNIT_TEST(testExistEdge);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testExistEdge();

  private:
    tlp::Graph *graph;
    tlp::node n0, n1, n2;
    tlp::edge e0, e1, e2, e3;
};

#endif // EXIST_EDGE_TEST_H
