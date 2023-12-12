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

#include <talipot/DoubleProperty.h>
#include <talipot/FilterIterator.h>
#include <talipot/SortIterator.h>
#include <talipot/UniqueIterator.h>
#include <talipot/ConversionIterator.h>
#include <talipot/ConcatIterator.h>

#include "IteratorTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IteratorTest);

const uint numberOfNodes = 4;

void IteratorTest::setUp() {
  graph = tlp::newGraph();
  idMetric = graph->getDoubleProperty("idMetric");
  for (uint i = 0; i < numberOfNodes; ++i) {
    tlp::node n = graph->addNode();
    idMetric->setNodeValue(n, n.id);
  }
}

void IteratorTest::tearDown() {
  delete graph;
}

void IteratorTest::testIterator() {
  std::set<tlp::node> nodesSet;
  for (tlp::node n : graph->getNodes()) {
    nodesSet.insert(n);
  }
  CPPUNIT_ASSERT_EQUAL(numberOfNodes, static_cast<uint>(nodesSet.size()));
}

void IteratorTest::testFilterIterator() {
  std::set<tlp::node> nodesSet;
  for (tlp::node n :
       tlp::filterIterator(graph->getNodes(), [](const tlp::node &n) { return n.id % 2 == 0; })) {
    nodesSet.insert(n);
  }
  CPPUNIT_ASSERT_EQUAL(numberOfNodes / 2, static_cast<uint>(nodesSet.size()));
}

void IteratorTest::testSortIterator() {
  std::vector<int> inputVec = {2, 1, 3, 0};
  auto cmpFunc = [](const int &a, const int &b) { return a > b; };
  std::vector<int> sortedVec;
  for (int i : tlp::sortIterator(inputVec, cmpFunc)) {
    sortedVec.push_back(i);
  }
  std::vector<int> expected = {3, 2, 1, 0};
  for (size_t i = 0; i < sortedVec.size(); ++i) {
    CPPUNIT_ASSERT_EQUAL(expected[i], sortedVec[i]);
  }
}

void IteratorTest::testUniqueIterator() {
  std::vector<int> inputVec = {2, 0, 2, 5, 5, 0};

  std::vector<int> uniqueVec;
  for (int i : tlp::uniqueIterator(inputVec)) {
    uniqueVec.push_back(i);
  }
  std::vector<int> expected = {2, 0, 5};
  for (size_t i = 0; i < uniqueVec.size(); ++i) {
    CPPUNIT_ASSERT_EQUAL(expected[i], uniqueVec[i]);
  }

  auto check = [](int v) { return v % 2 == 0; };

  uniqueVec.clear();
  for (int i : tlp::uniqueIterator(inputVec, check)) {
    uniqueVec.push_back(i);
  }
  expected = {2, 0, 5, 5};
  for (size_t i = 0; i < uniqueVec.size(); ++i) {
    CPPUNIT_ASSERT_EQUAL(expected[i], uniqueVec[i]);
  }
}

void IteratorTest::testConversionIterator() {
  auto intToStr = [&](int i) { return std::to_string(i); };
  std::vector<int> inputVec = {1, 2, 3, 4};

  std::vector<std::string> convertedVec;
  for (const std::string &s : tlp::conversionIterator<std::string>(inputVec, intToStr)) {
    convertedVec.push_back(s);
  }
  std::vector<std::string> expected = {"1", "2", "3", "4"};
  for (size_t i = 0; i < convertedVec.size(); ++i) {
    CPPUNIT_ASSERT_EQUAL(expected[i], convertedVec[i]);
  }
}

void IteratorTest::testConcatIterator() {
  std::vector<std::string> inputVec1 = {"a", "b", "c"};
  std::vector<std::string> inputVec2 = {"d", "e", "f"};

  std::vector<std::string> concatVec;
  for (const std::string &s :
       tlp::concatIterator(tlp::stlIterator(inputVec1), tlp::stlIterator(inputVec2))) {
    concatVec.push_back(s);
  }

  std::vector<std::string> expected = inputVec1;
  expected.insert(expected.end(), inputVec2.begin(), inputVec2.end());
  for (size_t i = 0; i < concatVec.size(); ++i) {
    CPPUNIT_ASSERT_EQUAL(expected[i], concatVec[i]);
  }
}

void IteratorTest::testStableIterator() {
  uint notStableIterated = 0;
  uint stableIterated = 0;
  tlp::Iterator<tlp::node> *stableIt = tlp::stableIterator(graph->getNodes());
  graph->clear();
  notStableIterated = tlp::iteratorCount(graph->getNodes());
  stableIterated = tlp::iteratorCount(stableIt);
  CPPUNIT_ASSERT_EQUAL(notStableIterated, 0u);
  CPPUNIT_ASSERT_EQUAL(stableIterated, numberOfNodes);
}

void IteratorTest::testIteratorCount() {
  CPPUNIT_ASSERT_EQUAL(graph->numberOfNodes(), tlp::iteratorCount(graph->getNodes()));
}

void IteratorTest::testIteratorMap() {
  tlp::iteratorMap(graph->getNodes(),
                   [this](const tlp::node &n) { idMetric->setNodeValue(n, n.id + 1); });
  for (tlp::node n : graph->getNodes()) {
    CPPUNIT_ASSERT_EQUAL(n.id + 1.0, idMetric->getNodeValue(n));
  }
}

void IteratorTest::testIteratorReduce() {
  uint refVal = 0;
  for (tlp::node n : graph->getNodes()) {
    refVal += n.id;
  }
  uint val = tlp::iteratorReduce(graph->getNodes(), 0u,
                                 [](uint curVal, const tlp::node &n) { return curVal + n.id; });
  CPPUNIT_ASSERT_EQUAL(refVal, val);
}

void IteratorTest::testIteratorCountCheck() {
  for (uint i = 0; i < numberOfNodes; ++i) {
    CPPUNIT_ASSERT(tlp::iteratorCountCheck(graph->getNodes(), i + 1));
  }
  CPPUNIT_ASSERT(tlp::iteratorEmpty(graph->getEdges()));
}

void IteratorTest::testIteratorToStlContainers() {
  std::vector<tlp::node> vNodes = tlp::iteratorVector(graph->getNodes());
  std::set<tlp::node> sNodes = tlp::iteratorSet(graph->getNodes());
  std::list<tlp::node> lNodes = tlp::iteratorList(graph->getNodes());

  for (auto n : graph->nodes()) {
    CPPUNIT_ASSERT(std::find(lNodes.begin(), lNodes.end(), n) != lNodes.end());
    CPPUNIT_ASSERT(std::find(vNodes.begin(), vNodes.end(), n) != vNodes.end());
    CPPUNIT_ASSERT(sNodes.contains(n));
  }
}

void IteratorTest::testStlMapIterator() {
  std::map<std::string, int> m{{"key1", 3}, {"key2", 67}, {"key3", -34}};
  size_t size = 0;
  for (const auto &kv : tlp::stlMapIterator(m)) {
    auto it = m.find(kv.first);
    CPPUNIT_ASSERT(it != m.end());
    CPPUNIT_ASSERT_EQUAL(kv.second, it->second);
    ++size;
  }
  CPPUNIT_ASSERT_EQUAL(size, m.size());
  size = 0;
  for (const auto &k : tlp::stlMapKeyIterator(m)) {
    auto it = m.find(k);
    CPPUNIT_ASSERT(it != m.end());
    ++size;
  }
  CPPUNIT_ASSERT_EQUAL(size, m.size());
}
