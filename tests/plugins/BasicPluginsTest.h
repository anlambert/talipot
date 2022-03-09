/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef BASIC_PLUGINS_TEST_H
#define BASIC_PLUGINS_TEST_H

#include <string>

#include "CppUnitIncludes.h"

#include <talipot/Graph.h>

class BasicPluginsTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(BasicPluginsTest);
  CPPUNIT_TEST(testImportCompleteGraph);
  CPPUNIT_TEST(testImportCompleteTree);
  CPPUNIT_TEST(testImportRandomGraph);
  CPPUNIT_TEST(testImportRandomTree);
  CPPUNIT_TEST(testImportRandomTreeGeneral);
  CPPUNIT_TEST(testImportRandomSimpleGraph);
  CPPUNIT_TEST(testImportPlanarGraph);
  CPPUNIT_TEST(testImportGrid);
  CPPUNIT_TEST(testImportGridApproximation);
  CPPUNIT_TEST(testImportGraphviz);
  CPPUNIT_TEST(testImportGml);
  CPPUNIT_TEST(testExportGml);
  CPPUNIT_TEST(testImportTLP);
  CPPUNIT_TEST(testExportTLP);
  CPPUNIT_TEST(testExportImportTLPB);
  CPPUNIT_TEST(testExportImportJSON);
  CPPUNIT_TEST(testImportAdjacencyMatrix);
  CPPUNIT_TEST(testImportBibTeX);
  CPPUNIT_TEST(testImportPajek);
  CPPUNIT_TEST(testImportUCINET);
  CPPUNIT_TEST(testMetricColorMapping);
  CPPUNIT_TEST(testInducedSubGraphSelection);
  CPPUNIT_TEST(testLoopSelection);
  CPPUNIT_TEST(testParallelEdgesSelection);
  CPPUNIT_TEST(testReachableSubGraphSelection);
  CPPUNIT_TEST(testSpanningDagSelection);
  CPPUNIT_TEST(testSpanningTreeSelection);
  CPPUNIT_TEST(testAutoSize);
  CPPUNIT_TEST(testMetricSizeMapping);
  CPPUNIT_TEST(testEqualValueClustering);
  CPPUNIT_TEST(testHierarchicalClustering);
  CPPUNIT_TEST(testQuotientClustering);
  CPPUNIT_TEST(testStrengthClustering);
#ifndef TALIPOT_BUILD_CORE_ONLY
  CPPUNIT_TEST(testImportFileSystem);
  CPPUNIT_TEST(testImportGEXF);
#endif
  CPPUNIT_TEST_SUITE_END();

private:
  tlp::Graph *graph;

public:
  void setUp() override;
  void tearDown() override;
  void initializeGraph(const std::string &type);
  template <typename PropType>
  bool computeProperty(const std::string &algorithm, const std::string &graphType = "Planar Graph",
                       PropType *prop = nullptr);
  void testImportCompleteGraph();
  void testImportCompleteTree();
  void testImportRandomGraph();
  void testImportRandomTree();
  void testImportRandomTreeGeneral();
  void testImportRandomSimpleGraph();
  void testImportPlanarGraph();
  void testImportGrid();
  void testImportGridApproximation();
  void testImportGraphviz();
  void testImportGml();
  void testExportGml();
  void testImportTLP();
  void testExportTLP();
  void testExportImportTLPB();
  void testExportImportJSON();
  void testImportAdjacencyMatrix();
  void testImportBibTeX();
  void testImportPajek();
  void testImportUCINET();
  void testMetricColorMapping();
  void testInducedSubGraphSelection();
  void testLoopSelection();
  void testParallelEdgesSelection();
  void testReachableSubGraphSelection();
  void testSpanningDagSelection();
  void testSpanningTreeSelection();
  void testAutoSize();
  void testFitToLabel();
  void testMetricSizeMapping();
  void testEqualValueClustering();
  void testHierarchicalClustering();
  void testQuotientClustering();
  void testStrengthClustering();

#ifndef TALIPOT_BUILD_CORE_ONLY
  void testImportFileSystem();
  void testImportGEXF();
#endif
};

#endif // BASIC_PLUGINS_TEST_H
