/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "IntegerPropertyMinMaxSubgraphTest.h"

const std::string integerPropertyName = "integer property for purpose testing";

const int rootGraphMin = 1;
const int rootGraphMax = 100;

const int subgraphMin = 2;
const int subgraphMax = 50;

using namespace tlp;

CPPUNIT_TEST_SUITE_REGISTRATION(IntegerPropertyMinMaxSubgraphTest);

void IntegerPropertyMinMaxSubgraphTest::setUp() {
    graph = newGraph();
    IntegerProperty *intProperty = graph->getIntegerProperty(integerPropertyName);
    node n = graph->addNode();
    intProperty->setNodeValue(n, rootGraphMin);
    n = graph->addNode();
    intProperty->setNodeValue(n, rootGraphMax);
    subGraph = graph->addSubGraph();
    n = subGraph->addNode();
    intProperty->setNodeValue(n, subgraphMin);
    n = subGraph->addNode();
    intProperty->setNodeValue(n, subgraphMax);
}

void IntegerPropertyMinMaxSubgraphTest::tearDown() {
    delete graph;
}

void IntegerPropertyMinMaxSubgraphTest::testIntegerPropertyRootGraphMin() {
    IntegerProperty *intProperty = graph->getIntegerProperty(integerPropertyName);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test IntegerProperty min value on root graph", rootGraphMin,
                                 intProperty->getNodeMin());
}

void IntegerPropertyMinMaxSubgraphTest::testIntegerPropertyRootGraphMax() {
    IntegerProperty *intProperty = graph->getIntegerProperty(integerPropertyName);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test IntegerProperty max value on root graph", rootGraphMax,
                                 intProperty->getNodeMax());
}

void IntegerPropertyMinMaxSubgraphTest::testIntegerPropertySubgraphMin() {
    IntegerProperty *intProperty = subGraph->getIntegerProperty(integerPropertyName);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test IntegerProperty min value on subgraph graph", subgraphMin,
                                 intProperty->getNodeMin(subGraph));
}

void IntegerPropertyMinMaxSubgraphTest::testIntegerPropertySubgraphMax() {
    IntegerProperty *intProperty = subGraph->getIntegerProperty(integerPropertyName);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test IntegerProperty max value on subgraph", subgraphMax,
                                 intProperty->getNodeMax(subGraph));
}
