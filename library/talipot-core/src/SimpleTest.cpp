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

#include <talipot/ConcatIterator.h>
#include <talipot/GraphTools.h>
#include <talipot/SimpleTest.h>
#include <talipot/UniqueIterator.h>
#include <talipot/VectorProperty.h>

using namespace std;
using namespace tlp;
//=================================================================
class SimpleTestListener : public Observable {
public:
  // override of Observable::treatEvent to remove the cached result for a graph if it is modified.
  void treatEvent(const Event &) override;

  /**
   * @brief Stored results for graphs. When a graph is updated, its entry is removed from the map.
   **/
  std::unordered_map<const Graph *, bool> resultsBuffer;

  void deleteResult(Graph *graph) {
    resultsBuffer.erase(graph);
    graph->removeListener(this);
  }
};

//=================================================================
void SimpleTestListener::treatEvent(const Event &evt) {
  const auto *gEvt = dynamic_cast<const GraphEvent *>(&evt);

  if (gEvt) {
    Graph *graph = gEvt->getGraph();

    switch (gEvt->getType()) {
    case GraphEventType::TLP_ADD_EDGE:

      if (resultsBuffer[graph]) {
        deleteResult(graph);
      }

      break;

    case GraphEventType::TLP_DEL_EDGE:

      if (!resultsBuffer[graph]) {
        deleteResult(graph);
      }

      break;

    case GraphEventType::TLP_REVERSE_EDGE:
      deleteResult(graph);
      break;

    default:
      break;
    }
  } else {

    auto *graph = static_cast<Graph *>(evt.sender());

    if (evt.type() == EventType::TLP_DELETE) {
      deleteResult(graph);
    }
  }
}
//=================================================================
static SimpleTestListener undirInstance;
static SimpleTestListener dirInstance;
static SimpleTestListener *getListener(bool directed) {
  if (directed) {
    return &dirInstance;
  } else {
    return &undirInstance;
  }
}
//=================================================================
bool SimpleTest::isSimple(const tlp::Graph *graph, const bool directed) {
  SimpleTestListener *listener = getListener(directed);
  auto &resultsBuffer = listener->resultsBuffer;

  if (const auto it = resultsBuffer.find(graph); it != resultsBuffer.end()) {
    return it->second;
  }

  graph->addListener(listener);
  auto [loops, parallelEdges] = getLoopsAndParallelEdges(graph, directed);
  return resultsBuffer[graph] = loops.empty() && parallelEdges.empty();
}
//**********************************************************************
vector<edge> SimpleTest::makeSimple(Graph *graph, const bool directed) {
  vector<edge> toRemove;
  if (!SimpleTest::isSimple(graph, directed)) {
    auto [loops, parallelEdges] = SimpleTest::getLoopsAndParallelEdges(graph, directed);

    // an edge can be a loop and a parallel one at the same time,
    // so ensure to deduplicate such edges before removing edges from graph
    auto *itEdges = uniqueIterator(concatIterator(stlIterator(loops), stlIterator(parallelEdges)));
    toRemove = iteratorVector(itEdges);

    for (edge e : toRemove) {
      graph->delEdge(e);
    }
  }

  assert(SimpleTest::isSimple(graph, directed));

  SimpleTestListener *listener = getListener(directed);
  listener->resultsBuffer[graph] = true;

  return toRemove;
}
//=================================================================
pair<vector<edge>, vector<edge>> SimpleTest::getLoopsAndParallelEdges(const tlp::Graph *graph,
                                                                      const bool directed) {

  vector<edge> loops, parallelEdges;

  EdgeVectorProperty<bool> visited(graph);
  visited.setAll(false);

  for (auto n : graph->nodes()) {
    set<node> seenOpposites;

    // search for parallel edges and loops
    for (auto e :
         getIncidentEdgesIterator(graph, n, directed ? EdgeType::DIRECTED : EdgeType::UNDIRECTED)) {

      // check if edge has already been visited
      if (visited[e]) {
        continue;
      }

      // mark edge as already visited
      visited[e] = true;
      node opposite = graph->opposite(e, n);

      if (opposite == n) { // loop
        loops.push_back(e);
      }

      if (seenOpposites.find(opposite) != seenOpposites.end()) { // parallel edge
        parallelEdges.push_back(e);
      } else {
        seenOpposites.insert(opposite);
      }
    }
  }

  return {loops, parallelEdges};
}
