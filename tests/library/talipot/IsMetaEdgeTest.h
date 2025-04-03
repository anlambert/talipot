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

#include <string>
#include <talipot/Graph.h>
#include <talipot/Matrix.h>

#include "CppUnitIncludes.h"

class IsMetaEdgeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IsMetaEdgeTest);
    CPPUNIT_TEST(testIsMetaEdge);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testIsMetaEdge();

  private:
    tlp::Graph *graph;
    tlp::Graph *quotientGraph;
};
