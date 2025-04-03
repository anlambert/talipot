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

#ifndef PLANAR_CON_MAP_TEST_H
#define PLANAR_CON_MAP_TEST_H

#include <talipot/PlanarConMap.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

namespace tlp {

class PlanarConMapTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PlanarConMapTest);
    CPPUNIT_TEST(testAddEdgeMap);
    CPPUNIT_TEST(testDelEdgeMap);
    CPPUNIT_TEST(testNbFaces);
    CPPUNIT_TEST(testUpdate);
    CPPUNIT_TEST(testMergeFaces);
    CPPUNIT_TEST(testSplitFace);
    CPPUNIT_TEST(testSuccCycleEdge);
    CPPUNIT_TEST(testPrecCycleEdge);
    CPPUNIT_TEST(testComputeFaces);
    CPPUNIT_TEST_SUITE_END();

  private:
    tlp::Graph *graph;
    tlp::PlanarConMap *carte;

    std::vector<edge> edges;
    std::vector<node> nodes;

    void build();
    void build2();
    void build3();
    void build4();

  public:
    void setUp() {
        graph = tlp::newGraph();
        // carte = new PlanarConMap(graph);
        edges.clear();
        nodes.clear();
    }

    void tearDown() {
        delete carte;
        delete graph;
    }

    void testAddEdgeMap();
    void testDelEdgeMap();
    void testNbFaces();

    void testUpdate();

    void testMergeFaces();
    void testSplitFace();

    void testSuccCycleEdge();
    void testPrecCycleEdge();

    void testComputeFaces();
};
}
#endif // PLANAR_CON_MAP_TEST_H
