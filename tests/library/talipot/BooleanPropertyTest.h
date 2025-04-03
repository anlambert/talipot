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

#ifndef BOOLEAN_PROPERTY_TEST_H
#define BOOLEAN_PROPERTY_TEST_H

#include <string>
#include <talipot/Graph.h>
#include <talipot/BooleanProperty.h>

#include "CppUnitIncludes.h"

namespace tlp {
class BooleanProperty;
}

class BooleanPropertyTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BooleanPropertyTest);
    CPPUNIT_TEST(testSetAll);
    CPPUNIT_TEST(testSetGet);
    CPPUNIT_TEST(testCopy);
    CPPUNIT_TEST(testIterators);
    CPPUNIT_TEST(testDelete);
    CPPUNIT_TEST(testReverse);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    tlp::BooleanProperty *selection;

  public:
    void setUp() override;
    void tearDown() override;
    void testSetAll();
    void testSetAll(bool);
    void testSetGet();
    void testSetGet(bool);
    void testCopy();
    void testIterators();
    void testDelete(bool value);
    void testDelete();
    void testReverse();
};

#endif // BOOLEAN_PROPERTY_TEST_H
