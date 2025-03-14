/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "PlanarConMapTest.h"
#include <talipot/PlanarityTest.h>

CPPUNIT_TEST_SUITE_REGISTRATION(tlp::PlanarConMapTest);

using namespace std;
using namespace tlp;

//============================================================
// PlanarConMapTest
//============================================================
void PlanarConMapTest::build() {
  graph->clear();
  edges.clear();
  nodes.clear();

  for (uint i = 0; i < 4; ++i)
    nodes.push_back(graph->addNode());

  edges.push_back(graph->addEdge(nodes[0], nodes[1]));
  edges.push_back(graph->addEdge(nodes[1], nodes[2]));
  edges.push_back(graph->addEdge(nodes[2], nodes[3]));
  edges.push_back(graph->addEdge(nodes[3], nodes[0]));

  delete carte;
  carte = computePlanarConMap(graph);
}

//============================================================
void PlanarConMapTest::build2() {
  graph->clear();
  edges.clear();
  nodes.clear();

  for (uint i = 0; i < 6; ++i)
    nodes.push_back(graph->addNode());

  for (uint i = 1; i < 6; ++i)
    edges.push_back(graph->addEdge(nodes[0], nodes[i]));

  delete carte;
  carte = computePlanarConMap(graph);
}

//============================================================
void PlanarConMapTest::build3() {
  graph->clear();
  edges.clear();
  nodes.clear();

  for (uint i = 0; i < 4; ++i)
    nodes.push_back(graph->addNode());

  edges.push_back(graph->addEdge(nodes[0], nodes[1]));
  edges.push_back(graph->addEdge(nodes[1], nodes[2]));
  edges.push_back(graph->addEdge(nodes[2], nodes[3]));
  edges.push_back(graph->addEdge(nodes[3], nodes[0]));
  edges.push_back(graph->addEdge(nodes[0], nodes[2]));
  edges.push_back(graph->addEdge(nodes[1], nodes[3]));

  delete carte;
  carte = computePlanarConMap(graph);
}

//============================================================
void PlanarConMapTest::build4() {
  graph->clear();
  edges.clear();
  nodes.clear();

  for (uint i = 0; i < 10; ++i)
    nodes.push_back(graph->addNode());

  edges.push_back(graph->addEdge(nodes[0], nodes[1]));
  edges.push_back(graph->addEdge(nodes[0], nodes[2]));
  edges.push_back(graph->addEdge(nodes[0], nodes[3]));
  edges.push_back(graph->addEdge(nodes[2], nodes[4]));
  edges.push_back(graph->addEdge(nodes[2], nodes[5]));
  edges.push_back(graph->addEdge(nodes[1], nodes[6]));
  edges.push_back(graph->addEdge(nodes[6], nodes[7]));
  edges.push_back(graph->addEdge(nodes[4], nodes[8]));
  edges.push_back(graph->addEdge(nodes[4], nodes[9]));

  delete carte;
  carte = computePlanarConMap(graph);
}

//============================================================
void PlanarConMapTest::testAddEdgeMap() {

  /* test 1 */
  build();
  Face f1, f2;
  f1 = carte->faces[0];
  f2 = carte->faces[1];
  edge e = carte->addEdgeMap(nodes[0], nodes[2], f1, edges[0], edges[2]);
  Face f3 = carte->faces[2];
  vector<vector<edge>> cycles(5);
  cycles[0].push_back(edges[1]);
  cycles[1].push_back(edges[2]);
  cycles[0].push_back(edges[0]);
  cycles[1].push_back(edges[3]);
  cycles[0].push_back(e);
  cycles[1].push_back(edges[0]);
  cycles[1].push_back(edges[1]);
  cycles[2].push_back(edges[3]);
  cycles[2].push_back(edges[2]);
  cycles[2].push_back(e);

  for (uint i = 0; i < 3; i++) {
    vector<edge> tmp;
    Iterator<edge> *it;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    if (i == 2)
      it = carte->getFaceEdges(f3);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 1 AddEdgeMap cycle ", cycles[i], tmp);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 1 AddEdgeMap dispositif decorateur ", graph->numberOfEdges(),
                               carte->numberOfEdges());

  /* test 2 */
  build2();
  f1 = carte->faces[0];
  e = carte->addEdgeMap(nodes[1], nodes[3], f1, edges[0], edges[2]);
  f2 = carte->faces[1];
  vector<vector<edge>> cycles2(2);
  cycles2[0].push_back(edges[2]);
  cycles2[0].push_back(edges[3]);
  cycles2[0].push_back(edges[3]);
  cycles2[0].push_back(edges[4]);
  cycles2[0].push_back(edges[4]);
  cycles2[0].push_back(edges[0]);
  cycles2[0].push_back(e);

  cycles2[1].push_back(edges[0]);
  cycles2[1].push_back(edges[1]);
  cycles2[1].push_back(edges[1]);
  cycles2[1].push_back(edges[2]);
  cycles2[1].push_back(e);

  for (uint i = 0; i < 2; i++) {
    vector<edge> tmp;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 2 AddEdgeMap cycle ", cycles2[i], tmp);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 2 AddEdgeMap dispositif decorateur ", graph->numberOfEdges(),
                               carte->numberOfEdges());

  /* test 3 */
  build4();
  f1 = carte->faces[0];
  e = carte->addEdgeMap(nodes[2], nodes[7], f1, edges[4], edges[6]);
  f2 = carte->faces[1];
  edges.push_back(e);
  e = carte->addEdgeMap(nodes[1], nodes[5], f1, edges[5], edges[4]);
  f3 = carte->faces[2];
  edges.push_back(e);
  e = carte->addEdgeMap(nodes[1], nodes[4], f2, edges[0], edges[3]);
  Face f4 = carte->faces[3];
  edges.push_back(e);
  vector<vector<edge>> cycles3(4);
  cycles3[0].push_back(edges[4]);
  cycles3[0].push_back(edges[9]);
  cycles3[0].push_back(edges[6]);
  cycles3[0].push_back(edges[5]);
  cycles3[0].push_back(edges[10]);

  cycles3[1].push_back(edges[8]);
  cycles3[1].push_back(edges[8]);
  cycles3[1].push_back(edges[7]);
  cycles3[1].push_back(edges[7]);
  cycles3[1].push_back(edges[3]);
  cycles3[1].push_back(edges[1]);
  cycles3[1].push_back(edges[2]);
  cycles3[1].push_back(edges[2]);
  cycles3[1].push_back(edges[0]);
  cycles3[1].push_back(edges[11]);

  cycles3[2].push_back(edges[0]);
  cycles3[2].push_back(edges[1]);
  cycles3[2].push_back(edges[4]);
  cycles3[2].push_back(edges[10]);

  cycles3[3].push_back(edges[5]);
  cycles3[3].push_back(edges[6]);
  cycles3[3].push_back(edges[9]);
  cycles3[3].push_back(edges[3]);
  cycles3[3].push_back(edges[11]);

  for (uint i = 0; i < 4; ++i) {
    vector<edge> tmp;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    if (i == 2)
      it = carte->getFaceEdges(f3);

    if (i == 3)
      it = carte->getFaceEdges(f4);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 3 AddEdgeMap cycle ", cycles3[i], tmp);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 3 AddEdgeMap dispositif decorateur ", graph->numberOfEdges(),
                               carte->numberOfEdges());
}

//============================================================
void PlanarConMapTest::testDelEdgeMap() {
  build();
  Face f1, f2;
  f1 = carte->faces[0];
  f2 = carte->faces[1];
  edge e = carte->addEdgeMap(nodes[0], nodes[2], f1, edges[0], edges[2]);

  carte->delEdgeMap(e);
  f1 = carte->faces[0];
  f2 = carte->faces[1];

  vector<vector<edge>> cycles(2);
  cycles[1].push_back(edges[3]);
  cycles[0].push_back(edges[2]);
  cycles[1].push_back(edges[2]);
  cycles[0].push_back(edges[3]);
  cycles[1].push_back(edges[1]);
  cycles[0].push_back(edges[0]);
  cycles[1].push_back(edges[0]);
  cycles[0].push_back(edges[1]);

  for (uint i = 0; i < 2; i++) {
    vector<edge> tmp;
    Iterator<edge> *it;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test DelEdgeMap cycle ", cycles[i], tmp);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test AddEdgeMap dispositif decorateur ", graph->numberOfEdges(),
                               carte->numberOfEdges());
  carte->clear();
}

//============================================================
void PlanarConMapTest::testNbFaces() {

  build();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test nbFaces ", 2, carte->nbFaces());
  carte->clear();
}

//============================================================
void PlanarConMapTest::testUpdate() {

  build();

  uint tmp = carte->nbFaces();
  edge e = graph->addEdge(nodes[0], nodes[2]);
  carte->update();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test update() ", (tmp + 1), carte->nbFaces());
}

//============================================================
void PlanarConMapTest::testMergeFaces() {
  build();
  Face f1, f2;
  f1 = carte->faces[0];
  f2 = carte->faces[1];
  edge e = carte->addEdgeMap(nodes[0], nodes[2], f1, edges[0], edges[2]);

  carte->mergeFaces(carte->edgesFaces[e][0], carte->edgesFaces[e][1]);
  f1 = carte->faces[0];
  f2 = carte->faces[1];

  vector<vector<edge>> cycles(2);
  cycles[1].push_back(edges[3]);
  cycles[0].push_back(edges[2]);
  cycles[1].push_back(edges[2]);
  cycles[0].push_back(edges[3]);
  cycles[1].push_back(edges[1]);
  cycles[0].push_back(edges[0]);
  cycles[1].push_back(edges[0]);
  cycles[0].push_back(edges[1]);

  for (uint i = 0; i < 2; i++) {
    vector<edge> tmp;
    Iterator<edge> *it;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test DelEdgeMap cycle ", tmp, cycles[i]);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test AddEdgeMap dispositif decorateur ", carte->numberOfEdges(),
                               graph->numberOfEdges());
  carte->clear();
}
//============================================================
void PlanarConMapTest::testSplitFace() {

  /* test 1 */
  build();
  Face f1, f2;
  f1 = carte->faces[0];
  f2 = carte->faces[1];
  Face f3 = carte->splitFace(f1, nodes[0], nodes[2]);
  edge e = carte->existEdge(nodes[0], nodes[2]).isValid() ? carte->existEdge(nodes[0], nodes[2])
                                                          : carte->existEdge(nodes[2], nodes[0]);
  vector<vector<edge>> cycles(5);
  cycles[0].push_back(edges[1]);
  cycles[1].push_back(edges[2]);
  cycles[0].push_back(edges[0]);
  cycles[1].push_back(edges[3]);
  cycles[0].push_back(e);
  cycles[1].push_back(edges[0]);
  cycles[1].push_back(edges[1]);
  cycles[2].push_back(edges[3]);
  cycles[2].push_back(edges[2]);
  cycles[2].push_back(e);

  for (uint i = 0; i < 3; i++) {
    Iterator<edge> *it = nullptr;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    if (i == 2)
      it = carte->getFaceEdges(f3);

    vector<edge> tmp = iteratorVector(it);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test AddEdgeMap cycle ", tmp, cycles[i]);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test AddEdgeMap dispositif decorateur ", graph->numberOfEdges(),
                               carte->numberOfEdges());

  /* test 2 */
  build4();

  f1 = carte->faces[0];
  f2 = carte->splitFace(f1, nodes[2], nodes[7]);
  e = carte->existEdge(nodes[2], nodes[7]).isValid() ? carte->existEdge(nodes[2], nodes[7])
                                                     : carte->existEdge(nodes[7], nodes[2]);
  edges.push_back(e);

  f3 = carte->splitFace(f2, nodes[1], nodes[5]);
  e = carte->existEdge(nodes[1], nodes[5]).isValid() ? carte->existEdge(nodes[1], nodes[5])
                                                     : carte->existEdge(nodes[5], nodes[1]);
  edges.push_back(e);

  Face f4 = carte->splitFace(f3, nodes[1], nodes[4]);
  e = carte->existEdge(nodes[1], nodes[4]).isValid() ? carte->existEdge(nodes[1], nodes[4])
                                                     : carte->existEdge(nodes[4], nodes[1]);
  edges.push_back(e);

  vector<vector<edge>> cycles3(4);
  cycles3[0].push_back(edges[4]);
  cycles3[0].push_back(edges[9]);
  cycles3[0].push_back(edges[6]);
  cycles3[0].push_back(edges[5]);
  cycles3[0].push_back(edges[10]);

  cycles3[1].push_back(edges[8]);
  cycles3[1].push_back(edges[8]);
  cycles3[1].push_back(edges[7]);
  cycles3[1].push_back(edges[7]);
  cycles3[1].push_back(edges[3]);
  cycles3[1].push_back(edges[1]);
  cycles3[1].push_back(edges[2]);
  cycles3[1].push_back(edges[2]);
  cycles3[1].push_back(edges[0]);
  cycles3[1].push_back(edges[11]);

  cycles3[2].push_back(edges[0]);
  cycles3[2].push_back(edges[1]);
  cycles3[2].push_back(edges[4]);
  cycles3[2].push_back(edges[10]);

  cycles3[3].push_back(edges[5]);
  cycles3[3].push_back(edges[6]);
  cycles3[3].push_back(edges[9]);
  cycles3[3].push_back(edges[3]);
  cycles3[3].push_back(edges[11]);

  for (uint i = 0; i < 4; ++i) {
    vector<edge> tmp;
    Iterator<edge> *it;

    if (i == 0)
      it = carte->getFaceEdges(f1);

    if (i == 1)
      it = carte->getFaceEdges(f2);

    if (i == 2)
      it = carte->getFaceEdges(f3);

    if (i == 3)
      it = carte->getFaceEdges(f4);

    for (auto e_tmp : it) {
      tmp.push_back(e_tmp);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 2 AddEdgeMap cycle ", tmp == cycles3[i]);
  }

  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test 2 AddEdgeMap dispositif decorateur ",
                               carte->numberOfEdges() == graph->numberOfEdges());
}

//============================================================
void PlanarConMapTest::testSuccCycleEdge() {

  build2();

  vector<edge> cycles;
  cycles.push_back(edges[0]);
  cycles.push_back(edges[1]);
  cycles.push_back(edges[2]);
  cycles.push_back(edges[3]);
  cycles.push_back(edges[4]);

  uint i = 0;
  Iterator<edge> *it = carte->getInOutEdges(nodes[0]);
  edge e = it->next();
  delete it;
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test SuccCycleEdge deb", cycles[0], e);

  do {
    e = carte->succCycleEdge(e, nodes[0]);
    i++;
    CPPUNIT_ASSERT_EQUAL_MESSAGE(string(" test SuccCycleEdge cycle "), cycles[i], e);
  } while (e != edges[0] && i < 4);

  carte->clear();
}

//============================================================
void PlanarConMapTest::testPrecCycleEdge() {

  build2();

  vector<edge> cycles;
  cycles.push_back(edges[0]);
  cycles.push_back(edges[4]);
  cycles.push_back(edges[3]);
  cycles.push_back(edges[2]);
  cycles.push_back(edges[1]);

  uint i = 0;
  Iterator<edge> *it = carte->getInOutEdges(nodes[0]);
  edge e = it->next();
  delete it;
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test predCycleEdge deb", cycles[0], e);

  do {
    e = carte->predCycleEdge(e, nodes[0]);
    i++;
    CPPUNIT_ASSERT_EQUAL_MESSAGE(string(" test predCycleEdge cycle "), cycles[i], e);
  } while ((e != edges[0]) && (i < 4));

  carte->clear();
}

//============================================================
void PlanarConMapTest::testComputeFaces() {

  build();
  int fc = carte->nbFaces();
  int e = carte->numberOfEdges();
  int n = carte->numberOfNodes();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test ComputesFaces 1", e - n + 2, fc);
  carte->clear();

  build2();
  fc = carte->nbFaces();
  e = carte->numberOfEdges();
  n = carte->numberOfNodes();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test ComputesFaces 2", e - n + 2, fc);
  carte->clear();

  build3();
  fc = carte->nbFaces();
  e = carte->numberOfEdges();
  n = carte->numberOfNodes();
  CPPUNIT_ASSERT_EQUAL_MESSAGE(" test ComputesFaces 3 ", e - n + 2, fc);
  carte->clear();
}
