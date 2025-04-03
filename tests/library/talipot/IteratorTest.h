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

#ifndef ITERATOR_TEST_H
#define ITERATOR_TEST_H

#include <string>

#include "CppUnitIncludes.h"

namespace tlp {
class Graph;
class DoubleProperty;
}

class IteratorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IteratorTest);
    CPPUNIT_TEST(testIterator);
    CPPUNIT_TEST(testStableIterator);
    CPPUNIT_TEST(testSortIterator);
    CPPUNIT_TEST(testFilterIterator);
    CPPUNIT_TEST(testUniqueIterator);
    CPPUNIT_TEST(testConversionIterator);
    CPPUNIT_TEST(testConcatIterator);
    CPPUNIT_TEST(testIteratorCount);
    CPPUNIT_TEST(testIteratorMap);
    CPPUNIT_TEST(testIteratorReduce);
    CPPUNIT_TEST(testIteratorCountCheck);
    CPPUNIT_TEST(testIteratorToStlContainers);
    CPPUNIT_TEST(testStlMapIterator);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    tlp::DoubleProperty *idMetric;

  public:
    void setUp();
    void tearDown();
    void testIterator();
    void testStableIterator();
    void testSortIterator();
    void testFilterIterator();
    void testUniqueIterator();
    void testConversionIterator();
    void testConcatIterator();
    void testIteratorCount();
    void testIteratorMap();
    void testIteratorReduce();
    void testIteratorCountCheck();
    void testIteratorToStlContainers();
    void testStlMapIterator();
};

#endif // ITERATOR_TEST_H
