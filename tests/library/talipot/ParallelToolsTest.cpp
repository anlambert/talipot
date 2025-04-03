
#include <talipot/VectorProperty.h>

#include "ParallelToolsTest.h"

const uint NB_NODES = 10000;
const uint NB_EDGES = 20000;

CPPUNIT_TEST_SUITE_REGISTRATION(ParallelToolsTest);

void ParallelToolsTest::setUp() {
    _graph = tlp::newGraph();
    _graph->addNodes(NB_NODES);
    for (uint i = 0; i < NB_EDGES; ++i) {
        std::ignore = i;
        _graph->addEdge(_graph->getRandomNode(), _graph->getRandomNode());
    }
    for (uint i = 0; i < NB_NODES / 3; ++i) {
        _graph->delNode(_graph->getRandomNode());
    }
}

void ParallelToolsTest::tearDown() {
    delete _graph;
}

void ParallelToolsTest::testParallelMapIndices() {
    std::vector<uint> v(100);
    tlp::TLP_PARALLEL_MAP_INDICES(v.size(), [&](uint i) { v[i] = 2 * i; });
    for (uint i = 0; i < v.size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(v[i], 2 * i);
    }
}

void ParallelToolsTest::testParallelMapNodes() {
    tlp::NodeVectorProperty<uint> deg(_graph);
    TLP_PARALLEL_MAP_NODES(_graph, [&](const tlp::node &n) { deg[n] = _graph->deg(n); });
    for (auto n : _graph->nodes()) {
        CPPUNIT_ASSERT_EQUAL(deg[n], _graph->deg(n));
    }
}

void ParallelToolsTest::testParallelMapNodesAndIndices() {
    std::vector<tlp::node> nodes(_graph->numberOfNodes());
    TLP_PARALLEL_MAP_NODES_AND_INDICES(_graph, [&](const tlp::node &n, uint i) { nodes[i] = n; });
    uint i = 0;
    for (auto n : _graph->nodes()) {
        CPPUNIT_ASSERT_EQUAL(n, nodes[i++]);
    }
}

void ParallelToolsTest::testParallelMapEdges() {
    tlp::EdgeVectorProperty<int> selfLoop(_graph);
    TLP_PARALLEL_MAP_EDGES(
        _graph, [&](const tlp::edge &e) { selfLoop[e] = _graph->source(e) == _graph->target(e); });
    for (auto e : _graph->edges()) {
        CPPUNIT_ASSERT_EQUAL(selfLoop[e], int(_graph->source(e) == _graph->target(e)));
    }
}

void ParallelToolsTest::testParallelMapEdgesAndIndices() {
    std::vector<tlp::edge> edges(_graph->numberOfEdges());
    TLP_PARALLEL_MAP_EDGES_AND_INDICES(_graph, [&](const tlp::edge &e, uint i) { edges[i] = e; });
    uint i = 0;
    for (auto e : _graph->edges()) {
        CPPUNIT_ASSERT_EQUAL(e, edges[i++]);
    }
}

void ParallelToolsTest::testCriticalSection() {
    uint maxDegPar = 0;
    TLP_PARALLEL_MAP_NODES(_graph, [&](const tlp::node &n) {
        uint deg = _graph->deg(n);
        TLP_LOCK_SECTION(maxDeg) {
            maxDegPar = std::max(deg, maxDegPar);
        }
        TLP_UNLOCK_SECTION(maxDeg);
    });
    uint maxDegSeq = 0;
    for (auto n : _graph->nodes()) {
        maxDegSeq = std::max(maxDegSeq, _graph->deg(n));
    }
    CPPUNIT_ASSERT_EQUAL(maxDegPar, maxDegSeq);
}

void ParallelToolsTest::testNumberOfThreads() {
    const uint vSize = 100;
    const uint nbThreads = 16;
    tlp::ThreadManager::setNumberOfThreads(nbThreads);
    std::vector<uint> tids(vSize);
    tlp::TLP_PARALLEL_MAP_INDICES(nbThreads,
                                  [&](uint i) { tids[i] = tlp::ThreadManager::getThreadNumber(); });

    for (uint tid : tids) {
        CPPUNIT_ASSERT(tid <= tlp::ThreadManager::getNumberOfThreads() - 1);
    }
}
