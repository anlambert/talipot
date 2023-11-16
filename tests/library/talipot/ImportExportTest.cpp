/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "ImportExportTest.h"

#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/GraphProperty.h>

#include <talipot/ExportModule.h>
#include <talipot/StringCollection.h>

using namespace tlp;
using namespace std;

ImportExportTest::ImportExportTest(const string &importAlgorithm, const string &exportAlgorithm)
    : importAlgorithm(importAlgorithm), exportAlgorithm(exportAlgorithm) {}

void ImportExportTest::setUp() {
  CppUnit::TestFixture::setUp();

  CPPUNIT_ASSERT_MESSAGE("Export plugin not loaded: " + exportAlgorithm,
                         PluginsManager::pluginExists(exportAlgorithm));
  CPPUNIT_ASSERT_MESSAGE("Import plugin not loaded: " + importAlgorithm,
                         PluginsManager::pluginExists(importAlgorithm));
}

void ImportExportTest::testgridImportExport() {
  Graph *original = createSimpleGraph();

  exportImportGraph(original);

  delete original;
}

void ImportExportTest::testgridImportExportNonAsciiPath() {
  Graph *original = createSimpleGraph();

  exportImportGraph(original, "graph_export_éà");

  delete original;
}

void ImportExportTest::testAttributes() {
  Graph *original = createSimpleGraph();

  const string originalName = "rootGraph";
  const bool b = true;
  const double d = 3.14;
  const float f = 0.2f;
  const int i = 33;
  const uint ui = 256;
  const long l = 56845725;
  const Coord c = {1, 2, 3};
  const Coord c2 = {10, 20, 30};
  const Color col = Color::Azure;
  const Size s = {4, 5, 6};
  vector<string> vs;
  vs.push_back("foo");
  vs.push_back("bar");
  vector<double> vd;
  vd.push_back(12.56);
  vd.push_back(45.85);
  vector<int> vi;
  vi.push_back(45);
  vi.push_back(-120);
  vector<bool> vb;
  vb.push_back(true);
  vb.push_back(false);
  vector<Coord> vc;
  vc.push_back(c);
  vc.push_back(c2);
  set<edge> setEdge;
  vector<edge> vectorEdge;
  vector<node> vectorNode;
  for (auto n : original->nodes()) {
    vectorNode.push_back(n);
  }
  for (auto e : original->edges()) {
    vectorEdge.push_back(e);
    setEdge.insert(e);
  }
  node n = original->getOneNode();
  edge e = original->getOneEdge();
  StringCollection sc;
  sc.push_back("foo");
  sc.push_back("bar");

  DataSet dataSet;
  dataSet.set("name", originalName);
  dataSet.set("boolean", b);
  dataSet.set("double", d);
  dataSet.set("float", f);
  dataSet.set("integer", i);
  dataSet.set("unsigned integer", ui);
  dataSet.set("long", l);
  dataSet.set("tlp::Coord", c);
  dataSet.set("tlp::Size", s);
  dataSet.set("tlp::Color", col);
  dataSet.set("tlp::node", n);
  dataSet.set("tlp::edge", e);
  dataSet.set("tlp::StringCollection", sc);
  dataSet.set("vector<string>", vs);
  dataSet.set("vector<double>", vd);
  dataSet.set("vector<int>", vi);
  dataSet.set("vector<bool>", vb);
  dataSet.set("vector<node>", vectorNode);
  dataSet.set("vector<edge>", vectorEdge);
  dataSet.set("vector<Coord>", vc);
  dataSet.set("set<edge>", setEdge);

  original->setAttribute("name", originalName);
  original->setAttribute("boolean", b);
  original->setAttribute("double", d);
  original->setAttribute("float", f);
  original->setAttribute("integer", i);
  original->setAttribute("unsigned integer", ui);
  original->setAttribute("long", l);
  original->setAttribute("tlp::Coord", c);
  original->setAttribute("tlp::Size", s);
  original->setAttribute("tlp::Color", col);
  original->setAttribute("tlp::node", n);
  original->setAttribute("tlp::edge", e);
  original->setAttribute("tlp::StringCollection", sc);
  original->setAttribute("vector<string>", vs);
  original->setAttribute("vector<double>", vd);
  original->setAttribute("vector<int>", vi);
  original->setAttribute("vector<bool>", vb);
  original->setAttribute("vector<node>", vectorNode);
  original->setAttribute("vector<edge>", vectorEdge);
  original->setAttribute("vector<Coord>", vc);
  original->setAttribute("set<edge>", setEdge);
  original->setAttribute("tlp::DataSet", dataSet);

  exportImportGraph(original);

  delete original;
}

void ImportExportTest::testSubGraphsImportExport() {
  Graph *original = createSimpleGraph();
  const string originalName = "rootGraph";
  original->setAttribute("name", originalName);

  std::vector<node> sub1nodes;
  int sub1lowerBound = 5;
  int sub1higherBound = 25;
  std::vector<node> sub2nodes;
  int sub2lowerBound = 50;
  int sub2higherBound = 100;
  std::vector<node> subsubnodes;
  int subsublowerBound = 75;
  int subsubhigherBound = 100;
  int i = 0;
  for (auto n : original->nodes()) {
    if (i >= sub1lowerBound && i < sub1higherBound) {
      sub1nodes.push_back(n);
    }

    if (i >= sub2lowerBound && i < sub2higherBound) {
      sub2nodes.push_back(n);

      if (i >= subsublowerBound && i < subsubhigherBound) {
        subsubnodes.push_back(n);
      }
    }

    ++i;
  }
  const string sub1name = "sub1";
  const string sub2name = "sub2";
  const string subsubname = "subsub";
  Graph *sub1 = original->inducedSubGraph(sub1nodes);
  Graph *sub0 = original->addSubGraph();
  Graph *sub2 = original->inducedSubGraph(sub2nodes);
  Graph *subsub = sub2->inducedSubGraph(subsubnodes);
  sub1->setAttribute("name", sub1name);
  sub2->setAttribute("name", sub2name);
  subsub->setAttribute("name", subsubname);
  // we delete sub0 to ensure its id will be missing
  original->delSubGraph(sub0);

  i = 0;
  IntegerProperty *sub1id = sub1->getLocalIntegerProperty("sub1id");
  for (auto n : sub1->nodes()) {
    sub1id->setNodeValue(n, i++);
  }

  i = 0;
  IntegerProperty *sub2id = sub2->getLocalIntegerProperty("sub2id");
  for (auto n : sub2->nodes()) {
    sub2id->setNodeValue(n, i++);
  }

  i = 0;
  IntegerProperty *subsubid = subsub->getLocalIntegerProperty("subsubid");
  for (auto n : subsub->nodes()) {
    subsubid->setNodeValue(n, i++);
  }

  exportImportGraph(original);
  exportImportGraph(sub1);
  exportImportGraph(sub2);
  exportImportGraph(subsub);

  delete original;
}

static Color genRandomColor() {
  return Color(uchar(tlp::randomNumber(255)), uchar(tlp::randomNumber(255)),
               uchar(tlp::randomNumber(255)), uchar(tlp::randomNumber(255)));
}

static Coord genRandomCoord() {
  return tlp::Coord(float(tlp::randomNumber(1000)), float(tlp::randomNumber(1000)),
                    float(tlp::randomNumber(1000)));
}

static Size genRandomSize() {
  return tlp::Size(float(tlp::randomNumber(10)), float(tlp::randomNumber(10)),
                   float(tlp::randomNumber(10)));
}

Graph *ImportExportTest::createSimpleGraph() const {
  Graph *original = tlp::newGraph();
  LayoutProperty *layout = original->getLayoutProperty("viewLayout");

  for (uint i = 0; i < 100; ++i) {
    node n = original->addNode();
    layout->setNodeValue(n, Coord(i % 11, i / 10));
  }

  for (int x = 0; x < 10; ++x) {
    for (int y = 0; y < 10; ++y) {
      node origin(x + 10 * y);

      for (int xDelta = -1; xDelta <= 1; ++xDelta) {
        for (int yDelta = -1; yDelta <= 1; ++yDelta) {
          if (x + xDelta >= 0 && x + xDelta <= 9 && y + yDelta >= 0 && y + yDelta <= 9) {
            node destination(x + xDelta + 10 * (y + yDelta));
            original->addEdge(origin, destination);
          }
        }
      }
    }
  }

  updateIdProperty(original);

  // create and populate any supported graph properties types in Talipot
  // to check that their serialization/deserialization is correct

  BooleanProperty *booleanProp = original->getBooleanProperty("booleanProp");
  ColorProperty *colorProp = original->getColorProperty("colorProp");
  DoubleProperty *doubleProp = original->getDoubleProperty("doubleProp");
  IntegerProperty *integerProp = original->getIntegerProperty("intProp");
  LayoutProperty *layoutProp = original->getLayoutProperty("layoutProp");
  SizeProperty *sizeProp = original->getSizeProperty("sizeProp");
  StringProperty *stringProp = original->getStringProperty("stringProp");

  BooleanVectorProperty *booleanVecProp = original->getBooleanVectorProperty("booleanVecProp");
  ColorVectorProperty *colorVecProp = original->getColorVectorProperty("colorVecProp");
  DoubleVectorProperty *doubleVecProp = original->getDoubleVectorProperty("doubleVecProp");
  IntegerVectorProperty *integerVecProp = original->getIntegerVectorProperty("intVecProp");
  LayoutVectorProperty *coordVecProp = original->getCoordVectorProperty("coordVecProp");
  SizeVectorProperty *sizeVecProp = original->getSizeVectorProperty("sizeVecProp");
  StringVectorProperty *stringVecProp = original->getStringVectorProperty("stringVecProp");

  std::ostringstream oss;
  for (auto n : original->nodes()) {

    uint vecSize = tlp::randomNumber(9) + 1;
    vector<bool> boolVec;
    vector<Color> colorVec;
    vector<Coord> coordVec;
    vector<double> doubleVec;
    vector<int> intVec;
    vector<Size> sizeVec;
    vector<string> stringVec;
    for (uint i = 0; i < vecSize; ++i) {
      boolVec.push_back((n.id + i) % 2 == 0);
      coordVec.push_back(genRandomCoord());
      colorVec.push_back(genRandomColor());
      doubleVec.push_back(tlp::randomNumber(DBL_MAX));
      intVec.push_back(tlp::randomNumber(INT_MAX));
      sizeVec.push_back(genRandomSize());
      oss << tlp::randomNumber();
      stringVec.push_back(oss.str());
      oss.str("");
    }

    booleanProp->setNodeValue(n, n.id % 2 == 0);
    colorProp->setNodeValue(n, genRandomColor());
    doubleProp->setNodeValue(n, tlp::randomNumber(DBL_MAX));
    integerProp->setNodeValue(n, tlp::randomNumber(INT_MAX));
    layoutProp->setNodeValue(n, genRandomCoord());
    sizeProp->setNodeValue(n, genRandomSize());
    oss << "node " << n.id;
    stringProp->setNodeValue(n, oss.str());
    oss.str("");

    booleanVecProp->setNodeValue(n, boolVec);
    colorVecProp->setNodeValue(n, colorVec);
    doubleVecProp->setNodeValue(n, doubleVec);
    integerVecProp->setNodeValue(n, intVec);
    coordVecProp->setNodeValue(n, coordVec);
    sizeVecProp->setNodeValue(n, sizeVec);
    stringVecProp->setNodeValue(n, stringVec);
  }

  for (auto e : original->edges()) {

    uint vecSize = tlp::randomNumber(9) + 1;
    vector<bool> boolVec;
    vector<Color> colorVec;
    vector<Coord> coordVec;
    vector<double> doubleVec;
    vector<int> intVec;
    vector<Size> sizeVec;
    vector<string> stringVec;

    for (uint i = 0; i < vecSize; ++i) {
      boolVec.push_back((e.id + i) % 2 == 0);
      coordVec.push_back(genRandomCoord());
      colorVec.push_back(genRandomColor());
      doubleVec.push_back(tlp::randomNumber(DBL_MAX));
      intVec.push_back(tlp::randomNumber(INT_MAX));
      sizeVec.push_back(genRandomSize());
      oss << tlp::randomNumber();
      stringVec.push_back(oss.str());
      oss.str("");
    }

    booleanProp->setEdgeValue(e, e.id % 2 == 0);
    colorProp->setEdgeValue(e, genRandomColor());
    doubleProp->setEdgeValue(e, tlp::randomNumber(DBL_MAX));
    integerProp->setEdgeValue(e, tlp::randomNumber(INT_MAX));
    layoutProp->setEdgeValue(e, coordVec);
    sizeProp->setEdgeValue(e, genRandomSize());
    oss << "edge " << e.id;
    stringProp->setEdgeValue(e, oss.str());
    oss.str("");

    booleanVecProp->setEdgeValue(e, boolVec);
    colorVecProp->setEdgeValue(e, colorVec);
    doubleVecProp->setEdgeValue(e, doubleVec);
    integerVecProp->setEdgeValue(e, intVec);
    coordVecProp->setEdgeValue(e, coordVec);
    sizeVecProp->setEdgeValue(e, sizeVec);
    stringVecProp->setEdgeValue(e, stringVec);
  }

  return original;
}

void ImportExportTest::updateIdProperty(Graph *graph) const {
  IntegerProperty *id = graph->getIntegerProperty("id");
  for (auto n : graph->nodes()) {
    id->setNodeValue(n, n.id);
  }
  for (auto e : graph->edges()) {
    id->setEdgeValue(e, e.id);
  }
}

void ImportExportTest::testNanInfValuesImportExport() {
  Graph *original = createSimpleGraph();
  DoubleProperty *doubleProp = original->getDoubleProperty("doubleProp");
  DoubleVectorProperty *doubleVecProp = original->getDoubleVectorProperty("doubleVecProp");
  for (auto n : original->nodes()) {
    if (n.id % 3 == 0) {
      doubleProp->setNodeValue(n, std::numeric_limits<double>::quiet_NaN());
    } else if (n.id % 3 == 1) {
      doubleProp->setNodeValue(n, numeric_limits<float>::infinity());
    } else {
      doubleProp->setNodeValue(n, -numeric_limits<float>::infinity());
    }
  }
  vector<double> value;
  value.push_back(std::numeric_limits<double>::quiet_NaN());
  value.push_back(numeric_limits<float>::infinity());
  value.push_back(-numeric_limits<float>::infinity());
  doubleVecProp->setAllNodeValue(value);

  exportImportGraph(original);
  delete original;
}

void ImportExportTest::testMetaGraphImportExport() {
  Graph *graph = createSimpleGraph();

  // Add a groups subgraph and create a bunch of meta-nodes
  // and meta-edges
  Graph *groups = graph->addCloneSubGraph("groups");
  uint groupsSize = 20;
  uint nbNodes = graph->numberOfNodes();
  for (uint i = 0; i < nbNodes; i += groupsSize) {
    vector<node> group;
    group.reserve(groupsSize);
    for (uint j = i; j < i + groupsSize; ++j) {
      group.push_back(node(j));
    }
    groups->createMetaNode(group, false);
  }
  updateIdProperty(graph);

  // test the root graph and its meta information are correctly
  // exported then imported
  exportImportGraph(graph);
  // test the groups subgraph without meta information is correctly
  // exported then imported
  exportImportGraph(groups);
  delete graph;
}

void ImportExportTest::exportImportGraph(tlp::Graph *original, const string &exportBaseFilename) {

  const auto &exportPlugin =
      static_cast<const ExportModule &>(PluginsManager::pluginInformation(exportAlgorithm));

  vector<string> exts = {exportPlugin.fileExtension(), exportPlugin.gzipFileExtensions().front(),
                         exportPlugin.zstdFileExtensions().front()};

  for (const auto &ext : exts) {

    string exportFilename = exportBaseFilename + "." + ext;

    tlp::saveGraph(original, exportFilename);

    Graph *imported = tlp::loadGraph(exportFilename);

    // When exporting a leaf subgraph in the hierarchy containing meta-nodes,
    // it should be exported as a graph without meta-nodes and meta-edges
    // as the nodes and edges contained in those are not elements of the
    // exported graph.
    // So we remove meta information in the exported graph in order to successfully
    // tests its equality with the imported graph.
    if (original != original->getSuperGraph() && original->numberOfSubGraphs() == 0 &&
        original->existProperty("viewMetaGraph")) {
      GraphProperty *viewMetaGraph = original->getGraphProperty("viewMetaGraph");
      viewMetaGraph->setAllNodeValue(nullptr, original);
      viewMetaGraph->setAllEdgeValue(set<edge>(), original);
    }

    testGraphsAreEqual(original, imported);

    delete imported;
  }
}

void ImportExportTest::exportGraph(tlp::Graph *graph, const std::string &exportPluginName,
                                   const std::string &filename) {
  std::ostream *os = nullptr;

  if ((filename.rfind(".gz") == (filename.length() - 3)) ||
      (filename.rfind(".tlpz") == (filename.length() - 5)) ||
      (filename.rfind(".tlpbz") == (filename.length() - 6))) {
    os = tlp::getZlibOutputFileStream(filename);
  } else {
    os = tlp::getOutputFileStream(filename);
  }

  DataSet set;
  tlp::exportGraph(graph, *os, exportPluginName, set);

  delete os;
}

tlp::Graph *ImportExportTest::importGraph(const std::string &importPluginName,
                                          const std::string &filename) {
  DataSet input;
  input.set("file::filename", filename);
  return tlp::importGraph(importPluginName, input);
}

void ImportExportTest::testGraphsAreEqual(Graph *first, Graph *second) {
  CPPUNIT_ASSERT_MESSAGE("The import failed; the graph is null", second != nullptr);

  testGraphsTopologiesAreEqual(first, second);
  testGraphAttributesAreEqual(first, second);
  testGraphPropertiesAreEqual(first, second);

  CPPUNIT_ASSERT_EQUAL_MESSAGE("Graphs have different number of subgraphs",
                               first->numberOfSubGraphs(), second->numberOfSubGraphs());

  Iterator<Graph *> *firstSubGraphs = first->getSubGraphs();
  Iterator<Graph *> *secondSubGraphs = second->getSubGraphs();

  while (firstSubGraphs->hasNext() && secondSubGraphs->hasNext()) {
    Graph *firstSub = firstSubGraphs->next();
    Graph *secondSub = secondSubGraphs->next();

    testGraphsAreEqual(firstSub, secondSub);
  }
  delete firstSubGraphs;
  delete secondSubGraphs;
}

void ImportExportTest::testGraphAttributesAreEqual(tlp::Graph *first, tlp::Graph *second) {
  for (const std::pair<std::string, tlp::DataType *> &attribute :
       first->getAttributes().getValues()) {
    stringstream attributeNameMessage;

    attributeNameMessage << "attribute \"" << attribute.first
                         << "\" does not exists on imported graph.";
    CPPUNIT_ASSERT_MESSAGE(attributeNameMessage.str(), second->existAttribute(attribute.first));

    stringstream attributeTypeMessage;
    attributeTypeMessage << "attribute \"" << attribute.first
                         << "\" has different type on imported graph";
    tlp::DataType *secondAttr = second->getAttribute(attribute.first);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(attributeTypeMessage.str(), attribute.second->getTypeName(),
                                 secondAttr->getTypeName());

    stringstream attributeValueMessage;
    attributeValueMessage << "attribute \"" << attribute.first
                          << "\" has different value on imported graph";
    tlp::DataTypeSerializer *serializer =
        DataSet::typenameToSerializer(attribute.second->getTypeName());
    stringstream firstValue;
    stringstream secondValue;
    serializer->writeData(firstValue, attribute.second);
    serializer->writeData(secondValue, secondAttr);
    delete secondAttr;
    CPPUNIT_ASSERT_EQUAL_MESSAGE(attributeValueMessage.str(), firstValue.str(), secondValue.str());
  }
}

void ImportExportTest::testGraphPropertiesAreEqual(Graph *first, Graph *second) {
  uint firstPropertiesCount = iteratorCount(first->getObjectProperties());
  uint secondPropertiesCount = iteratorCount(second->getObjectProperties());
  IntegerProperty *secondIdProperty = second->getIntegerProperty("id");

  CPPUNIT_ASSERT_EQUAL_MESSAGE("Graphs have different number of properties", firstPropertiesCount,
                               secondPropertiesCount);

  for (const string &firstPropertyName : first->getProperties()) {
    PropertyInterface *firstProperty = first->getProperty(firstPropertyName);

    stringstream message;
    message << "the property " << firstPropertyName << " does not exist in the second graph !";
    CPPUNIT_ASSERT_MESSAGE(message.str(), second->existProperty(firstPropertyName));

    PropertyInterface *secondProperty = second->getProperty(firstPropertyName);

    message.str("");
    message << "a node value for property " << firstPropertyName
            << " in the first graph is not equal to the one in the second graph";

    for (auto n2 : second->nodes()) {
      node n1(secondIdProperty->getNodeValue(n2));
      CPPUNIT_ASSERT_EQUAL_MESSAGE(message.str(), firstProperty->getNodeStringValue(n1),
                                   secondProperty->getNodeStringValue(n2));
    }

    message.str("");
    message << "an edge value for property " << firstPropertyName
            << " in the first graph is not equal to the one in the second graph";

    for (auto e2 : second->edges()) {
      edge e1(secondIdProperty->getEdgeValue(e2));

      CPPUNIT_ASSERT_EQUAL_MESSAGE(message.str(), firstProperty->getEdgeStringValue(e1),
                                   secondProperty->getEdgeStringValue(e2));
    }
  }
}

void ImportExportTest::testGraphsTopologiesAreEqual(tlp::Graph *first, tlp::Graph *second) {
  stringstream nodesMessage;
  string name1, name2;
  first->getAttribute("name", name1);
  second->getAttribute("name", name2);
  nodesMessage << "Graphs '" << name1 << "' and '" << name2 << "' have different number of nodes";
  stringstream edgesMessage;
  edgesMessage << "Graphs " << name1 << " and " << name2 << " have different number of edges";
  CPPUNIT_ASSERT_EQUAL_MESSAGE(nodesMessage.str(), first->numberOfNodes(), second->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(edgesMessage.str(), first->numberOfEdges(), second->numberOfEdges());

  CPPUNIT_ASSERT_MESSAGE("id control property does not exists on original graph, please add it "
                         "before testing if graphs are equal.",
                         first->existProperty("id"));
  CPPUNIT_ASSERT_MESSAGE("id control property does not exists on imported graph",
                         second->existProperty("id"));

  IntegerProperty *firstIdProperty = first->getIntegerProperty("id");
  IntegerProperty *secondIdProperty = second->getIntegerProperty("id");

  CPPUNIT_ASSERT_MESSAGE("layout property does not exists on original graph",
                         first->existProperty("viewLayout"));
  CPPUNIT_ASSERT_MESSAGE("layout property does not exists on imported graph",
                         second->existProperty("viewLayout"));

  LayoutProperty *firstLayout = first->getLayoutProperty("viewLayout");
  LayoutProperty *secondLayout = second->getLayoutProperty("viewLayout");

  std::set<uint> fNodes;
  std::set<uint> sNodes;

  Iterator<node> *firstNodeIt = first->getNodes();
  Iterator<node> *secondNodeIt = second->getNodes();
  while (firstNodeIt->hasNext() && secondNodeIt->hasNext()) {
    fNodes.insert(firstNodeIt->next().id);
    sNodes.insert(secondIdProperty->getNodeValue(secondNodeIt->next()));
  }
  CPPUNIT_ASSERT_MESSAGE("all nodes of the first graph have not been iterated upon",
                         !firstNodeIt->hasNext());
  CPPUNIT_ASSERT_MESSAGE("all nodes of the second graph have not been iterated upon",
                         !secondNodeIt->hasNext());
  delete firstNodeIt;
  delete secondNodeIt;
  CPPUNIT_ASSERT_MESSAGE("the sets of nodes not are not equal", fNodes == sNodes);

  secondNodeIt = second->getNodes();
  while (secondNodeIt->hasNext()) {
    node secondNode = secondNodeIt->next();
    node firstNode(secondIdProperty->getNodeValue(secondNode));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("nodes do not have same id",
                                 firstIdProperty->getNodeValue(firstNode),
                                 secondIdProperty->getNodeValue(secondNode));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("nodes do not have same degree", first->deg(firstNode),
                                 second->deg(secondNode));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("nodes are not at the same position",
                                 firstLayout->getNodeStringValue(firstNode),
                                 secondLayout->getNodeStringValue(secondNode));
  }
  delete secondNodeIt;

  std::set<uint> fEdges;
  std::set<uint> sEdges;

  Iterator<edge> *firstEdgeIt = first->getEdges();
  Iterator<edge> *secondEdgeIt = second->getEdges();
  while (firstEdgeIt->hasNext() && secondEdgeIt->hasNext()) {
    fEdges.insert(firstEdgeIt->next().id);
    sEdges.insert(secondIdProperty->getEdgeValue(secondEdgeIt->next()));
  }
  CPPUNIT_ASSERT_MESSAGE("all edges of the first graph have not been iterated upon",
                         !firstEdgeIt->hasNext());
  CPPUNIT_ASSERT_MESSAGE("all edges of the second graph have not been iterated upon",
                         !secondEdgeIt->hasNext());

  delete firstEdgeIt;
  delete secondEdgeIt;
  assert(fEdges == sEdges);

  // subgraphs test
  for (Graph *firstSub : first->getDescendantGraphs()) {
    string name(firstSub->getName());
    Graph *secondSub = second->getDescendantGraph(name);
    CPPUNIT_ASSERT_MESSAGE("a subgraph is missing", secondSub != nullptr);
    CPPUNIT_ASSERT_MESSAGE("subgraphs have not the same id",
                           firstSub->getId() == secondSub->getId());
  }
}

void TalipotSaveLoadGraphFunctionsTest::testTalipotSaveLoadGraphFunctions() {
  tlp::Graph *graph = createSimpleGraph();
  tlp::Graph *loadedGraph = nullptr;
  string exportFilename;

  exportFilename = "test_tlp_export_import.tlp";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLP Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLP Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_tlp_gz_export_import.tlp.gz";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLP Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLP Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_tlp_gz_export_import.tlpz";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLP Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLP Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_tlpb_export_import.tlpb";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLPB Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLPB Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_tlpb_gz_export_import.tlpb.gz";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLPB Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLPB Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_tlpb_gz_export_import.tlpbz";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("TLPB Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "TLPB Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  exportFilename = "test_json_export_import.json";
  tlp::saveGraph(graph, exportFilename);
  loadedGraph = importGraph("JSON Import", exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;
  exportGraph(graph, "JSON Export", exportFilename);
  loadedGraph = tlp::loadGraph(exportFilename);
  testGraphsAreEqual(graph, loadedGraph);
  delete loadedGraph;

  delete graph;
}

void TalipotSaveLoadGraphFunctionsTest::testSaveGraphNoSuchFile() {
  tlp::Graph *graph = createSimpleGraph();
  CPPUNIT_ASSERT(!tlp::saveGraph(graph, "invalid/path/to/save/graph.tlp"));
  CPPUNIT_ASSERT(!tlp::saveGraph(graph, "invalid/path/to/save/graph.tlp.gz"));
  CPPUNIT_ASSERT(!tlp::saveGraph(graph, "invalid/path/to/save/graph.tlp.zst"));
  delete graph;
}

CPPUNIT_TEST_SUITE_REGISTRATION(TlpImportExportTest);

TlpImportExportTest::TlpImportExportTest() : ImportExportTest("TLP Import", "TLP Export") {}

CPPUNIT_TEST_SUITE_REGISTRATION(TlpBImportExportTest);

TlpBImportExportTest::TlpBImportExportTest() : ImportExportTest("TLPB Import", "TLPB Export") {}

CPPUNIT_TEST_SUITE_REGISTRATION(JsonImportExportTest);

JsonImportExportTest::JsonImportExportTest() : ImportExportTest("JSON Import", "JSON Export") {}

CPPUNIT_TEST_SUITE_REGISTRATION(TalipotSaveLoadGraphFunctionsTest);

TalipotSaveLoadGraphFunctionsTest::TalipotSaveLoadGraphFunctionsTest() : ImportExportTest("", "") {}
