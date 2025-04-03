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

#ifndef DATA_SET_TEST_H
#define DATA_SET_TEST_H

#include "CppUnitIncludes.h"

namespace tlp {

class DataSetTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(DataSetTest);
    CPPUNIT_TEST(testSetGetBool);
    CPPUNIT_TEST(testSetGetDouble);
    CPPUNIT_TEST(testSetGetInt);
    CPPUNIT_TEST(testSetGetString);
    CPPUNIT_TEST(testSetGetStruct);
    CPPUNIT_TEST(testDataSetSerialization);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override {}
    void tearDown() override {}
    void testSetGetBool();
    void testSetGetDouble();
    void testSetGetInt();
    void testSetGetString();
    void testSetGetStruct();
    void testDataSetSerialization();
};
}
#endif // DATA_SET_TEST_H
