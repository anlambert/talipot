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

#include <talipot/PlanarityTest.h>
#include <talipot/PlanarConMap.h>
#include <talipot/ConnectedTest.h>
#include <talipot/LayoutProperty.h>
#include "PlanarityTestTest.h"

using namespace std;
using namespace tlp;

const std::string GRAPHPATH = "./DATA/graphs/";

CPPUNIT_TEST_SUITE_REGISTRATION(PlanarityTestTest);
//==========================================================
void PlanarityTestTest::planarGraphs() {
  graph = tlp::loadGraph(GRAPHPATH + "planar/grid1010.tlp");
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "planar/unconnected.tlp");
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "planar/unbiconnected.tlp");
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(graph));
  delete graph;
}
//==========================================================
/*
 * TODO: move that function in LayoutProperty test.
 *
 */
void PlanarityTestTest::planarEmbeddingFromLayoutGraphs() {
  graph = tlp::loadGraph(GRAPHPATH + "planar/planar30drawnFPP.tlp.gz");
  LayoutProperty *layout = graph->getLayoutProperty("viewLayout");
  layout->computeEmbedding(graph);
  CPPUNIT_ASSERT(PlanarityTest::isPlanarEmbedding(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "planar/planar30drawnMM.tlp.gz");
  layout = graph->getLayoutProperty("viewLayout");
  layout->computeEmbedding(graph);
  CPPUNIT_ASSERT(PlanarityTest::isPlanarEmbedding(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "notplanar/k33lostInGrip.tlp.gz");
  layout = graph->getLayoutProperty("viewLayout");
  layout->computeEmbedding(graph);
  CPPUNIT_ASSERT(!PlanarityTest::isPlanarEmbedding(graph));
  delete graph;
}
//==========================================================
void PlanarityTestTest::notPlanarGraphs() {
  graph = tlp::loadGraph(GRAPHPATH + "notplanar/k33lostInGrip.tlp.gz");
  CPPUNIT_ASSERT(!PlanarityTest::isPlanar(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "notplanar/k33k55.tlp.gz");
  CPPUNIT_ASSERT(!PlanarityTest::isPlanar(graph));
  delete graph;
  graph = tlp::loadGraph(GRAPHPATH + "notplanar/k5lostingrid5050.tlp.gz");
  CPPUNIT_ASSERT(!PlanarityTest::isPlanar(graph));
  delete graph;
}
//==========================================================
uint eulerIdentity(Graph *graph) {
  return graph->numberOfEdges() - graph->numberOfNodes() + 1u +
         ConnectedTest::numberOfConnectedComponents(graph);
}
//==========================================================
void PlanarityTestTest::planarGraphsEmbedding() {
  tlp::warning() << "==================================" << endl;
  graph = tlp::loadGraph(GRAPHPATH + "planar/grid1010.tlp");
  PlanarConMap *graphMap = computePlanarConMap(graph);
  //  graphMap->makePlanar();
  CPPUNIT_ASSERT_EQUAL(eulerIdentity(graph), graphMap->nbFaces());
  delete graphMap;
  delete graph;
  tlp::warning() << "==================================" << endl;
  graph = tlp::loadGraph(GRAPHPATH + "planar/unconnected.tlp");
  graph->setAttribute("name", string("unconnected"));
  // no planar connected map computed
  // because is not connected
  graphMap = computePlanarConMap(graph);
  CPPUNIT_ASSERT(graphMap == nullptr);
  delete graph;
  tlp::warning() << "==================================" << endl;
  tlp::warning() << "unbiconnected" << endl;
  graph = tlp::loadGraph(GRAPHPATH + "planar/unbiconnected.tlp");

  graphMap = computePlanarConMap(graph);

  //  graphMap->makePlanar();
  CPPUNIT_ASSERT_EQUAL(eulerIdentity(graph), graphMap->nbFaces());

  delete graphMap;
  delete graph;
  tlp::warning() << "==================================" << endl;
}
//==========================================================
void PlanarityTestTest::planarMetaGraphsEmbedding() {
  tlp::warning() << "===========MetaGraphsEmbedding=======================" << endl;
  graph = tlp::loadGraph(GRAPHPATH + "planar/grid1010.tlp");
  Graph *g = graph->addCloneSubGraph();
  vector<node> toGroup;
  toGroup.reserve(10);
  const std::vector<node> &nodes = graph->nodes();

  for (uint i = 0; i < 10; ++i) {
    toGroup.push_back(nodes[i]);
  }

  g->createMetaNode(toGroup);
  toGroup.clear();

  for (uint i = 10; i < 20; ++i) {
    toGroup.push_back(nodes[i]);
  }

  node meta2 = g->createMetaNode(toGroup);
  toGroup.clear();
  toGroup.push_back(meta2);

  for (uint i = 20; i < 30; ++i) {
    toGroup.push_back(nodes[i]);
  }

  g->createMetaNode(toGroup, false);
  toGroup.clear();

  PlanarConMap *graphMap = computePlanarConMap(g);
  //  graphMap->makePlanar();
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(g));        // eulerIdentity(g), graphMap->nbFaces());
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(graphMap)); // eulerIdentity(g), graphMap->nbFaces());
  delete graphMap;
  graph->delSubGraph(g);
  delete graph;
  tlp::warning() << "==================================" << endl;
  /*
    graph = tlp::loadGraph(GRAPHPATH + "planar/unconnected.tlp");
    graph->setAttribute("name", string("unconnected"));
    graphMap = new PlanarConMap(graph);
    tlp::warning() << "Graph name : " << graph->getAttribute<string>("name") << endl;
    graphMap->makePlanar();*/
  /*
   * The number of faces must be adapted because the Planarity Test split the
   * external face into several faces (one by connected component).
   */
  /*  CPPUNIT_ASSERT_EQUAL(eulerIdentity(graph), graphMap->nbFaces() -
     (ConnectedTest::numberOfConnectedComponnents(graph) - 1));
      delete graphMap;
      delete graph;
      tlp::warning() << "==================================" << endl;
      tlp::warning() << "unbiconnected" << endl;
      graph = tlp::loadGraph(GRAPHPATH + "planar/unbiconnected.tlp");

      graphMap = new PlanarConMap(graph);

      graphMap->makePlanar();
      CPPUNIT_ASSERT_EQUAL(eulerIdentity(graph), graphMap->nbFaces());

      delete graphMap;
      delete graph;
      tlp::warning() << "==================================" << endl;*/
}

void PlanarityTestTest::emptyGraph() {
  graph = tlp::newGraph();
  CPPUNIT_ASSERT(PlanarityTest::isPlanar(graph));
  delete graph;
}
