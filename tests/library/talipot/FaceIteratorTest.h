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

#ifndef FACE_ITERATOR_TEST_H
#define FACE_ITERATOR_TEST_H

#include <talipot/FaceIterator.h>
#include <talipot/PlanarConMap.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

class FaceIteratorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FaceIteratorTest);
    CPPUNIT_TEST(testNodeFaceIterator);
    CPPUNIT_TEST(testFaceAdjIterator);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    tlp::PlanarConMap *map;

    std::vector<tlp::edge> edges;
    std::vector<tlp::node> nodes;

  public:
    void setUp();
    void tearDown();

    void testNodeFaceIterator();
    void testFaceAdjIterator();
};

#endif // FACE_ITERATOR_TEST_H
