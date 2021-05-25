/**
 *
 * Copyright (C) 2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <type_traits>

#include <talipot/Graph.h>
#include <talipot/Iterator.h>
#include <talipot/TlpTools.h>
#include <talipot/BooleanProperty.h>
#include <talipot/ColorProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/IntegerProperty.h>
#include <talipot/LayoutProperty.h>
#include <talipot/SizeProperty.h>
#include <talipot/StringProperty.h>

#include "CppUnitIncludes.h"

using namespace std;
using namespace tlp;

template <typename T>
struct is_vector {
  static const bool value = false;
};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> {
  static const bool value = true;
};

class PropertyProxyTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PropertyProxyTest);
  CPPUNIT_TEST(testBooleanProperty);
  CPPUNIT_TEST(testBooleanVectorProperty);
  CPPUNIT_TEST(testColorProperty);
  CPPUNIT_TEST(testColorVectorProperty);
  CPPUNIT_TEST(testDoubleProperty);
  CPPUNIT_TEST(testDoublePropertySetIntValues);
  CPPUNIT_TEST(testDoubleVectorProperty);
  CPPUNIT_TEST(testIntegerProperty);
  CPPUNIT_TEST(testIntegerVectorProperty);
  CPPUNIT_TEST(testLayoutProperty);
  CPPUNIT_TEST(testCoordVectorProperty);
  CPPUNIT_TEST(testSizeProperty);
  CPPUNIT_TEST(testSizeVectorProperty);
  CPPUNIT_TEST(testStringProperty);
  CPPUNIT_TEST(testStringVectorProperty);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
    graph = tlp::newGraph();
    n = graph->addNode();
    n2 = graph->addNode();
    e = graph->addEdge(n, n2);
    e2 = graph->addEdge(n2, n);
  }

  void tearDown() {
    delete graph;
  }

  template <typename TNode, typename TEdge>
  void testTypedProperty(const TNode &nodeValue, const TNode &nodeValue2, const TEdge &edgeValue,
                         const TEdge &edgeValue2, const string &expectedPropertyType) {

    string propName = "propTest";
    (*graph)[propName][n] = nodeValue;
    (*graph)[propName][n2] = nodeValue2;
    (*graph)[propName][e] = edgeValue2;
    (*graph)[propName][e2] = edgeValue;

    CPPUNIT_ASSERT_EQUAL(expectedPropertyType, graph->getProperty(propName)->getTypename());

    CPPUNIT_ASSERT(nodeValue == (*graph)[propName][n]);
    CPPUNIT_ASSERT((*graph)[propName][n] == nodeValue);
    CPPUNIT_ASSERT(edgeValue2 == (*graph)[propName][e]);
    CPPUNIT_ASSERT((*graph)[propName][e] == edgeValue2);
    CPPUNIT_ASSERT(nodeValue2 == (*graph)[propName][n2]);
    CPPUNIT_ASSERT(edgeValue == (*graph)[propName][e2]);

    if constexpr (is_same<TNode, TEdge>::value) {
      (*graph)[propName][n] = (*graph)[propName][n2];
      (*graph)[propName][e] = (*graph)[propName][e2];
      CPPUNIT_ASSERT((*graph)[propName][n] == (*graph)[propName][n2]);
      CPPUNIT_ASSERT(nodeValue2 == (*graph)[propName][n]);
      CPPUNIT_ASSERT((*graph)[propName][e] == (*graph)[propName][e2]);
      CPPUNIT_ASSERT(edgeValue == (*graph)[propName][e]);

      (*graph)[propName][n] = (*graph)[propName][e2];
      (*graph)[propName][e] = (*graph)[propName][n2];
      CPPUNIT_ASSERT((*graph)[propName][n] == (*graph)[propName][e2]);
      CPPUNIT_ASSERT(edgeValue == (*graph)[propName][n]);
      CPPUNIT_ASSERT((*graph)[propName][e] == (*graph)[propName][n2]);
      CPPUNIT_ASSERT(nodeValue2 == (*graph)[propName][e]);

      (*graph)[propName][n] = nodeValue;
      (*graph)[propName][e] = edgeValue2;
    }

    if constexpr (!is_vector<TNode>::value) {
      CPPUNIT_ASSERT(nodeValue < nodeValue2);

      CPPUNIT_ASSERT(nodeValue < (*graph)[propName][n2]);
      CPPUNIT_ASSERT(nodeValue <= (*graph)[propName][n2]);
      CPPUNIT_ASSERT(nodeValue2 > (*graph)[propName][n]);
      CPPUNIT_ASSERT(nodeValue2 >= (*graph)[propName][n]);

      CPPUNIT_ASSERT((*graph)[propName][n2] > nodeValue);
      CPPUNIT_ASSERT((*graph)[propName][n2] >= nodeValue);
      CPPUNIT_ASSERT((*graph)[propName][n] < nodeValue2);
      CPPUNIT_ASSERT((*graph)[propName][n] <= nodeValue2);

      CPPUNIT_ASSERT((*graph)[propName][n] != (*graph)[propName][n2]);
      CPPUNIT_ASSERT((*graph)[propName][n] < (*graph)[propName][n2]);
      CPPUNIT_ASSERT((*graph)[propName][n] <= (*graph)[propName][n2]);
      CPPUNIT_ASSERT((*graph)[propName][n2] > (*graph)[propName][n]);
      CPPUNIT_ASSERT((*graph)[propName][n2] >= (*graph)[propName][n]);
    }

    if constexpr (!is_vector<TEdge>::value) {
      CPPUNIT_ASSERT(edgeValue < edgeValue2);

      CPPUNIT_ASSERT(edgeValue2 > (*graph)[propName][e2]);
      CPPUNIT_ASSERT(edgeValue2 >= (*graph)[propName][e2]);
      CPPUNIT_ASSERT(edgeValue < (*graph)[propName][e]);
      CPPUNIT_ASSERT(edgeValue <= (*graph)[propName][e]);

      CPPUNIT_ASSERT((*graph)[propName][e] != (*graph)[propName][e2]);
      CPPUNIT_ASSERT((*graph)[propName][e] > (*graph)[propName][e2]);
      CPPUNIT_ASSERT((*graph)[propName][e] >= (*graph)[propName][e2]);
      CPPUNIT_ASSERT((*graph)[propName][e2] < (*graph)[propName][e]);
      CPPUNIT_ASSERT((*graph)[propName][e2] <= (*graph)[propName][e]);
    }

    (*graph)[propName].setAllNodeValue(nodeValue);
    CPPUNIT_ASSERT((*graph)[propName][graph->getRandomNode()] == nodeValue);
    node rn = graph->getRandomNode();
    (*graph)[propName][rn] = nodeValue2;
    CPPUNIT_ASSERT((*graph)[propName].hasNonDefaultValuatedNodes());
    CPPUNIT_ASSERT((*graph)[propName].numberOfNonDefaultValuatedNodes() == 1);
    CPPUNIT_ASSERT(iteratorVector((*graph)[propName].getNodesEqualTo(nodeValue2)) == vector{rn});

    (*graph)[propName].setAllEdgeValue(edgeValue);
    CPPUNIT_ASSERT((*graph)[propName][graph->getRandomEdge()] == edgeValue);
    edge re = graph->getRandomEdge();
    (*graph)[propName][re] = edgeValue2;
    CPPUNIT_ASSERT((*graph)[propName].hasNonDefaultValuatedEdges());
    CPPUNIT_ASSERT((*graph)[propName].numberOfNonDefaultValuatedEdges() == 1);
    CPPUNIT_ASSERT(iteratorVector((*graph)[propName].getEdgesEqualTo(edgeValue2)) == vector{re});
  }

  void testBooleanProperty() {
    testTypedProperty(false, true, false, true, BooleanProperty::propertyTypename);
  }

  void testColorProperty() {
    testTypedProperty(Color::Black, Color::White, Color::Blue, Color::Green,
                      ColorProperty::propertyTypename);
  }

  void testDoubleProperty() {
    testTypedProperty(1.5, 2.3, 4.3, 6.7, DoubleProperty::propertyTypename);
  }

  void testDoublePropertySetIntValues() {
    string propName = "doubleProp";
    node n = graph->getRandomNode();
    edge e = graph->getRandomEdge();
    int ival = 3;
    int ival2 = 7;
    (*graph)[propName].setAllNodeValue(0.0);
    (*graph)[propName][n] = ival;
    CPPUNIT_ASSERT((*graph)[propName][n] == double(ival));
    (*graph)[propName].setAllNodeValue(ival2);
    CPPUNIT_ASSERT((*graph)[propName][n] == double(ival2));

    (*graph)[propName].setAllEdgeValue(0.0);
    (*graph)[propName][e] = ival;
    CPPUNIT_ASSERT((*graph)[propName][e] == double(ival));
    (*graph)[propName].setAllEdgeValue(ival2);
    CPPUNIT_ASSERT((*graph)[propName][e] == double(ival2));
  }

  void testIntegerProperty() {
    testTypedProperty(2, 5, 8, 11, IntegerProperty::propertyTypename);
  }

  void testLayoutProperty() {
    Coord c1 = {1, 2, 3}, c2 = {4, 5, 6};
    vector<Coord> vc1 = {c1, c2};
    vector<Coord> vc2 = {c2, c1};
    testTypedProperty(c1, c2, vc1, vc2, LayoutProperty::propertyTypename);
  }

  void testSizeProperty() {
    Size s1 = {1, 2, 3}, s2 = {4, 5, 6}, s3 = {7, 8, 9}, s4 = {10, 11, 12};

    testTypedProperty(s1, s2, s3, s4, SizeProperty::propertyTypename);
  }

  void testStringProperty() {
    testTypedProperty(string("a"), string("b"), string("c"), string("d"),
                      StringProperty::propertyTypename);
  }

  void testBooleanVectorProperty() {
    vector<bool> vb1 = {false, false};
    vector<bool> vb2 = {true, true};
    vector<bool> vb3 = {false, true};
    vector<bool> vb4 = {true, false};
    testTypedProperty(vb1, vb2, vb3, vb4, BooleanVectorProperty::propertyTypename);
  }

  void testColorVectorProperty() {
    vector<Color> vc1 = {Color::Black, Color::White};
    vector<Color> vc2 = {Color::Red, Color::Blue};
    vector<Color> vc3 = {Color::Green, Color::Harlequin};
    vector<Color> vc4 = {Color::Indigo, Color::Jade};
    testTypedProperty(vc1, vc2, vc3, vc4, ColorVectorProperty::propertyTypename);
  }

  void testDoubleVectorProperty() {
    vector<double> vd1 = {0.5, 3.0};
    vector<double> vd2 = {6.7, 1.7};
    vector<double> vd3 = {7.8, 0.8};
    vector<double> vd4 = {7.6, 6.9};
    testTypedProperty(vd1, vd2, vd3, vd4, DoubleVectorProperty::propertyTypename);
  }

  void testIntegerVectorProperty() {
    vector<int> vi1 = {0, 3};
    vector<int> vi2 = {6, 1};
    vector<int> vi3 = {7, 0};
    vector<int> vi4 = {7, 6};
    testTypedProperty(vi1, vi2, vi3, vi4, IntegerVectorProperty::propertyTypename);
  }

  void testCoordVectorProperty() {
    Coord c1 = {1, 2, 3}, c2 = {4, 5, 6};
    vector<Coord> vc1 = {c1, c2};
    vector<Coord> vc2 = {c2, c1};
    vector<Coord> vc3 = {c1, c1};
    vector<Coord> vc4 = {c2, c2};
    testTypedProperty(vc1, vc2, vc3, vc4, CoordVectorProperty::propertyTypename);
  }

  void testSizeVectorProperty() {
    Coord s1 = {1, 2, 3}, s2 = {4, 5, 6};
    vector<Size> vs1 = {s1, s2};
    vector<Size> vs2 = {s2, s1};
    vector<Size> vs3 = {s1, s1};
    vector<Size> vs4 = {s2, s2};
    testTypedProperty(vs1, vs2, vs3, vs4, SizeVectorProperty::propertyTypename);
  }

  void testStringVectorProperty() {
    vector<string> vs1 = {"foo", "bar"};
    vector<string> vs2 = {"baz", "foo"};
    vector<string> vs3 = {"bar", "baz"};
    vector<string> vs4 = {"foo", "baz"};
    testTypedProperty(vs1, vs2, vs3, vs4, StringVectorProperty::propertyTypename);
  }

private:
  tlp::Graph *graph;
  node n, n2;
  edge e, e2;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PropertyProxyTest);
