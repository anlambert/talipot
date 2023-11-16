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

#include "DoublePropertyTest.h"

#include <talipot/VectorProperty.h>

using namespace tlp;
using namespace std;

const string doublePropertyName = "double property test";

const double originalMin = 5;
const double originalMax = 10;

const double newMin = 1;
const double newMax = 15;

CPPUNIT_TEST_SUITE_REGISTRATION(DoublePropertyTest);

void DoublePropertyTest::setUp() {
  graph = newGraph();
  graph->getLocalDoubleProperty(doublePropertyName);
  n1 = graph->addNode();
  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n1, originalMin);

  n2 = graph->addNode();
  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n2, 6);

  n3 = graph->addNode();
  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n3, 7);

  n4 = graph->addNode();
  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n4, originalMax);

  e1 = graph->addEdge(n1, n3);
  e2 = graph->addEdge(n2, n4);
}

void DoublePropertyTest::tearDown() {
  delete graph;
}

void DoublePropertyTest::testAnonymousDoublePropertyMaxUpdate() {
  DoubleProperty prop(graph);
  double maxNode;

  maxNode = prop.getNodeMax();
  CPPUNIT_ASSERT(maxNode == 0.0);
  prop.setNodeValue(n1, newMax);
  maxNode = prop.getNodeMax();
  CPPUNIT_ASSERT(maxNode == newMax);
}

void DoublePropertyTest::testDoublePropertyMinUpdate() {
  double minNode;

  minNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMin();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty min value before update", originalMin, minNode);

  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n1, newMin);
  minNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMin();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty min value after update", newMin, minNode);
}

void DoublePropertyTest::testDoublePropertyMaxUpdate() {
  double maxNode;

  maxNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMax();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty max value before update", originalMax, maxNode);

  graph->getLocalDoubleProperty(doublePropertyName)->setNodeValue(n4, newMax);
  maxNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMax();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty max value after update", newMax, maxNode);
}

void DoublePropertyTest::testDoublePropertyMinUpdateFromString() {
  double minNode;

  minNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMin();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty min value before update", originalMin, minNode);

  const string newStringMin = "1";

  graph->getLocalDoubleProperty(doublePropertyName)->setNodeStringValue(n1, newStringMin);
  minNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMin();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty min value after update", newMin, minNode);
}

void DoublePropertyTest::testDoublePropertyMaxUpdateFromString() {
  double maxNode;

  maxNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMax();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty max value before update", originalMax, maxNode);

  const string newStringMax = "15";

  graph->getLocalDoubleProperty(doublePropertyName)->setNodeStringValue(n4, newStringMax);
  maxNode = graph->getLocalDoubleProperty(doublePropertyName)->getNodeMax();
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test DoubleProperty max value after update", newMax, maxNode);
}

void DoublePropertyTest::testDoublePropertySubGraphMin() {
  DoubleProperty *doubleProperty = graph->getDoubleProperty(doublePropertyName);
  Graph *subGraph = graph->addSubGraph();
  node n2 = subGraph->addNode();
  doubleProperty->setNodeValue(n2, 6);
  node n3 = subGraph->addNode();
  doubleProperty->setNodeValue(n3, 9);
  CPPUNIT_ASSERT_EQUAL(originalMin, doubleProperty->getNodeMin());
  CPPUNIT_ASSERT_EQUAL(6.0, doubleProperty->getNodeMin(subGraph));

  subGraph->delNode(n2);
  CPPUNIT_ASSERT_EQUAL(9.0, doubleProperty->getNodeMin(subGraph));
  CPPUNIT_ASSERT_EQUAL(originalMin, doubleProperty->getNodeMin());
  graph->delNode(n1);
  CPPUNIT_ASSERT_EQUAL(9.0, doubleProperty->getNodeMin(subGraph));
  CPPUNIT_ASSERT_EQUAL(6.0, doubleProperty->getNodeMin());
}

void DoublePropertyTest::testDoublePropertySubGraphMax() {
  DoubleProperty *doubleProperty = graph->getDoubleProperty(doublePropertyName);
  Graph *subGraph = graph->addSubGraph();
  node n2 = subGraph->addNode();
  doubleProperty->setNodeValue(n2, 6.0);
  node n3 = subGraph->addNode();
  doubleProperty->setNodeValue(n3, 9.0);
  CPPUNIT_ASSERT_EQUAL(doubleProperty->getNodeMax(), originalMax);
  CPPUNIT_ASSERT_EQUAL(9.0, doubleProperty->getNodeMax(subGraph));

  subGraph->delNode(n3);
  CPPUNIT_ASSERT_EQUAL(doubleProperty->getNodeMax(), originalMax);
  CPPUNIT_ASSERT_EQUAL(6.0, doubleProperty->getNodeMax(subGraph));
  graph->delNode(n4);
  CPPUNIT_ASSERT(doubleProperty->getNodeMax(subGraph) == 6);
  CPPUNIT_ASSERT(doubleProperty->getNodeMax() == 9);
}

void DoublePropertyTest::testDoublePropertyInfValue() {
  double zero = 0.0;
  double infValue = 1.0 / zero;

  CPPUNIT_ASSERT(infValue == std::numeric_limits<double>::infinity());
  CPPUNIT_ASSERT(-infValue == -std::numeric_limits<double>::infinity());

  node n = graph->addNode();

  DoubleProperty *prop = graph->getLocalDoubleProperty(doublePropertyName);
  CPPUNIT_ASSERT(prop->getNodeValue(n) == 0.0);

  prop->setNodeValue(n, infValue);
  CPPUNIT_ASSERT(prop->getNodeValue(n) == infValue);

  prop->setNodeValue(n, 1.0);
  CPPUNIT_ASSERT(prop->getNodeValue(n) == 1.0);

  prop->setNodeValue(n, -infValue);
  CPPUNIT_ASSERT(prop->getNodeValue(n) == -infValue);

  prop->setNodeValue(n, 1.0);
  CPPUNIT_ASSERT(prop->getNodeValue(n) == 1.0);

  prop->setNodeStringValue(n, "inf");
  CPPUNIT_ASSERT(prop->getNodeValue(n) == infValue);

  prop->setNodeStringValue(n, "-inf");
  CPPUNIT_ASSERT(prop->getNodeValue(n) == -infValue);
}

void DoublePropertyTest::testDoublePropertySetAllValue() {

  // create a subgraph
  Graph *sg = graph->addSubGraph();
  sg->addNode(graph->source(e1));
  sg->addNode(graph->target(e1));
  sg->addEdge(e1);

  const double v1 = tlp::randomNumber();
  const double v2 = tlp::randomNumber();

  // create a double property and set all values for nodes and edges
  DoubleProperty *prop = graph->getLocalDoubleProperty(doublePropertyName);
  prop->setAllNodeValue(v1);
  prop->setAllEdgeValue(v2);

  // check that the default property value has been correctly modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v1, 1e-6);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v2, 1e-6);

  // check that each node has the correct value
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n), v1, 1e-6);
  }
  // check that the default node value has been changed
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v1, 1e-6);

  // check that each edge has the correct value
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeValue(e), v2, 1e-6);
  }
  // check that the default edge value has been changed
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v2, 1e-6);

  // set different values for the nodes and edges of the subgraph
  prop->setAllNodeValue(v2, sg);
  prop->setAllEdgeValue(v1, sg);

  // check that the default property value has not been modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v1, 1e-6);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v2, 1e-6);

  // check that the nodes have expected values
  for (auto n : graph->nodes()) {
    if (sg->isElement(n)) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n), v2, 1e-6);
    } else {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n), v1, 1e-6);
    }
  }
  // check that the default node value has not been modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v1, 1e-6);

  // check that the edges have expected values
  for (auto e : graph->edges()) {
    if (sg->isElement(e)) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeValue(e), v1, 1e-6);
    } else {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeValue(e), v2, 1e-6);
    }
  }
  // check that the default edge value has not been modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v2, 1e-6);
}

void DoublePropertyTest::testDoublePropertySetDefaultValue() {

  const double v1 = tlp::randomNumber();
  const double v2 = tlp::randomNumber();

  // create a double property and set all values for nodes and edges
  DoubleProperty *prop = graph->getLocalDoubleProperty(doublePropertyName);
  prop->setAllNodeValue(v1);
  prop->setAllEdgeValue(v2);

  // check number of non default valuated elements
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedNodes(), 0u);
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedEdges(), 0u);

  // check that the default property value has been correctly modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v1, 1e-6);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v2, 1e-6);

  // set value of n1 to future default value
  prop->setNodeValue(n1, v2);
  // check non default valuated nodes
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedNodes(), 1u);
  // change the default node value for future added nodes
  prop->setNodeDefaultValue(v2);
  // check that the default property value has been correctly modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeDefaultValue(), v2, 1e-6);
  // check non default valuated nodes
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedNodes(), graph->numberOfNodes() - 1);
  // reset n1 prop value to v1
  prop->setNodeValue(n1, v1);

  // set value of e1 to future default value
  prop->setEdgeValue(e1, v1);
  // check non default valuated edges
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedEdges(), 1u);
  // change the default edge value for future added edges
  prop->setEdgeDefaultValue(v1);
  // check that the default property value has been correctly modified
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeDefaultValue(), v1, 1e-6);
  // check non default valuated edges
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedEdges(), graph->numberOfEdges() - 1);
  // reset value of e1 to v2
  prop->setEdgeValue(e1, v2);

  // check number of non default valuated elements
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedNodes(), graph->numberOfNodes());
  CPPUNIT_ASSERT_EQUAL(prop->numberOfNonDefaultValuatedEdges(), graph->numberOfEdges());

  // add a new node
  node nNew = graph->addNode();
  // add a new edge
  edge eNew = graph->addEdge(graph->getRandomNode(), graph->getRandomNode());

  // check that the new default property value has been correctly set
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(nNew), v2, 1e-6);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeValue(eNew), v1, 1e-6);

  // check that original nodes and edges still have the same value
  // as before modifying the default property value
  for (auto n : graph->nodes()) {
    if (n != nNew) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n), v1, 1e-6);
    }
  }
  for (auto e : graph->edges()) {
    if (e != eNew) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getEdgeValue(e), v2, 1e-6);
    }
  }

  // check if there is no graph push/pop side effect when setting the new default value
  // on a node that already has it
  graph->push();
  prop->setNodeValue(n1, v2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n1), v2, 1e-6);
  graph->pop();
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n1), v1, 1e-6);
  graph->unpop();
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(n1), v2, 1e-6);

  // check that after pushing a graph, adding a new node and changing the default property value
  // the node property value gets restored to the default value of the property the time the node
  // was created
  double v3 = tlp::randomNumber();
  // push graph state
  graph->push();
  // add a node, its property value should be v2
  node newNode = graph->addNode();
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(newNode), v2, 1e-6);
  // change the default property value to v3
  prop->setNodeDefaultValue(v3);
  // pop graph state
  graph->pop();
  // unpop graph state
  graph->unpop();
  // node value should be v2
  CPPUNIT_ASSERT_DOUBLES_EQUAL(prop->getNodeValue(newNode), v2, 1e-6);
}

void DoublePropertyTest::testVectorDoublePropertyCopyFrom() {
  auto *prop = graph->getLocalDoubleProperty(doublePropertyName);
  NodeVectorProperty<double> nVectorProp(graph);
  nVectorProp.copyFromProperty(prop);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(nVectorProp[n] == prop->getNodeValue(n));
  }
  nVectorProp.setAll(1.1);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(nVectorProp[n] == 1.1);
  }
  nVectorProp.copyFromNumericProperty(prop);
  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(nVectorProp[n] == prop->getNodeValue(n));
  }
  for (auto e : graph->edges()) {
    prop->setEdgeValue(e, tlp::randomNumber());
  }
  EdgeVectorProperty<double> eVectorProp(graph);
  eVectorProp.copyFromProperty(prop);
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT(eVectorProp[e] == prop->getEdgeValue(e));
  }
  eVectorProp.setAll(1.1);
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT(eVectorProp[e] == 1.1);
  }
  eVectorProp.copyFromNumericProperty(prop);
  for (auto e : graph->edges()) {
    CPPUNIT_ASSERT(eVectorProp[e] == prop->getEdgeValue(e));
  }
}
