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

#ifndef MUTABLE_CONTAINER_TEST_H
#define MUTABLE_CONTAINER_TEST_H

#include <string>
#include <talipot/MutableContainer.h>
// needed by MutableContainer<std::string>
#include <talipot/PropertyTypes.h>

#include "CppUnitIncludes.h"

namespace tlp {
class MutableContainerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MutableContainerTest);
    CPPUNIT_TEST(testSetAll);
    CPPUNIT_TEST(testSetGet);
    CPPUNIT_TEST(testFindAll);
    CPPUNIT_TEST(testCompression);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::MutableContainer<bool> *mutBool;
    tlp::MutableContainer<double> *mutDouble;
    tlp::MutableContainer<std::string> *mutString;
    static const uint NBTEST = 1000;

  public:
    void setUp() override;
    void tearDown() override;
    void testSetAll();
    void testSetGet();
    void testFindAll();
    void testCompression();
};
}
#endif // MUTABLE_CONTAINER_TEST_H
