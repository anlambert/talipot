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

#ifndef DOUBLE_PROPERTY_TEST_H
#define DOUBLE_PROPERTY_TEST_H

#include <talipot/Graph.h>
#include <talipot/DoubleProperty.h>

#include "CppUnitIncludes.h"

class DoublePropertyTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(DoublePropertyTest);
    CPPUNIT_TEST(testAnonymousDoublePropertyMaxUpdate);
    CPPUNIT_TEST(testDoublePropertyMinUpdate);
    CPPUNIT_TEST(testDoublePropertyMaxUpdate);
    CPPUNIT_TEST(testDoublePropertyMinUpdateFromString);
    CPPUNIT_TEST(testDoublePropertyMaxUpdateFromString);
    CPPUNIT_TEST(testDoublePropertySubGraphMin);
    CPPUNIT_TEST(testDoublePropertyInfValue);
    CPPUNIT_TEST(testDoublePropertySetAllValue);
    CPPUNIT_TEST(testDoublePropertySetDefaultValue);
    CPPUNIT_TEST(testVectorDoublePropertyCopyFrom);
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() override;
    void tearDown() override;
    void testAnonymousDoublePropertyMaxUpdate();
    void testDoublePropertyMinUpdate();
    void testDoublePropertyMaxUpdate();
    void testDoublePropertyMinUpdateFromString();
    void testDoublePropertyMaxUpdateFromString();
    void testDoublePropertySubGraphMin();
    void testDoublePropertySubGraphMax();
    void testDoublePropertyInfValue();
    void testDoublePropertySetAllValue();
    void testDoublePropertySetDefaultValue();
    void testVectorDoublePropertyCopyFrom();

  private:
    tlp::Graph *graph;
    tlp::node n1, n2, n3, n4;
    tlp::edge e1, e2;
};

#endif // DOUBLE_PROPERTY_TEST_H
