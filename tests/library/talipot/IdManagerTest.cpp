/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "IdManagerTest.h"

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(IdManagerTest);

//==========================================================
void IdManagerTest::testFragmentation() {
  for (uint i = 0; i < 1000; ++i) {
    CPPUNIT_ASSERT_EQUAL(i, idManager->get());
  }

  for (uint i = 1; i < 100; ++i) {
    idManager->free(i);
  }

  CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(99), idManager->state.freeIds.size());
  idManager->free(0);
  CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), idManager->state.freeIds.size());

  for (uint i = 900; i < 999; ++i) {
    idManager->free(i);
  }

  CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(99), idManager->state.freeIds.size());
  idManager->free(999);
  CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(100), idManager->state.freeIds.size());
}
//==========================================================
void IdManagerTest::testGetFree() {
  for (uint i = 0; i < 1000; ++i) {
    CPPUNIT_ASSERT_EQUAL(i, idManager->get());
  }

  for (uint i = 0; i < 500; ++i) {
    idManager->free(i * 2);
  }

  for (uint i = 0; i < 500; ++i) {
    CPPUNIT_ASSERT_EQUAL(i * 2, idManager->get());
  }

  for (uint i = 100; i <= 200; ++i) {
    idManager->free(i);
  }

  for (uint i = 100; i <= 200; ++i) {
    CPPUNIT_ASSERT_EQUAL(i, idManager->get());
  }
}
//==========================================================
void IdManagerTest::testIsFree() {
  for (uint i = 0; i < 1000; ++i) {
    idManager->get();
  }

  for (uint i = 0; i < 500; ++i) {
    idManager->free(i * 2);
  }

  for (uint i = 0; i < 500; ++i) {
    CPPUNIT_ASSERT(idManager->is_free(i * 2));
    CPPUNIT_ASSERT(!idManager->is_free(i * 2 + 1));
  }

  CPPUNIT_ASSERT(idManager->is_free(1200));
}
//==========================================================
void IdManagerTest::testIterate() {
  for (uint i = 0; i < 1000; ++i) {
    idManager->get();
  }

  uint id = 0;

  for (uint itId : idManager->getIds()) {
    CPPUNIT_ASSERT_EQUAL(id, itId);
    ++id;
  }

  for (uint i = 0; i < 500; ++i) {
    idManager->free(i * 2);
  }

  id = 0;

  for (uint itId : idManager->getIds()) {
    CPPUNIT_ASSERT_EQUAL(2u * id + 1u, itId);
    ++id;
  }

  for (uint i = 0; i < 500; ++i) {
    CPPUNIT_ASSERT(idManager->is_free(i * 2));
    CPPUNIT_ASSERT(!idManager->is_free(i * 2 + 1));
  }

  CPPUNIT_ASSERT(idManager->is_free(1200));
}
