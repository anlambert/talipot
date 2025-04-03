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

#include "CppUnitIncludes.h"

class PropertiesMinMaxAfterAddNodeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PropertiesMinMaxAfterAddNodeTest);
    CPPUNIT_TEST(testDoublePropertyMinMaxAfterAddNode);
    CPPUNIT_TEST(testIntegerPropertyMinMaxAfterAddNode);
    CPPUNIT_TEST(testLayoutPropertyMinMaxAfterAddNode);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testDoublePropertyMinMaxAfterAddNode();
    void testIntegerPropertyMinMaxAfterAddNode();
    void testLayoutPropertyMinMaxAfterAddNode();

  private:
    tlp::Graph *graph;
};
