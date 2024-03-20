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

#include "TlpImportExportTest.h"
#include <talipot/Color.h>
#include <talipot/Coord.h>
#include <talipot/Size.h>

using namespace std;
using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(TlpImportExportTest);

//==========================================================
void TlpImportExportTest::testImport() {
  DataSet dataSet;
  dataSet.set("file::filename", std::string("cette erreur fait partie de testImport"));
  Graph *graph = tlp::importGraph("TLP Import", dataSet, nullptr);
  CPPUNIT_ASSERT(graph == nullptr);
  dataSet.set("file::filename", std::string("TlpImportExportTest.cpp"));
  graph = tlp::importGraph("TLP Import", dataSet, nullptr);
  CPPUNIT_ASSERT(graph == nullptr);
  dataSet.set("file::filename", std::string("DATA/filesystem.tlp.gz"));
  graph = tlp::importGraph("TLP Import", dataSet, nullptr);
  CPPUNIT_ASSERT(graph != nullptr);
  delete graph;
}
//==========================================================
void TlpImportExportTest::testSave() {
  Graph *graph = newGraph();
  CPPUNIT_ASSERT(graph != nullptr);
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  edge e1 = graph->addEdge(n1, n2);
  bool ok = saveGraph(graph, "save_test.tlp");
  delete graph;
  CPPUNIT_ASSERT(ok);
  graph = nullptr;
  graph = loadGraph("save_test.tlp");
  CPPUNIT_ASSERT(graph != nullptr);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT((n == n1) || (n == n2));
  }
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT(e == e1);
  }
  delete graph;
}
//==========================================================
void TlpImportExportTest::testExport() {
  Graph *graph = newGraph();
  CPPUNIT_ASSERT(graph != nullptr);
  node n1 = graph->addNode();
  // for sf bug  #2999413
  graph->delNode(n1);
  n1 = graph->addNode();
  node n2 = graph->addNode();
  edge e1 = graph->addEdge(n1, n2);
  ostream *os = new ofstream("export_test.tlp");
  DataSet dataSet;
  bool ok = exportGraph(graph, *os, "TLP Export", dataSet);
  delete graph;
  delete os;
  CPPUNIT_ASSERT(ok);
  graph = nullptr;
  graph = tlp::loadGraph("export_test.tlp");
  CPPUNIT_ASSERT(graph != nullptr);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT((n == node(n1.id)) || (n == node(n2.id)));
  }
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT_EQUAL(e1, e);
  }
  delete graph;
}
//==========================================================
void TlpImportExportTest::testExportCluster() {
  Graph *graph = newGraph();
  CPPUNIT_ASSERT(graph != nullptr);
  node n1 = graph->addNode();
  node n2 = graph->addNode();
  edge e1 = graph->addEdge(n1, n2);
  Graph *sg = graph->addSubGraph("\"name with double quotes \"");
  sg->addNode(n1);
  sg->addNode(n2);
  ostream *os = new ofstream("export_test.tlp");
  DataSet dataSet;
  bool ok = exportGraph(graph, *os, "TLP Export", dataSet);
  delete graph;
  delete os;
  CPPUNIT_ASSERT(ok);
  graph = nullptr;
  graph = tlp::loadGraph("export_test.tlp");
  CPPUNIT_ASSERT(graph != nullptr);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT((n == n1) || (n == n2));
  }
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT_EQUAL(e1, e);
  }
  for (Graph *g : graph->getSubGraphs()) {
    string name;
    g->getAttribute(string("name"), name);
    CPPUNIT_ASSERT_EQUAL(string("\"name with double quotes \""), name);
    for (auto n : g->nodes()) {
      CPPUNIT_ASSERT((n == n1) || (n == n2));
    }
  }
  delete graph;
}
//==========================================================
void TlpImportExportTest::testExportAttributes() {
  Graph *graph = newGraph();
  ostream *os = new ofstream("export_attributes.tlp");
  bool b = true, sb = false;
  double d = 100., sd = 50.;
  Color color = {1, 2, 3, 4}, scolor = {4, 3, 2, 1};
  Coord coord = {5., 6., 7.}, scoord = {0., 8., 9.};
  float f = 200., sf = 100.;
  int i = -10, si = -5;
  uint ui = 10, sui = 5;
  Size size = {8., 9., 10.}, ssize = {10., 9., 8.};
  string str = "data", sstr = "atad";
  vector<string> sv;
  sv.push_back(str);
  sv.push_back(sstr);

  // set graph attributes of different types
  graph->setAttribute("type = bool", b);
  graph->setAttribute("type = double", d);
  graph->setAttribute("type = color", color);
  graph->setAttribute("type = coord", coord);
  graph->setAttribute("type = float", f);
  graph->setAttribute("type = int", i);
  graph->setAttribute("type = uint", ui);
  graph->setAttribute("type = size", size);
  graph->setAttribute("type = string", str);
  graph->setAttribute("type = stringvector", sv);

  Graph *sg = graph->addSubGraph("sg");
  CPPUNIT_ASSERT_EQUAL(1u, sg->getId());

  // set sg attributes of different types
  sg->setAttribute("type = bool", sb);
  sg->setAttribute("type = double", sd);
  sg->setAttribute("type = color", scolor);
  sg->setAttribute("type = coord", scoord);
  sg->setAttribute("type = float", sf);
  sg->setAttribute("type = int", si);
  sg->setAttribute("type = uint", sui);
  sg->setAttribute("type = size", ssize);
  sg->setAttribute("type = string", sstr);

  DataSet dataSet;
  bool ok = exportGraph(graph, *os, "TLP Export", dataSet);
  delete graph;
  delete os;
  CPPUNIT_ASSERT(ok);
  graph = nullptr;
  graph = tlp::loadGraph("export_attributes.tlp");
  CPPUNIT_ASSERT(graph);

  b = false;
  CPPUNIT_ASSERT(graph->getAttribute("type = bool", b));
  CPPUNIT_ASSERT(b);
  d = 0.;
  CPPUNIT_ASSERT(graph->getAttribute("type = double", d));
  CPPUNIT_ASSERT(d == 100.);
  color = Color(0, 0, 0, 0);
  CPPUNIT_ASSERT(graph->getAttribute("type = color", color));
  CPPUNIT_ASSERT(color == Color(1, 2, 3, 4));
  coord = Coord(0., 0., 0);
  CPPUNIT_ASSERT(graph->getAttribute("type = coord", coord));
  CPPUNIT_ASSERT_EQUAL(Coord(5., 6., 7.), coord);
  f = 0.;
  CPPUNIT_ASSERT(graph->getAttribute("type = float", f));
  CPPUNIT_ASSERT_EQUAL(200.0f, f);
  i = 0;
  CPPUNIT_ASSERT(graph->getAttribute("type = int", i));
  CPPUNIT_ASSERT_EQUAL(-10, i);
  ui = 0;
  CPPUNIT_ASSERT(graph->getAttribute("type = uint", ui));
  CPPUNIT_ASSERT_EQUAL(10u, ui);
  size = Size(0., 0., 0.);
  CPPUNIT_ASSERT(graph->getAttribute("type = size", size));
  CPPUNIT_ASSERT_EQUAL(Size(8., 9., 10.), size);
  str = string("");
  CPPUNIT_ASSERT(graph->getAttribute("type = string", str));
  CPPUNIT_ASSERT_EQUAL(string("data"), str);
  sv.clear();
  CPPUNIT_ASSERT(graph->getAttribute("type = stringvector", sv));
  CPPUNIT_ASSERT(sv[0] == "data");
  CPPUNIT_ASSERT(sv[1] == "atad");

  sg = graph->getSubGraph(1);
  CPPUNIT_ASSERT(sg);

  b = false;
  CPPUNIT_ASSERT(sg->getAttribute("type = bool", b));
  CPPUNIT_ASSERT_EQUAL(sb, b);
  d = 0.;
  CPPUNIT_ASSERT(sg->getAttribute("type = double", d));
  CPPUNIT_ASSERT(d == sd);
  color = Color(0, 0, 0, 0);
  CPPUNIT_ASSERT(sg->getAttribute("type = color", color));
  CPPUNIT_ASSERT(color == scolor);
  coord = Coord(0., 0., 0);
  CPPUNIT_ASSERT(sg->getAttribute("type = coord", coord));
  CPPUNIT_ASSERT(coord == scoord);
  f = 0.;
  CPPUNIT_ASSERT(sg->getAttribute("type = float", f));
  CPPUNIT_ASSERT(f == sf);
  i = 0;
  CPPUNIT_ASSERT(sg->getAttribute("type = int", i));
  CPPUNIT_ASSERT(i == si);
  ui = 0;
  CPPUNIT_ASSERT(sg->getAttribute("type = uint", ui));
  CPPUNIT_ASSERT(ui == sui);
  size = Size(0., 0., 0.);
  CPPUNIT_ASSERT(sg->getAttribute("type = size", size));
  CPPUNIT_ASSERT(size == ssize);
  str = string("");
  CPPUNIT_ASSERT(sg->getAttribute("type = string", str));
  CPPUNIT_ASSERT(str == sstr);

  delete graph;
}
