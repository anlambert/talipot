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

#ifndef STRING_PROPERTY_TEST_H
#define STRING_PROPERTY_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
}

class StringPropertyTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StringPropertyTest);
    CPPUNIT_TEST(simpleVectorTest);
    CPPUNIT_TEST(complexVectorTest);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;

  public:
    void setUp() override;
    void tearDown() override;

    void simpleVectorTest();
    void complexVectorTest();
};

#endif // STRING_PROPERTY_TEST_H
