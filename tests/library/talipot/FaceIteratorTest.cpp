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

#include "FaceIteratorTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FaceIteratorTest);

//============================================================
// FaceIteratorTest
//============================================================
void FaceIteratorTest::setUp() {
    map = computePlanarConMap(graph = tlp::newGraph());

    map->clear();
    edges.clear();
    nodes.clear();

    for (uint i = 0; i < 10; i++) {
        nodes.push_back(map->addNode());
    }

    edges.push_back(map->addEdge(nodes[0], nodes[1]));
    edges.push_back(map->addEdge(nodes[1], nodes[2]));
    edges.push_back(map->addEdge(nodes[3], nodes[2]));

    edges.push_back(map->addEdge(nodes[4], nodes[3]));
    edges.push_back(map->addEdge(nodes[3], nodes[5]));
    edges.push_back(map->addEdge(nodes[3], nodes[6]));

    edges.push_back(map->addEdge(nodes[7], nodes[6]));
    edges.push_back(map->addEdge(nodes[7], nodes[1]));

    edges.push_back(map->addEdge(nodes[0], nodes[8]));
    edges.push_back(map->addEdge(nodes[8], nodes[9]));
    edges.push_back(map->addEdge(nodes[0], nodes[9]));

    edges.push_back(map->addEdge(nodes[4], nodes[2]));
    edges.push_back(map->addEdge(nodes[4], nodes[5]));
    edges.push_back(map->addEdge(nodes[7], nodes[9]));

    map->update();
}

void FaceIteratorTest::tearDown() {
    delete map;
    delete graph;
}

//============================================================
void FaceIteratorTest::testNodeFaceIterator() {
    for (tlp::Face f : map->getFaces()) {
        uint i = iteratorCount(map->getFaceNodes(f));
        CPPUNIT_ASSERT_EQUAL_MESSAGE(" test NodeFaceIterator ", map->nbFacesEdges(f), i);
    }
}

//============================================================
void FaceIteratorTest::testFaceAdjIterator() {
    uint i = iteratorCount(map->getFacesAdj(nodes[4]));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test FaceAdjIterator nbFaces ", 3u, i);

    i = iteratorCount(map->getFacesAdj(nodes[1]));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test FaceadjIterator nbFaces ", 3u, i);

    i = iteratorCount(map->getFacesAdj(nodes[8]));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test FaceAdjIterator nbFaces ", 2u, i);
}
