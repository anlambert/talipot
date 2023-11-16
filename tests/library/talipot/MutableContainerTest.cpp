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

#include "MutableContainerTest.h"

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(MutableContainerTest);

//==========================================================
void MutableContainerTest::setUp() {
#ifndef NDEBUG
  // choose the seed to ease the debugging
  tlp::setSeedOfRandomSequence(1);
  tlp::initRandomSequence();
#endif
  mutBool = new MutableContainer<bool>();
  mutDouble = new MutableContainer<double>();
  mutString = new MutableContainer<string>();
}
//==========================================================
void MutableContainerTest::tearDown() {
  delete mutBool;
  delete mutDouble;
  delete mutString;
}
//==========================================================
void MutableContainerTest::testCompression() {
  mutDouble->setAll(10.0);
  CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
  CPPUNIT_ASSERT_EQUAL(unsigned(0), mutDouble->elementInserted);

  for (uint i = 0; i < 1000; ++i) {
    mutDouble->set(100 + i, 13.0);
    CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
    CPPUNIT_ASSERT_EQUAL(i + 1, mutDouble->elementInserted);
  }

  mutDouble->setAll(10.0);
  CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
  CPPUNIT_ASSERT_EQUAL(unsigned(0), mutDouble->elementInserted);

  for (uint i = 0; i < 1000; ++i) {
    mutDouble->set(10000 + i * 2, 13.0);
    CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
    CPPUNIT_ASSERT_EQUAL(i + 1, mutDouble->elementInserted);
  }

  for (uint i = 0; i < 1000; ++i) {
    mutDouble->set(10000 + i * 2, 13.0);
    mutDouble->add(10000 + i * 2, i);
    CPPUNIT_ASSERT_EQUAL(mutDouble->get(10000 + i * 2), 13. + i);
    CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
    CPPUNIT_ASSERT_EQUAL(unsigned(1000), mutDouble->elementInserted);
  }

  mutDouble->setAll(10.0);
  CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
  CPPUNIT_ASSERT_EQUAL(unsigned(0), mutDouble->elementInserted);
  mutDouble->set(1000, 345);
  mutDouble->set(10000, 345);
  CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::HASH, mutDouble->state);
  CPPUNIT_ASSERT_EQUAL(unsigned(2), mutDouble->elementInserted);
  mutDouble->add(1000, 1000.5);
  CPPUNIT_ASSERT_EQUAL(mutDouble->get(1000), 1345.5);
  mutDouble->add(10000, -1000.2);
  CPPUNIT_ASSERT_EQUAL(mutDouble->get(10000), 345. - 1000.2);
  mutDouble->add(100, 1000.45);
  CPPUNIT_ASSERT_EQUAL(mutDouble->get(100), 1010.45);

  for (uint i = 1001; i < 10000; ++i) {
    mutDouble->set(i, 345);
    CPPUNIT_ASSERT_EQUAL(unsigned(i + 3 - 1000), mutDouble->elementInserted);
  }

  CPPUNIT_ASSERT_EQUAL(MutableContainer<double>::VECT, mutDouble->state);
  mutDouble->setAll(-1);
  std::ifstream in("sillyMutableData.txt");
  uint id;

  while (in >> id) {
    double value;
    in >> value;
    mutDouble->set(id, value);
  }
}
//==========================================================
void MutableContainerTest::testFindAll() {
  mutBool->setAll(false);
  mutDouble->setAll(10.0);
  mutString->setAll("David");

  mutBool->set(10, true);
  mutBool->set(15, true);
  mutBool->set(17, true);

  mutDouble->set(10, 13.0);
  mutDouble->set(15, 13.0);
  mutDouble->set(17, 13.0);

  mutString->set(10, "Sophie");
  mutString->set(15, "Sophie");
  mutString->set(17, "Sophie");

  Iterator<uint> *itB;
  Iterator<uint> *itD;
  Iterator<uint> *itS;

  itB = mutBool->findAll(true);
  itD = mutDouble->findAll(13.0);
  itS = mutString->findAll("Sophie");
  CPPUNIT_ASSERT(itB->hasNext());
  CPPUNIT_ASSERT_EQUAL(10u, itB->next());
  CPPUNIT_ASSERT(itD->hasNext());
  CPPUNIT_ASSERT_EQUAL(10u, itD->next());
  CPPUNIT_ASSERT(itS->hasNext());
  CPPUNIT_ASSERT_EQUAL(10u, itS->next());

  CPPUNIT_ASSERT(itB->hasNext());
  CPPUNIT_ASSERT_EQUAL(15u, itB->next());
  CPPUNIT_ASSERT(itD->hasNext());
  CPPUNIT_ASSERT_EQUAL(15u, itD->next());
  CPPUNIT_ASSERT(itS->hasNext());
  CPPUNIT_ASSERT_EQUAL(15u, itS->next());

  CPPUNIT_ASSERT(itB->hasNext());
  CPPUNIT_ASSERT_EQUAL(17u, itB->next());
  CPPUNIT_ASSERT(itD->hasNext());
  CPPUNIT_ASSERT_EQUAL(17u, itD->next());
  CPPUNIT_ASSERT(itS->hasNext());
  CPPUNIT_ASSERT_EQUAL(17u, itS->next());

  delete itB;
  delete itD;
  delete itS;

  Iterator<uint> *nullIterator = nullptr;

  itB = mutBool->findAll(false);
  CPPUNIT_ASSERT_EQUAL(nullIterator, itB);

  itD = mutDouble->findAll(10.0);
  CPPUNIT_ASSERT_EQUAL(nullIterator, itD);

  itS = mutString->findAll("David");
  CPPUNIT_ASSERT_EQUAL(nullIterator, itS);
}
//==========================================================
void MutableContainerTest::testSetAll() {
  mutBool->setAll(true);
  mutDouble->setAll(10.0);
  mutString->setAll("David");

  for (uint i = 0; i < NBTEST; ++i) {
    CPPUNIT_ASSERT(mutBool->get(i));
    CPPUNIT_ASSERT_EQUAL(10.0, mutDouble->get(i));
    CPPUNIT_ASSERT_EQUAL(string("David"), mutString->get(i));
  }

  mutBool->setAll(false);
  mutDouble->setAll(-33.0);
  mutString->setAll("Sophie");

  for (uint i = 0; i < NBTEST; ++i) {
    CPPUNIT_ASSERT(!mutBool->get(i));
    CPPUNIT_ASSERT_EQUAL(-33.0, mutDouble->get(i));
    CPPUNIT_ASSERT_EQUAL(string("Sophie"), mutString->get(i));
  }

  // New test bug at index 0
  MutableContainer<int> vect;
  vect.setAll(-1);
  CPPUNIT_ASSERT_EQUAL(-1, vect.get(1));
  vect.set(1, 1);
  CPPUNIT_ASSERT_EQUAL(1, vect.get(1));
  CPPUNIT_ASSERT_EQUAL(-1, vect.get(2));
  CPPUNIT_ASSERT_EQUAL(-1, vect.get(0));

  MutableContainer<int> vect2;
  vect2.setAll(0);
  vect2.set(0, 0);
  CPPUNIT_ASSERT_EQUAL(0, vect2.get(0));
  vect2.set(1, 0);
  CPPUNIT_ASSERT_EQUAL(0, vect2.get(1));
}
//==========================================================
void MutableContainerTest::testSetGet() {
  mutBool->setAll(false);
  mutDouble->setAll(-33.0);
  mutDouble->set(100, 10);
  mutDouble->set(1000000, 20);
  CPPUNIT_ASSERT_EQUAL(10.0, mutDouble->get(100));
  CPPUNIT_ASSERT_EQUAL(20.0, mutDouble->get(1000000));
  mutDouble->set(1000000, -33.0);

  for (uint i = 101; i < 1000; ++i) {
    mutDouble->set(i, i);
    CPPUNIT_ASSERT_EQUAL(double(i), mutDouble->get(i));
  }

  for (uint i = 101; i < 999; ++i) {
    mutDouble->set(i, -33.0);
  }

  CPPUNIT_ASSERT_EQUAL(10.0, mutDouble->get(100));

  mutString->setAll("Sophie");

  for (uint i = 0; i < NBTEST * 10; ++i) {
    uint rando = randomNumber(NBTEST - 1);
    mutBool->set(rando, true);
    mutDouble->set(rando, rando);
    mutString->set(rando, string("David"));
    CPPUNIT_ASSERT(mutBool->get(rando));
    CPPUNIT_ASSERT_EQUAL(double(rando), mutDouble->get(rando));
    CPPUNIT_ASSERT_EQUAL(string("David"), mutString->get(rando));
  }

  mutBool->setAll(true);

  for (uint i = 0; i < NBTEST * 10; ++i) {
    uint rando = randomNumber(NBTEST - 1);
    mutBool->set(rando, false);
    CPPUNIT_ASSERT(!mutBool->get(rando));
  }

  for (uint i = 0; i < NBTEST * 10; ++i) {
    uint rando = randomNumber(NBTEST - 1);
    bool isNotDefault = true;
    mutBool->set(rando, true);
    CPPUNIT_ASSERT(mutBool->get(rando, isNotDefault));
    CPPUNIT_ASSERT(!isNotDefault);
    mutString->set(rando, string("Sophie"));
    CPPUNIT_ASSERT_EQUAL(string("Sophie"), mutString->get(rando, isNotDefault));
    CPPUNIT_ASSERT(!isNotDefault);
  }
}
