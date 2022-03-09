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

#include "BasicPluginsTest.h"
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/SimplePluginProgress.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(BasicPluginsTest);

//==========================================================
void BasicPluginsTest::setUp() {
  graph = tlp::newGraph();
}
//==========================================================
void BasicPluginsTest::tearDown() {
  delete graph;
}
//==========================================================
void BasicPluginsTest::initializeGraph(const string &type) {
  DataSet ds;
  Graph *g = importGraph(type, ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
template <typename PropType>
bool BasicPluginsTest::computeProperty(const std::string &algorithm, const std::string &graphType,
                                       PropType *prop) {
  initializeGraph(graphType);
  bool deleteProp = (prop == nullptr);

  if (prop == nullptr) {
    prop = new PropType(graph);
  }

  string errorMsg;
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm(algorithm, prop, errorMsg);

  if (deleteProp) {
    delete prop;
  }

  return result;
}
//==========================================================
void BasicPluginsTest::testImportCompleteGraph() {
  DataSet ds;
  Graph *g = importGraph("Complete General Graph", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportCompleteTree() {
  DataSet ds;
  Graph *g = importGraph("Complete Tree", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportRandomGraph() {
  DataSet ds;
  Graph *g = importGraph("Random General Graph", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportRandomTree() {
  DataSet ds;
  Graph *g = importGraph("Uniform Random Binary Tree", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportRandomTreeGeneral() {
  DataSet ds;
  Graph *g = importGraph("Random General Tree", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportRandomSimpleGraph() {
  DataSet ds;
  Graph *g = importGraph("Random Simple Graph", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportPlanarGraph() {
  DataSet ds;
  Graph *g = importGraph("Planar Graph", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportGrid() {
  DataSet ds;
  Graph *g = importGraph("Grid", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportGridApproximation() {
  DataSet ds;
  Graph *g = importGraph("Grid Approximation", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportGraphviz() {
  DataSet ds;
  ds.set("file::filename", string("data/toto.dot"));
  Graph *g = importGraph("Graphviz", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/graph.dot"));
  g = importGraph("Graphviz", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  CPPUNIT_ASSERT_EQUAL(9u, graph->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(8u, graph->numberOfEdges());
}
//==========================================================
void BasicPluginsTest::testImportGml() {
  DataSet ds;
  ds.set("file::filename", string("data/toto.gml"));
  Graph *g = importGraph("GML", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/CMPb.gml"));
  g = importGraph("GML", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testExportGml() {
  DataSet ds;
  Graph *g = importGraph("Planar Graph", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  std::stringstream os;
  CPPUNIT_ASSERT(exportGraph(graph, os, "GML Export", ds, nullptr));
}
//==========================================================
void BasicPluginsTest::testImportTLP() {
  DataSet ds;
  ds.set("file::filename", string("data/toto.tlp"));
  Graph *g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/tlp_importexport_test.tlp"));
  g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  ds.set("file::filename", string("data/graph.dot"));
  g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
}
//==========================================================
void BasicPluginsTest::testExportTLP() {
  DataSet ds;
  ds.set("file::filename", string("data/tlp_importexport_test.tlp"));
  Graph *g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  std::stringstream os;
  CPPUNIT_ASSERT(exportGraph(graph, os, "TLP Export", ds, nullptr));
}
//==========================================================
void BasicPluginsTest::testExportImportTLPB() {
  DataSet ds;
  ds.set("file::filename", string("data/tlp_importexport_test.tlp"));
  Graph *g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  std::ostream *os = tlp::getOutputFileStream("tlpb_importexport_test.tlpb");
  CPPUNIT_ASSERT(exportGraph(graph, *os, "TLPB Export", ds, nullptr));
  delete os;
  tearDown();
  setUp();
  ds.set("file::filename", string("data/toto.tlpb"));
  g = importGraph("TLPB Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("tlpb_importexport_test.tlpb"));
  g = importGraph("TLPB Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testExportImportJSON() {
  DataSet ds;
  ds.set("file::filename", string("data/tlp_importexport_test.tlp"));
  Graph *g = importGraph("TLP Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
  std::ofstream os("json_importexport_test.json");
  CPPUNIT_ASSERT(exportGraph(graph, os, "JSON Export", ds, nullptr));
  os.close();
  tearDown();
  setUp();
  ds.set("file::filename", string("data/toto.json"));
  g = importGraph("JSON Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("json_importexport_test.json"));
  g = importGraph("JSON Import", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportAdjacencyMatrix() {
  DataSet ds;
  ds.set("file::filename", string("data/toto.txt"));
  Graph *g = importGraph("Adjacency Matrix", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/adj_mat.txt"));
  g = importGraph("Adjacency Matrix", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportBibTeX() {
  DataSet ds;
  ds.set("file::filename", string("data/toto.bib"));
  Graph *g = importGraph("BibTeX", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/adj_mat.txt"));
  g = importGraph("BibTeX", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("file::filename", string("data/teachpress_pub_17012022.bib"));
  g = importGraph("BibTeX", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportPajek() {
  // test all data/*.net files
  list<string> net_files = {"data/NDactors.net", "data/NDwww.net", "data/netscience.net"};

  for (const auto &file : net_files) {
    DataSet ds;
    ds.set("file::filename", string(file));
    Graph *g = importGraph("Pajek", ds, nullptr, graph);
    CPPUNIT_ASSERT(g == graph);
    g->clear();
  }
}
//==========================================================
void BasicPluginsTest::testImportUCINET() {
  // test all data/dl_*.txt files
  list<string> dl_files = {"data/dl_el1_test_labels_embedded.txt",
                           "data/dl_el1_test_labels.txt",
                           "data/dl_el1_test_multiple_labels_embedded.txt",
                           "data/dl_el2_test2_labels_embedded.txt",
                           "data/dl_el2_test_labels_embedded.txt",
                           "data/dl_fm_test2.txt",
                           "data/dl_fm_test3.txt",
                           "data/dl_fm_test_labels_no_diag.txt",
                           "data/dl_fm_test2_labels_no_diag.txt",
                           "data/dl_fm_test_labels.txt",
                           "data/dl_fm_test_multi_matrices.txt",
                           "data/dl_fm_test_rect_labels_embedded.txt",
                           "data/dl_fm_test_rect_labels.txt",
                           "data/dl_fm_test_rect.txt",
                           "data/dl_fm_test.txt",
                           "data/dl_lh_test_labels_no_diag.txt",
                           "data/dl_lh_test_labels.txt",
                           "data/dl_nl1_test2_labels_embedded.txt",
                           "data/dl_nl1_test2_labels.txt",
                           "data/dl_nl1_test_labels_embedded.txt",
                           "data/dl_nl1_test_labels.txt",
                           "data/dl_nl2_test_row_col_labels_embedded.txt"};

  for (const auto &file : dl_files) {
    DataSet ds;
    ds.set("file::filename", file);
    Graph *g = importGraph("UCINET", ds, nullptr, graph);
    CPPUNIT_ASSERT(g == graph);
    g->clear();
  }
}
//==========================================================
void BasicPluginsTest::testMetricColorMapping() {
  initializeGraph("Planar Graph");
  DoubleProperty metric(graph);
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Degree", &metric, errorMsg);
  CPPUNIT_ASSERT(result);

  DataSet ds;
  ds.set("linear/uniform\nproperty", &metric);
  ColorProperty color(graph);
  result = graph->applyPropertyAlgorithm("Color Mapping", &color, errorMsg, &ds, nullptr);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testInducedSubGraphSelection() {
  bool result = computeProperty<BooleanProperty>("Induced SubGraph");
  CPPUNIT_ASSERT(result);
  // check with old name
  result = computeProperty<BooleanProperty>("Induced SubGraph");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testLoopSelection() {
  BooleanProperty selection(graph);
  bool result = computeProperty<BooleanProperty>("Loop Selection", "Planar Graph", &selection);
  CPPUNIT_ASSERT(result);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(selection.getNodeValue(n) == false);
  }
}
//==========================================================
void BasicPluginsTest::testParallelEdgesSelection() {
  BooleanProperty selection(graph);
  bool result =
      computeProperty<BooleanProperty>("Parallel Edges Selection", "Planar Graph", &selection);
  CPPUNIT_ASSERT(result);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(selection.getNodeValue(n) == false);
  }
}
//==========================================================
void BasicPluginsTest::testReachableSubGraphSelection() {
  bool result = computeProperty<BooleanProperty>("Reachable SubGraph");
  CPPUNIT_ASSERT(result);
  // check with old name
  result = computeProperty<BooleanProperty>("Reachable SubGraph");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testSpanningDagSelection() {
  BooleanProperty selection(graph);
  bool result = computeProperty<BooleanProperty>("Spanning Dag", "Planar Graph", &selection);
  CPPUNIT_ASSERT(result);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(selection.getNodeValue(n));
  }
}
//==========================================================
void BasicPluginsTest::testSpanningTreeSelection() {
  BooleanProperty selection(graph);
  bool result = computeProperty<BooleanProperty>("Spanning Forest", "Planar Graph", &selection);
  CPPUNIT_ASSERT(result);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(selection.getNodeValue(n));
  }
}
//==========================================================
void BasicPluginsTest::testAutoSize() {
  bool result = computeProperty<SizeProperty>("Auto Sizing");
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testMetricSizeMapping() {
  initializeGraph("Planar Graph");
  DoubleProperty metric(graph);
  string errorMsg;
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm("Degree", &metric, errorMsg);
  CPPUNIT_ASSERT(result);

  SizeProperty size(graph);
  ds.set("property", &metric);
  result = graph->applyPropertyAlgorithm("Size Mapping", &size, errorMsg, &ds, nullptr);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testEqualValueClustering() {
  string errorMsg;
  DataSet ds;
  bool result;
  const std::string algorithmName = "Equal Value";

  DoubleProperty *metric = graph->getDoubleProperty("metric");
  ds.set("Property", metric);

  result = graph->applyAlgorithm(algorithmName, errorMsg, &ds);
  CPPUNIT_ASSERT_MESSAGE(errorMsg, result);

  // fill graph & metric
  vector<node> nodes;
  vector<edge> edges;
  uint NB_ADD = 100;
  uint EDGE_RATIO = 100;

  for (uint i = 0; i < NB_ADD; ++i) {
    nodes.push_back(graph->addNode());
    metric->setNodeValue(nodes[i], randomUnsignedInteger(NB_ADD - 1));
  }

  uint NB_EDGES = EDGE_RATIO * NB_ADD;

  for (uint i = 0; i < NB_EDGES; ++i) {
    graph->addEdge(graph->getRandomNode(), graph->getRandomNode());
  }

  // check dcall to computeEqualValueClustering
  result = graph->applyAlgorithm(algorithmName, errorMsg, &ds);
  CPPUNIT_ASSERT_MESSAGE(errorMsg, result);

  graph->clear();

  PluginProgress *progress = new SimplePluginProgress();
  initializeGraph("Planar Graph");
  result = graph->applyPropertyAlgorithm("Degree", metric, errorMsg, nullptr, progress);
  CPPUNIT_ASSERT_MESSAGE(errorMsg, result);
  result = graph->applyAlgorithm(algorithmName, errorMsg, &ds);
  CPPUNIT_ASSERT_MESSAGE(errorMsg, result);
  delete progress;
}
//==========================================================
void BasicPluginsTest::testHierarchicalClustering() {
  initializeGraph("Planar Graph");
  DoubleProperty *metric = graph->getDoubleProperty("viewMetric");
  string errorMsg;
  bool result = graph->applyPropertyAlgorithm("Degree", metric, errorMsg);
  CPPUNIT_ASSERT(result);
  result = graph->applyAlgorithm("Hierarchical", errorMsg);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testQuotientClustering() {
  initializeGraph("Planar Graph");
  DoubleProperty metric(graph);
  string errorMsg;
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm("Degree", &metric, errorMsg);
  CPPUNIT_ASSERT(result);
  ds.set("Property", &metric);
  result = graph->applyAlgorithm("Equal Value", errorMsg, &ds);
  CPPUNIT_ASSERT(result);
  result = graph->applyAlgorithm("Quotient Clustering", errorMsg);
  CPPUNIT_ASSERT(result);
}
//==========================================================
void BasicPluginsTest::testStrengthClustering() {
  initializeGraph("Planar Graph");
  string errorMsg;
  DoubleProperty metric(graph);
  DataSet ds;
  bool result = graph->applyPropertyAlgorithm("Degree", &metric, errorMsg);
  CPPUNIT_ASSERT(result);
  ds.set("metric", &metric);
  DoubleProperty resultMetric(graph);
  result = graph->applyPropertyAlgorithm("Strength Clustering", &resultMetric, errorMsg);
  CPPUNIT_ASSERT(result);
}
//==========================================================
#ifndef TALIPOT_BUILD_CORE_ONLY
void BasicPluginsTest::testImportFileSystem() {
  DataSet ds;
  ds.set("dir::directory", string("toto"));
  Graph *g = importGraph("File System Directory", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == nullptr);
  ds.set("dir::directory", string(".."));
  g = importGraph("File System Directory", ds, nullptr, graph);
  CPPUNIT_ASSERT(g == graph);
}
//==========================================================
void BasicPluginsTest::testImportGEXF() {
  // test all data/*.net files
  list<string> gexf_files = {
      "data/basic.gexf",      "data/data.gexf",       "data/hierarchy1.gexf",
      "data/hierarchy2.gexf", "data/hierarchy3.gexf", "data/hierarchy4.gexf",
      "data/phylogeny.gexf",  "data/viz.gexf",        "data/WebAtlas_EuroSiS.gexf"};

  for (const auto &file : gexf_files) {
    DataSet ds;
    ds.set("file::filename", file);
    Graph *g = importGraph("GEXF", ds, nullptr, graph);
    CPPUNIT_ASSERT(g == graph);
    g->clear();
  }
}
#endif
