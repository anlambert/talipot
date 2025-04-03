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

#ifndef INTEGER_PROPERTY_MIN_MAX_UPTODATE_TEST_H
#define INTEGER_PROPERTY_MIN_MAX_UPTODATE_TEST_H

#include <talipot/Graph.h>
#include <talipot/IntegerProperty.h>

#include "CppUnitIncludes.h"

class IntegerPropertyMinMaxUpdateTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IntegerPropertyMinMaxUpdateTest);
    CPPUNIT_TEST(testIntegerPropertyMinUpdate);
    CPPUNIT_TEST(testIntegerPropertyMaxUpdate);
    CPPUNIT_TEST(testIntegerPropertyMinUpdateFromString);
    CPPUNIT_TEST(testIntegerPropertyMaxUpdateFromString);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testIntegerPropertyMinUpdate();
    void testIntegerPropertyMaxUpdate();
    void testIntegerPropertyMinUpdateFromString();
    void testIntegerPropertyMaxUpdateFromString();

  private:
    tlp::Graph *graph;
    tlp::node n1, n4;
};

#endif // INTEGER_PROPERTY_MIN_MAX_UPTODATE_TEST_H
