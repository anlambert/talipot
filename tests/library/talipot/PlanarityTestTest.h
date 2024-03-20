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

#ifndef PLANARITY_TEST_TEST_H
#define PLANARITY_TEST_TEST_H

#include <string>
#include <talipot/Graph.h>

#include "CppUnitIncludes.h"

class PlanarityTestTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PlanarityTestTest);
  CPPUNIT_TEST(planarGraphs);
  CPPUNIT_TEST(notPlanarGraphs);
  CPPUNIT_TEST(planarGraphsEmbedding);
  CPPUNIT_TEST(planarMetaGraphsEmbedding);
  CPPUNIT_TEST(planarEmbeddingFromLayoutGraphs);
  CPPUNIT_TEST(emptyGraph);
  CPPUNIT_TEST_SUITE_END();

public:
  void planarGraphs();
  void notPlanarGraphs();
  void planarGraphsEmbedding();
  void planarMetaGraphsEmbedding();
  void planarEmbeddingFromLayoutGraphs();
  void emptyGraph();

private:
  tlp::Graph *graph;
};

#endif // PLANARITY_TEST_TEST_H
