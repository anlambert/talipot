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

#include <talipot/BooleanProperty.h>
#include <talipot/GraphProperty.h>

#include "IsMetaEdgeTest.h"

using namespace tlp;
using namespace std;

const uint nbNodesPerCluster = 5;

void createCompleteGraphAndSelect(Graph *graph, BooleanProperty *selection) {

    vector<node> addedNodes;

    for (uint j = 0; j < nbNodesPerCluster; ++j) {
        node n = graph->addNode();
        addedNodes.push_back(n);
        selection->setNodeValue(n, true);
    }

    for (size_t i = 0; i < addedNodes.size(); ++i) {
        for (size_t j = 0; j < addedNodes.size(); ++j) {
            if (addedNodes[i] != addedNodes[j]) {
                edge e = graph->addEdge(addedNodes[i], addedNodes[j]);
                selection->setEdgeValue(e, true);
            }
        }
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION(IsMetaEdgeTest);

void IsMetaEdgeTest::setUp() {

    graph = tlp::newGraph();
    BooleanProperty selec(graph);

    // create a first complete graph that will be our first cluster
    createCompleteGraphAndSelect(graph, &selec);
    Graph *sg1 = graph->addSubGraph(&selec);

    selec.setAllNodeValue(false);
    selec.setAllEdgeValue(false);

    // create another complete graph that will be our second cluster
    createCompleteGraphAndSelect(graph, &selec);
    Graph *sg2 = graph->addSubGraph(&selec);

    // link each node of the first cluster to each node of the second cluster
    for (auto n1 : sg1->nodes()) {
        for (auto n2 : sg2->nodes()) {
            graph->addEdge(n1, n2);
        }
    }

    // Create our quotient graph, should contain two meta-nodes and one meta-edge
    quotientGraph = graph->addSubGraph();
    Iterator<Graph *> *itS = graph->getSubGraphs();
    vector<node> mNodes;
    graph->createMetaNodes(itS, quotientGraph, mNodes);
    delete itS;
}

void IsMetaEdgeTest::tearDown() {
    graph->delAllSubGraphs(quotientGraph);
    delete graph;
}

void IsMetaEdgeTest::testIsMetaEdge() {
    uint nbMetaNodes = 0;
    uint nbMetaEdges = 0;

    // check if we have two nodes and one edge in the quotient
    CPPUNIT_ASSERT_EQUAL(2u, quotientGraph->numberOfNodes());
    CPPUNIT_ASSERT_EQUAL(1u, quotientGraph->numberOfEdges());

    for (auto n : quotientGraph->nodes()) {
        if (quotientGraph->isMetaNode(n)) {
            ++nbMetaNodes;
        }
    }

    for (auto e : quotientGraph->edges()) {
        if (quotientGraph->isMetaEdge(e)) {
            ++nbMetaEdges;
        }
    }

    // check that the two quotient nodes are meta nodes
    CPPUNIT_ASSERT_EQUAL(2u, nbMetaNodes);

    set<edge> underlyingEdgesInMetaEdge =
        quotientGraph->getGraphProperty("viewMetaGraph")->getEdgeValue(quotientGraph->getOneEdge());

    // check the number of underlying edges in meta edge
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(nbNodesPerCluster * nbNodesPerCluster),
                         underlyingEdgesInMetaEdge.size());

    // check if the quotient edge is a meta edge
    CPPUNIT_ASSERT_EQUAL(1u, nbMetaEdges);
}
