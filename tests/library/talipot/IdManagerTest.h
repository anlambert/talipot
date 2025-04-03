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

#ifndef ID_MANAGER_TEST_H
#define ID_MANAGER_TEST_H

#include <string>
#include <talipot/Graph.h>
#include <talipot/IdManager.h>

#include "CppUnitIncludes.h"

namespace tlp {

class IdManagerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IdManagerTest);
    CPPUNIT_TEST(testIsFree);
    CPPUNIT_TEST(testFragmentation);
    CPPUNIT_TEST(testGetFree);
    CPPUNIT_TEST(testIterate);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override {
        idManager = new IdManager();
    }
    void tearDown() override {
        delete idManager;
    }
    void testIsFree();
    void testFragmentation();
    void testGetFree();
    void testIterate();

  private:
    IdManager *idManager;
};
}
#endif // ID_MANAGER_TEST_H
